#pragma once

#include "core/game.hpp"

namespace ttt::my_player {

using game::IPlayer;
using game::Point;
using game::Sign;
using game::State;
using game::MoveResult;

class MyPlayer : public IPlayer {
private:
  Sign m_sign = Sign::NONE;
  const char* m_name;

  bool is_winning_move(const State& state, Point move, Sign player_sign) const;
  bool is_blocking_move(const State& state, Point move, Sign opponent_sign) const;
  int calculate_weight(const State& state, Point cell) const;
  int count_signs_in_radius(const State& state, Point center, Sign target_sign, int radius) const;

public:
  MyPlayer(const char* name);
  void set_sign(Sign sign) override;
  Point make_move(const State& state) override;
  const char* get_name() const override;
};

} // namespace ttt::my_player
