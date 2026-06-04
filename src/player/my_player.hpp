#pragma once

#include "core/game.hpp"

namespace ttt::my_player {

// Импортируем нужные типы из пространства имён game для удобства
using game::IPlayer;      // интерфейс игрока (обязателен для наследования)
using game::Point;        // структура с координатами (x, y)
using game::Sign;         // перечисление: X, O, NONE, WALL
using game::State;        // состояние игры (поле, чей ход, победитель и т.д.)
using game::MoveResult;   // результат хода (OK, WIN, DQ и т.д.)

// ============================================================
// КЛАСС МОЕГО БОТА
// ============================================================
// Наследуем интерфейс IPlayer, чтобы игра могла вызывать нашего бота
class MyPlayer : public IPlayer {
private:
  // ------------------------------------------------------------
  // ПРИВАТНЫЕ ПОЛЯ (данные, которые хранит бот)
  // ------------------------------------------------------------
  Sign m_sign = Sign::NONE;      // каким знаком играет бот (X или O)
  const char* m_name;            // имя бота (например, "MyBot")

  // ------------------------------------------------------------
  // ПРИВАТНЫЕ МЕТОДЫ (вспомогательная логика)
  // ------------------------------------------------------------
  
  // Проверка: выиграет ли игрок, если поставит свой знак в клетку move
  bool is_winning_move(const State& state, Point move, Sign player_sign) const;
  
  // Проверка: выиграет ли соперник, если поставит свой знак в клетку move
  bool is_blocking_move(const State& state, Point move, Sign opponent_sign) const;
  
  // Вычисление веса клетки (чем больше вес, тем лучше ход)
  int calculate_weight(const State& state, Point cell) const;
  
  // Подсчёт знаков target_sign в квадрате (2*radius+1) x (2*radius+1) вокруг центра
  int count_signs_in_radius(const State& state, Point center, Sign target_sign, int radius) const;

public:
  // ------------------------------------------------------------
  // ПУБЛИЧНЫЕ МЕТОДЫ (интерфейс, который требует IPlayer)
  // ------------------------------------------------------------
  
  // Конструктор: принимает имя бота
  MyPlayer(const char* name);
  
  // Игра сообщает боту, каким знаком он играет (X или O)
  void set_sign(Sign sign) override;
  
  // ГЛАВНЫЙ МЕТОД: бот выбирает, куда походить
  // Принимает текущее состояние игры, возвращает координаты хода
  Point make_move(const State& state) override;
  
  // Возвращает имя бота (для отображения в консоли)
  const char* get_name() const override;
};

} // namespace ttt::my_player