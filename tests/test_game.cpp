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
  
  RandomObstaclesFI initializer(1.0, 0, 0);
  State state(opts, &initializer);
  state.reset();
  
  MyPlayer bot("MyBot");
  bot.set_sign(Sign::X);
  
  std::cout << "========================================\n";
  std::cout << "=== GAME: MyBot (X) vs Human (O) ===\n";
  std::cout << "========================================\n";
  std::cout << "Enter x y (0-19), example: 10 10\n\n";
  
  bool bot_turn = true;
  int move_count = 0;
  
  while (state.get_status() == Status::ACTIVE && move_count < opts.max_moves) {
    // Показываем поле
    std::cout << "   ";
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
    
    if (bot_turn) {
      std::cout << "Bot thinking...\n";
      Point move = bot.make_move(state);
      std::cout << "Bot plays: " << move.x << " " << move.y << "\n";
      state.process_move(Sign::X, move.x, move.y);
      
      if (state.get_winner() == Sign::X) {
        std::cout << "\n*** BOT WON! ***\n";
        break;
      }
    } else {
      int x, y;
      std::cout << "Your move (O) - enter x y: ";
      std::cin >> x >> y;
      
      if (x < 0 || x >= opts.cols || y < 0 || y >= opts.rows) {
        std::cout << "Invalid! Use 0-19\n";
        continue;
      }
      if (state.get_value(x, y) != Sign::NONE) {
        std::cout << "Cell occupied!\n";
        continue;
      }
      
      state.process_move(Sign::O, x, y);
      
      if (state.get_winner() == Sign::O) {
        std::cout << "\n*** HUMAN WON! ***\n";
        break;
      }
    }
    
    bot_turn = !bot_turn;
    move_count++;
  }
  
  if (state.get_winner() == Sign::NONE) {
    std::cout << "\n*** DRAW! ***\n";
  }
  
  return 0;
}
