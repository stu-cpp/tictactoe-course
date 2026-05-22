#include "my_player.hpp"
#include <climits>
#include <vector>

namespace ttt::my_player {

 
// IPlayer: базовые методы 

void MyPlayer::set_sign(Sign sign) { m_sign = sign; }
const char *MyPlayer::get_name() const { return m_name; }


// Вспомогательные функции


// base^exp, целочисленно
int MyPlayer::ipow(int base, int exp) {
  int result = 1;
  for (int i = 0; i < exp; ++i)
    result *= base;
  return result;
}

// Шаг 2/3: проверяем, образует ли постановка знака sign в (x, y) линию >= L
// Не изменяет state — только смотрит на уже стоящие знаки + воображаемый ход
bool MyPlayer::check_win(const State &state, int x, int y, Sign sign) const {
  if (state.get_value(x, y) != Sign::NONE)
    return false;

  const int L    = state.get_opts().win_len;
  const int cols = state.get_opts().cols;
  const int rows = state.get_opts().rows;

  // 4 направления: горизонталь, вертикаль и две диагонали
  const int dirs[4][2] = {{1, 0}, {0, 1}, {1, 1}, {1, -1}};

  for (auto &d : dirs) {
    int count = 1; // сама клетка (x, y)

    // считаем в одну сторону
    for (int step = 1; step < L; ++step) {
      int nx = x + d[0] * step;
      int ny = y + d[1] * step;
      if (nx < 0 || nx >= cols || ny < 0 || ny >= rows) break;
      if (state.get_value(nx, ny) != sign) break;
      ++count;
    }
    // считаем в другую сторону
    for (int step = 1; step < L; ++step) {
      int nx = x - d[0] * step;
      int ny = y - d[1] * step;
      if (nx < 0 || nx >= cols || ny < 0 || ny >= rows) break;
      if (state.get_value(nx, ny) != sign) break;
      ++count;
    }

    if (count >= L)
      return true;
  }
  return false;
}


// Шаг 5: эвристика
// Формула: W = base^k * M
//   k — длина цепочки (1 <= k < L)
//   M — число открытых концов (0, 1, 2)
// Score = сумма_весов_O - сумма_весов_X


void MyPlayer::scan_line(const State &state, int x0, int y0, int dx, int dy, int len,  int &score_o, int &score_x) const {
  const int L    = state.get_opts().win_len;
  const int cols = state.get_opts().cols;
  const int rows = state.get_opts().rows;

  int i = 0;
  while (i < len) {
    int cx = x0 + dx * i;
    int cy = y0 + dy * i;
    Sign cur = state.get_value(cx, cy);

    // Пропускаем пустые клетки и стены
    if (cur == Sign::NONE || cur == Sign::WALL) {
      ++i;
      continue;
    }

    // Нашли начало цепочки (O или X)
    Sign chain_sign = cur;
    int start = i;
    int k = 0;

    // Считаем длину цепочки
    while (i < len) {
      int nx = x0 + dx * i;
      int ny = y0 + dy * i;
      if (state.get_value(nx, ny) != chain_sign) break;
      ++k;
      ++i;
    }

    // Цепочки длиной >= L не оцениваем эвристикой (это уже победа)
    if (k >= L)
      continue;

    // Считаем открытые концы (M)
    int open_ends = 0;

    // Левый конец (перед началом цепочки)
    if (start > 0) {
      int lx = x0 + dx * (start - 1);
      int ly = y0 + dy * (start - 1);
      if (lx >= 0 && lx < cols && ly >= 0 && ly < rows &&
          state.get_value(lx, ly) == Sign::NONE)
        ++open_ends;
    }

    // Правый конец (после конца цепочки)
    {
      int rx = x0 + dx * i;
      int ry = y0 + dy * i;
      if (i < len &&
          rx >= 0 && rx < cols && ry >= 0 && ry < rows &&
          state.get_value(rx, ry) == Sign::NONE)
        ++open_ends;
    }

    // W = base^k * M
    int w = ipow(m_base, k) * open_ends;

    if (chain_sign == Sign::O)
      score_o += w;
    else
      score_x += w;
  }
}

int MyPlayer::evaluate(const State &state) const {
  const int cols = state.get_opts().cols;
  const int rows = state.get_opts().rows;

  int score_o = 0, score_x = 0;

  // Горизонтали
  for (int y = 0; y < rows; ++y)
    scan_line(state, 0, y, 1, 0, cols, score_o, score_x);

  // Вертикали
  for (int x = 0; x < cols; ++x)
    scan_line(state, x, 0, 0, 1, rows, score_o, score_x);

  // Диагонали ↘ (сверху-слева вниз-вправо)
  for (int x = 0; x < cols; ++x) {
    int len = std::min(cols - x, rows);
    scan_line(state, x, 0, 1, 1, len, score_o, score_x);
  }
  for (int y = 1; y < rows; ++y) {
    int len = std::min(cols, rows - y);
    scan_line(state, 0, y, 1, 1, len, score_o, score_x);
  }

  // Диагонали ↗ (снизу-слева вверх-вправо)
  for (int x = 0; x < cols; ++x) {
    int len = std::min(cols - x, rows);
    scan_line(state, x, rows - 1, 1, -1, len, score_o, score_x);
  }
  for (int y = 0; y < rows - 1; ++y) {
    int len = std::min(cols, y + 1);
    scan_line(state, 0, y, 1, -1, len, score_o, score_x);
  }

  return score_o - score_x;
}


// Шаг 5: Minimax с альфа-бета отсечением
// Глубина D в полуходах. maximizing=true — ход O (компьютер).

int MyPlayer::minimax(State state, int depth, int alpha, int beta,
                      bool maximizing) const {
  const int cols = state.get_opts().cols;
  const int rows = state.get_opts().rows;
  Sign opp = (m_sign == Sign::O) ? Sign::X : Sign::O;

  // Терминальное состояние — если есть победитель
  if (state.get_winner() == m_sign)
    return 1000000 + depth;   // победа O — чем быстрее, тем лучше
  if (state.get_winner() == opp)
    return -1000000 - depth;  // победа X — чем дальше, тем лучше

  // Достигли дна — возвращаем эвристику
  if (depth == 0)
    return evaluate(state);

  // Собираем свободные клетки (только соседние с занятыми — оптимизация)
  std::vector<Point> candidates;
  std::vector<bool> visited(cols * rows, false);

  for (int x = 0; x < cols; ++x) {
    for (int y = 0; y < rows; ++y) {
      Sign v = state.get_value(x, y);
      if (v != Sign::X && v != Sign::O) continue;
      // смотрим клетки вокруг в радиусе 1
      for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
          int nx = x + dx, ny = y + dy;
          if (nx < 0 || nx >= cols || ny < 0 || ny >= rows) continue;
          if (state.get_value(nx, ny) != Sign::NONE) continue;
          int idx = ny * cols + nx;
          if (!visited[idx]) {
            visited[idx] = true;
            candidates.push_back({nx, ny});
          }
        }
      }
    }
  }

  // Если нет кандидатов — ничья
  if (candidates.empty())
    return evaluate(state);

  if (maximizing) {
    // Ход O: ищем максимум
    int best = INT_MIN;
    for (const Point &p : candidates) {
      State next = state;
      next.process_move(p);
      int score = minimax(next, depth - 1, alpha, beta, false);
      if (score > best) best = score;
      if (best > alpha) alpha = best;
      if (beta <= alpha) break; // β-отсечение
    }
    return best;
  } else {
    // Ход X: ищем минимум
    int best = INT_MAX;
    for (const Point &p : candidates) {
      State next = state;
      next.process_move(p);
      int score = minimax(next, depth - 1, alpha, beta, true);
      if (score < best) best = score;
      if (best < beta) beta = best;
      if (beta <= alpha) break; // α-отсечение
    }
    return best;
  }
}


