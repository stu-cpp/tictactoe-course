#pragma once

#include "core/game.hpp"
#include <iostream>
#include <fstream>

namespace ttt::my_player {

class OstreamWriter : public game::IObserver {
private:
  std::ostream& out;

public:
  OstreamWriter() : out(std::cout) {}
  OstreamWriter(std::ostream& os) : out(os) {}
  
  void handle_event(const game::State& state, const game::Event& event) override {
    print_game_state(state);
  }
  
  void print_game_state(const game::State& state) {
    const auto& opts = state.get_opts();
    out << "  ";
    for (int x = 0; x < opts.cols; ++x) {
      out << (x % 10) << " ";
    }
    out << "\n";
    
    for (int y = 0; y < opts.rows; ++y) {
      out << (y % 10) << " ";
      for (int x = 0; x < opts.cols; ++x) {
        Sign val = state.get_value(x, y);
        if (val == Sign::X) out << "X ";
        else if (val == Sign::O) out << "O ";
        else if (val == Sign::EMPTY) out << ". ";
        else if (val == Sign::WALL) out << "# ";
        else out << "? ";
      }
      out << "\n";
    }
    out << "\n";
  }
};

} // namespace ttt::my_player
