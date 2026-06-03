#include "core/game.hpp"
#include "core/random_obstacles.hpp"
#include "player/my_player.hpp"
#include <chrono>
#include <iostream>
#include <vector>

using namespace ttt::game;
using namespace ttt::my_player;

int main() {
  State::Opts opts;
  opts.rows = 20;
  opts.cols = 20;
  opts.win_len = 5;
  opts.max_moves = 400;
  
  RandomObstaclesFI initializer(0.75, 1, 50);
  
  std::vector<double> move_times;
  
  for (int game_num = 0; game_num < 10; ++game_num) {
    State state(opts, &initializer);
    state.reset();
    
    MyPlayer player("MyBot");
    player.set_sign(Sign::X);
    
    for (int move = 0; move < 50; ++move) {
      auto start = std::chrono::high_resolution_clock::now();
      Point p = player.make_move(state);
      auto end = std::chrono::high_resolution_clock::now();
      
      double ms = std::chrono::duration<double, std::milli>(end - start).count();
      move_times.push_back(ms);
      
      // Делаем случайный ход соперника, чтобы игра продолжалась
      for (int y = 0; y < opts.rows; ++y) {
        for (int x = 0; x < opts.cols; ++x) {
          if (state.get_value(x, y) == Sign::NONE) {
            state.process_move(Sign::O, x, y);
            break;
          }
        }
      }
      if (state.get_status() == Status::ENDED) break;
    }
  }
  
  double total = 0;
  double max_time = 0;
  for (double t : move_times) {
    total += t;
    if (t > max_time) max_time = t;
  }
  double avg = total / move_times.size();
  
  std::cout << "=== Performance Test ===\n";
  std::cout << "Total moves measured: " << move_times.size() << "\n";
  std::cout << "Average move time: " << avg << " ms\n";
  std::cout << "Max move time: " << max_time << " ms\n";
  if (avg < 100) {
    std::cout << "✓ PASSED (avg < 100 ms)\n";
  } else {
    std::cout << "✗ FAILED (avg >= 100 ms)\n";
  }
  
  return 0;
}