// make_move — главный метод


Point MyPlayer::make_move(const State &state) {
  const int cols = state.get_opts().cols;
  const int rows = state.get_opts().rows;
  Sign opp = (m_sign == Sign::O) ? Sign::X : Sign::O;

  //  Шаг 1: сформировать список free_cells 
  std::vector<Point> free_cells;
  for (int x = 0; x < cols; ++x)
    for (int y = 0; y < rows; ++y)
      if (state.get_value(x, y) == Sign::NONE)
        free_cells.push_back({x, y});

  // Если ходов нет — сигнализируем (-1, -1)
  if (free_cells.empty())
    return {-1, -1};

  Point best_move = free_cells[0]; // fallback

  //  Шаг 2: тактика — немедленная победа O 
  for (const Point &p : free_cells) {
    if (check_win(state, p.x, p.y, m_sign)) {
      best_move = p;
      goto step6; // нашли — сразу к шагу 6
    }
  }

  //  Шаг 3: тактика — блокировка победы X 
  for (const Point &p : free_cells) {
    if (check_win(state, p.x, p.y, opp)) {
      best_move = p;
      goto step6;
    }
  }

  //  Шаг 4: обработка флага last_move_for_O 
  if (m_last_move_for_o) {
    // Пытаемся собрать линию (цель — ничья)
    for (const Point &p : free_cells) {
      if (check_win(state, p.x, p.y, m_sign)) {
        best_move = p;
        goto step6;
      }
    }
    // Ни одна клетка не даёт линию — снимаем флаг, идём к шагу 5
    m_last_move_for_o = false;
  }

  //  Шаг 5: стратегический выбор (minimax) 
  {
    int best_score = INT_MIN;

    for (const Point &p : free_cells) {
      // Ставим O и запускаем симуляцию на глубину D-1 (один полуход уже сделан)
      State next = state;
      next.process_move(p);

      int score = minimax(next, m_depth - 1, INT_MIN, INT_MAX, false); // следующий ход — X

      if (score > best_score) {
        best_score = score;
        best_move  = p;
      }
    }
  }

step6:
  //  Шаг 6: сброс флага, возврат хода 
  m_last_move_for_o = false;
  return best_move;
}

}; // namespace ttt::my_player
