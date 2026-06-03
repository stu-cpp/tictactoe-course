#include "core/game.hpp"
#include "core/field.hpp"
#include "player/my_player.hpp"
#include <iostream>

using namespace ttt::game;
using namespace ttt::my_player;

int main() {
  State::Opts opts;
  opts.rows = 20;
  opts.cols = 20;
  opts.win_len = 5;
  opts.max_moves = 400;
  
  // RandomObstaclesFI определён в field.hpp
  RandomObstaclesFI initializer(0.75, 1, 50);
  State state(opts, &initializer);
  state.reset();
  
  MyPlayer bot("MyBot");
  bot.set_sign(Sign::X);  // Бот ходит первым (X)
  
  std::cout << "=== Game: MyBot (X) vs Human (O) ===\n";
  std::cout << std::flush;
  std::cout << "Enter coordinates as: x y (0-19)\n\n";
  
  bool bot_turn = true;
  
  while (state.get_status() == Status::ACTIVE) {
    // Отрисовка поля
    std::cout << "  ";
    for (int x = 0; x < opts.cols; ++x) std::cout << (x % 10) << " ";
    std::cout << "\n";
    for (int y = 0; y < opts.rows; ++y) {
      std::cout << (y % 10) << " ";
      for (int x = 0; x < opts.cols; ++x) {
        Sign val = state.get_value(x, y);
        if (val == Sign::X) std::cout << "X ";
        else if (val == Sign::O) std::cout << "O ";
        else if (val == Sign::WALL) std::cout << "# ";
        else std::cout << ". ";
      }
      std::cout << "\n";
    }
    std::cout << "\n";
        std::cout << std::flush;
    if (bot_turn) {
      std::cout << "Bot thinking...\n";
      Point move = bot.make_move(state);
      std::cout << "Bot plays: (" << move.x << ", " << move.y << ")\n";
      state.process_move(Sign::X, move.x, move.y);
      
      if (state.get_winner() == Sign::X) {
        std::cout << "\n*** BOT WON! ***\n";
        break;
      }
    } else {
      int x, y;
      std::cout << "Your move (O): x y (0-19): ";
      std::cin >> x >> y;
      
      if (x < 0 || x >= opts.cols || y < 0 || y >= opts.rows) {
        std::cout << "Invalid coordinates! Try again.\n";
        continue;
      }
      if (state.get_value(x, y) != Sign::NONE) {
        std::cout << "Cell occupied! Try again.\n";
        continue;
      }
      
      state.process_move(Sign::O, x, y);
      
      if (state.get_winner() == Sign::O) {
        std::cout << "\n*** HUMAN WON! ***\n";
        break;
      }
    }
    
    bot_turn = !bot_turn;
    
    if (state.get_move_no() >= opts.max_moves) {
      std::cout << "\n*** DRAW! ***\n";
      break;
    }
  }
  
  return 0;
}
