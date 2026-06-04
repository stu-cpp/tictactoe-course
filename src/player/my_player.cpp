#include "my_player.hpp"
#include <cstdlib>
#include <ctime>
#include <vector>
#include <limits>
#include <chrono>
#include <iostream>

namespace ttt::my_player {

// ============================================================
// КОНСТРУКТОР И БАЗОВЫЕ МЕТОДЫ
// ============================================================

// Конструктор: сохраняем имя бота и инициализируем генератор случайных чисел
MyPlayer::MyPlayer(const char* name) : m_name(name) {
  std::srand(std::time(nullptr));  // инициализация rand() текущим временем
}

// Запоминаем, каким знаком играет бот (X или O)
void MyPlayer::set_sign(Sign sign) {
  m_sign = sign;
}

// Возвращаем имя бота (нужно для интерфейса IPlayer)
const char* MyPlayer::get_name() const {
  return m_name;
}

// ============================================================
// ВСПОМОГАТЕЛЬНЫЕ МЕТОДЫ ДЛЯ АЛГОРИТМА
// ============================================================

// ------------------------------------------------------------
// ПРОВЕРКА ВЫИГРЫШНОГО ХОДА (приоритет 1 - АТАКА)
// ------------------------------------------------------------
// Вход: состояние игры, клетка (move), знак игрока (player_sign)
// Выход: true, если после хода в эту клетку игрок выигрывает
bool MyPlayer::is_winning_move(const State& state, Point move, Sign player_sign) const {
  State copy = state;                           // создаём копию состояния (не меняем оригинал)
  if (copy.process_move(player_sign, move.x, move.y) != MoveResult::OK) {
    return false;                               // не удалось походить (клетка занята или вне поля)
  }
  return copy.get_winner() == player_sign;      // проверяем, появился ли победитель
}

// ------------------------------------------------------------
// ПРОВЕРКА БЛОКИРУЮЩЕГО ХОДА (приоритет 2 - ЗАЩИТА)
// ------------------------------------------------------------
// Вход: состояние игры, клетка (move), знак соперника (opponent_sign)
// Выход: true, если после хода соперника в эту клетку он выигрывает
bool MyPlayer::is_blocking_move(const State& state, Point move, Sign opponent_sign) const {
  State copy = state;                           // создаём копию состояния
  if (copy.process_move(opponent_sign, move.x, move.y) != MoveResult::OK) {
    return false;                               // не удалось походить
  }
  return copy.get_winner() == opponent_sign;    // проверяем, выиграл бы соперник
}

// ------------------------------------------------------------
// ПОДСЧЁТ ЗНАКОВ В РАДИУСЕ (квадрат 5x5)
// ------------------------------------------------------------
// Вход: состояние, центр (center), искомый знак (target_sign), радиус (radius)
// Выход: количество знаков target_sign в квадрате (2*radius+1) x (2*radius+1)
int MyPlayer::count_signs_in_radius(const State& state, Point center, Sign target_sign, int radius) const {
  int count = 0;
  const int rows = state.get_opts().rows;        // высота поля (20)
  const int cols = state.get_opts().cols;        // ширина поля (20)
  
  // Проходим по квадрату от -radius до +radius по X и Y
  for (int dy = -radius; dy <= radius; ++dy) {
    for (int dx = -radius; dx <= radius; ++dx) {
      int x = center.x + dx;                     // координата X соседней клетки
      int y = center.y + dy;                     // координата Y соседней клетки
      
      // Проверяем, что клетка находится в пределах поля
      if (x < 0 || x >= cols || y < 0 || y >= rows) continue;
      
      // Если в клетке нужный знак — увеличиваем счётчик
      if (state.get_value(x, y) == target_sign) {
        ++count;
      }
    }
  }
  return count;
}

// ------------------------------------------------------------
// ВЫЧИСЛЕНИЕ ВЕСА КЛЕТКИ (приоритет 3)
// ------------------------------------------------------------
// Формула веса:
//   +10 за каждый свой знак в радиусе 2 (атака)
//   +5  за каждый чужой знак в радиусе 2 (защита)
//   +3  за близость к центру (клетки 9,9 - 10,10)
int MyPlayer::calculate_weight(const State& state, Point cell) const {
  // Если клетка не пустая — возвращаем очень маленькое число (нельзя ходить)
  if (state.get_value(cell.x, cell.y) != Sign::NONE) {
    return -1000000;
  }
  
  int weight = 0;
  const int radius = 2;   // радиус 2 = квадрат 5x5
  
  // СЧИТАЕМ СВОИ ЗНАКИ В РАДИУСЕ (+10 каждый)
  int my_signs = count_signs_in_radius(state, cell, m_sign, radius);
  weight += my_signs * 10;
  
  // СЧИТАЕМ ЧУЖИЕ ЗНАКИ В РАДИУСЕ (+5 каждый)
  Sign opponent = (m_sign == Sign::X) ? Sign::O : Sign::X;
  int opponent_signs = count_signs_in_radius(state, cell, opponent, radius);
  weight += opponent_signs * 5;
  
  // БОНУС ЗА ЦЕНТР (+3)
  // Центральные клетки: (9,9), (9,10), (10,9), (10,10)
  const int center_min = 9;
  const int center_max = 10;
  if (cell.x >= center_min && cell.x <= center_max &&
      cell.y >= center_min && cell.y <= center_max) {
    weight += 3;
  }
  
  return weight;
}

// ============================================================
// ГЛАВНЫЙ МЕТОД — ВЫБОР ХОДА
// ============================================================
Point MyPlayer::make_move(const State& state) {
  // ЗАМЕР ВРЕМЕНИ НАЧАЛА (для проверки производительности)
  auto start = std::chrono::high_resolution_clock::now();
  
  const int rows = state.get_opts().rows;    // высота поля (20)
  const int cols = state.get_opts().cols;    // ширина поля (20)
  Sign opponent = (m_sign == Sign::X) ? Sign::O : Sign::X;
  
  // ------------------------------------------------------------
  // ПРИОРИТЕТ 1: АТАКА — ищем выигрышный ход
  // ------------------------------------------------------------
  for (int y = 0; y < rows; ++y) {
    for (int x = 0; x < cols; ++x) {
      Point p{x, y};
      if (state.get_value(x, y) == Sign::NONE) {      // только пустые клетки
        if (is_winning_move(state, p, m_sign)) {      // можем выиграть?
          auto end = std::chrono::high_resolution_clock::now();
          double ms = std::chrono::duration<double, std::milli>(end - start).count();
          std::cout << "[Performance] Move time: " << ms << " ms (winning move)" << std::endl;
          return p;                                    // ходим туда
        }
      }
    }
  }
  
  // ------------------------------------------------------------
  // ПРИОРИТЕТ 2: ЗАЩИТА — ищем блокирующий ход
  // ------------------------------------------------------------
  for (int y = 0; y < rows; ++y) {
    for (int x = 0; x < cols; ++x) {
      Point p{x, y};
      if (state.get_value(x, y) == Sign::NONE) {      // только пустые клетки
        if (is_blocking_move(state, p, opponent)) {   // соперник выиграет?
          auto end = std::chrono::high_resolution_clock::now();
          double ms = std::chrono::duration<double, std::milli>(end - start).count();
          std::cout << "[Performance] Move time: " << ms << " ms (blocking move)" << std::endl;
          return p;                                    // блокируем
        }
      }
    }
  }
  
  // ------------------------------------------------------------
  // ПРИОРИТЕТ 3: ВЫБОР ПО ВЕСУ
  // ------------------------------------------------------------
  int best_weight = std::numeric_limits<int>::min();   // самый маленький возможный вес
  std::vector<Point> best_cells;                       // список клеток с лучшим весом
  
  // Перебираем все клетки поля
  for (int y = 0; y < rows; ++y) {
    for (int x = 0; x < cols; ++x) {
      Point p{x, y};
      if (state.get_value(x, y) != Sign::NONE) continue;  // только пустые
      
      int w = calculate_weight(state, p);                 // вычисляем вес клетки
      
      if (w > best_weight) {                              // нашли клетку с большим весом
        best_weight = w;
        best_cells.clear();                               // старый список больше не нужен
        best_cells.push_back(p);                          // запоминаем новую лучшую клетку
      } else if (w == best_weight) {                      // вес такой же, как лучший
        best_cells.push_back(p);                          // добавляем в список (для случайного выбора)
      }
    }
  }
  
  // Выбираем случайную клетку из лучших (если их несколько)
  Point result;
  if (!best_cells.empty()) {
    int idx = std::rand() % best_cells.size();            // случайный индекс
    result = best_cells[idx];
  } else {
    // Резервный вариант: первая свободная клетка (никогда не должно сработать)
    for (int y = 0; y < rows; ++y) {
      for (int x = 0; x < cols; ++x) {
        if (state.get_value(x, y) == Sign::NONE) {
          result = {x, y};
          break;
        }
      }
    }
  }
  
  // ЗАМЕР ВРЕМЕНИ ОКОНЧАНИЯ
  auto end = std::chrono::high_resolution_clock::now();
  double ms = std::chrono::duration<double, std::milli>(end - start).count();
  std::cout << "[Performance] Move time: " << ms << " ms (weighted choice)" << std::endl;
  
  return result;
}

} // namespace ttt::my_player