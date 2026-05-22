#pragma once

#include "core/game.hpp"
#include <vector>

namespace ttt::my_player {

using game::Event;
using game::IPlayer;
using game::Point;
using game::Sign;
using game::State;

class MyPlayer : public IPlayer {
  Sign m_sign = Sign::NONE;
  const char *m_name;

  // Параметры алгоритма 
  int m_depth;       // D — глубина поиска в полуходах
  int m_base;        // base — коэффициент роста эвристики
  bool m_last_move_for_o; // флаг «последний ход для O»

public:

  MyPlayer(const char *name, int depth = 4, int base = 3)
      : m_sign(Sign::NONE), m_name(name),
        m_depth(depth), m_base(base),
        m_last_move_for_o(false) {}

  void set_sign(Sign sign) override;
  Point make_move(const State &state) override;
  const char *get_name() const override;

  // Публичные методы для юнит-тестов

  // Шаг 2: проверяет, даёт ли постановка sign в (x,y) линию >= L
  bool check_win(const State &state, int x, int y, Sign sign) const;

  // Шаг 5: эвристическая оценка позиции (Score = сумма_O - сумма_X)
  // Сканирует все линии, считает base^k * M для цепочек длины k < L
  int evaluate(const State &state) const;

private:
  // Minimax с альфа-бета отсечением (шаг 5)
  // maximizing=true — ход O (компьютер), false — ход X (противник)
  int minimax(State state, int depth, int alpha, int beta, bool maximizing) const;

  // Вспомогательные функции

  // Целочисленное возведение в степень (base^exp)
  static int ipow(int base, int exp);

  // Подсчёт веса одной линии (горизонталь/вертикаль/диагональ)
  // Суммирует base^k * M для всех цепочек O и X в линии
  void scan_line(const State &state,
                 int x0, int y0,    // начальная клетка
                 int dx, int dy,    // направление
                 int len,           // длина линии
                 int &score_o, int &score_x) const;
};

}; // namespace ttt::my_player
