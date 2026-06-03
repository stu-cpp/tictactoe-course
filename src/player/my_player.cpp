#include "my_player.hpp"
#include <cstdlib>
#include <ctime>
#include <vector>
#include <limits>
#include <chrono>
#include <iostream>

namespace ttt::my_player {

MyPlayer::MyPlayer(const char* name) : m_name(name) {
  std::srand(std::time(nullptr));
}

void MyPlayer::set_sign(Sign sign) {
  m_sign = sign;
}

const char* MyPlayer::get_name() const {
  return m_name;
}

// Проверка: выиграет ли player_sign, если поставить в (x,y)
bool MyPlayer::is_winning_move(const State& state, Point move, Sign player_sign) const {
  State copy = state;
  if (copy.process_move(player_sign, move.x, move.y) != MoveResult::OK) {
    return false;
  }
  return copy.get_winner() == player_sign;
}

// Проверка: выиграет ли opponent_sign, если поставить в (x,y)
bool MyPlayer::is_blocking_move(const State& state, Point move, Sign opponent_sign) const {
  State copy = state;
  if (copy.process_move(opponent_sign, move.x, move.y) != MoveResult::OK) {
    return false;
  }
  return copy.get_winner() == opponent_sign;
}

// Подсчёт знаков target_sign в квадрате (2*radius+1) x (2*radius+1)
int MyPlayer::count_signs_in_radius(const State& state, Point center, Sign target_sign, int radius) const {
  int count = 0;
  const int rows = state.get_opts().rows;
  const int cols = state.get_opts().cols;
  
  for (int dy = -radius; dy <= radius; ++dy) {
    for (int dx = -radius; dx <= radius; ++dx) {
      int x = center.x + dx;
      int y = center.y + dy;
      if (x < 0 || x >= cols || y < 0 || y >= rows) continue;
      if (state.get_value(x, y) == target_sign) {
        ++count;
      }
    }
  }
  return count;
}

// Вес клетки
int MyPlayer::calculate_weight(const State& state, Point cell) const {
  // Пустая ли клетка?
  if (state.get_value(cell.x, cell.y) != Sign::NONE) {
    return -1000000;
  }
  
  int weight = 0;
  const int radius = 2;
  
  // Свои знаки в радиусе +10 каждый
  int my_signs = count_signs_in_radius(state, cell, m_sign, radius);
  weight += my_signs * 10;
  
  // Знаки противника в радиусе +5 каждый
  Sign opponent = (m_sign == Sign::X) ? Sign::O : Sign::X;
  int opponent_signs = count_signs_in_radius(state, cell, opponent, radius);
  weight += opponent_signs * 5;
  
  // Центр (клетки 9,9 9,10 10,9 10,10 при размере 20)
  const int center_min = 9;
  const int center_max = 10;
  if (cell.x >= center_min && cell.x <= center_max &&
      cell.y >= center_min && cell.y <= center_max) {
    weight += 3;
  }
  
  return weight;
}

Point MyPlayer::make_move(const State& state) {
  // Замер времени начала хода
  auto start = std::chrono::high_resolution_clock::now();
  
  const int rows = state.get_opts().rows;
  const int cols = state.get_opts().cols;
  Sign opponent = (m_sign == Sign::X) ? Sign::O : Sign::X;
  
  // Приоритет 1: атака (свой выигрыш)
  for (int y = 0; y < rows; ++y) {
    for (int x = 0; x < cols; ++x) {
      Point p{x, y};
      if (state.get_value(x, y) == Sign::NONE) {
        if (is_winning_move(state, p, m_sign)) {
          auto end = std::chrono::high_resolution_clock::now();
          double ms = std::chrono::duration<double, std::milli>(end - start).count();
          std::cout << "[Performance] Move time: " << ms << " ms (winning move)" << std::endl;
          return p;
        }
      }
    }
  }
  
  // Приоритет 2: защита (блокировка выигрыша соперника)
  for (int y = 0; y < rows; ++y) {
    for (int x = 0; x < cols; ++x) {
      Point p{x, y};
      if (state.get_value(x, y) == Sign::NONE) {
        if (is_blocking_move(state, p, opponent)) {
          auto end = std::chrono::high_resolution_clock::now();
          double ms = std::chrono::duration<double, std::milli>(end - start).count();
          std::cout << "[Performance] Move time: " << ms << " ms (blocking move)" << std::endl;
          return p;
        }
      }
    }
  }
  
  // Приоритет 3: максимальный вес
  int best_weight = std::numeric_limits<int>::min();
  std::vector<Point> best_cells;
  
  for (int y = 0; y < rows; ++y) {
    for (int x = 0; x < cols; ++x) {
      Point p{x, y};
      if (state.get_value(x, y) != Sign::NONE) continue;
      
      int w = calculate_weight(state, p);
      if (w > best_weight) {
        best_weight = w;
        best_cells.clear();
        best_cells.push_back(p);
      } else if (w == best_weight) {
        best_cells.push_back(p);
      }
    }
  }
  
  Point result;
  if (!best_cells.empty()) {
    int idx = std::rand() % best_cells.size();
    result = best_cells[idx];
  } else {
    // Fallback: первая свободная клетка
    for (int y = 0; y < rows; ++y) {
      for (int x = 0; x < cols; ++x) {
        if (state.get_value(x, y) == Sign::NONE) {
          result = {x, y};
          break;
        }
      }
    }
  }
  
  // Замер времени окончания хода
  auto end = std::chrono::high_resolution_clock::now();
  double ms = std::chrono::duration<double, std::milli>(end - start).count();
  std::cout << "[Performance] Move time: " << ms << " ms (weighted choice)" << std::endl;
  
  return result;
}

} // namespace ttt::my_player
