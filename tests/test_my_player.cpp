#include "player/my_observer.hpp"
#include "player/my_player.hpp"

#include <cassert>
#include <cstdlib>
#include <iostream>

 
// Тесты check_win
 

static void test_check_win_horizontal() {
  // Поле 6x6, L=4.
  // O стоит в (1,0)(2,0)(3,0), слева в (0,0) стоит X — блокирует.
  // Выигрышный ход O только вправо: (4,0).
  // (0,0) занята X → check_win вернёт false (клетка не свободна).
  ttt::game::State::Opts opts;
  opts.rows = opts.cols = 6;
  opts.win_len = 4;
  opts.max_moves = 0;
  ttt::game::State state(opts);

  // X занимает (0,0) — блокирует левый конец цепочки O
  state.process_move(ttt::game::Sign::X, 0, 0);
  state.process_move(ttt::game::Sign::O, 1, 0);
  state.process_move(ttt::game::Sign::X, 5, 5);
  state.process_move(ttt::game::Sign::O, 2, 0);
  state.process_move(ttt::game::Sign::X, 5, 4);
  state.process_move(ttt::game::Sign::O, 3, 0);

  ttt::my_player::MyPlayer player("test");
  player.set_sign(ttt::game::Sign::O);

  // (4,0) — выигрышный ход (продолжает цепочку вправо до 4)
  assert(player.check_win(state, 4, 0, ttt::game::Sign::O) == true);
  // (0,0) занята X — check_win должен вернуть false
  assert(player.check_win(state, 0, 0, ttt::game::Sign::O) == false);
  // (5,0) — слишком далеко, цепочка не достанет
  assert(player.check_win(state, 5, 0, ttt::game::Sign::O) == false);

  std::cout << "[OK] test_check_win_horizontal\n";
}

static void test_check_win_vertical() {
  // O стоит в (0,0)(0,1)(0,2) — ход в (0,3) даёт вертикаль длиной 4
  ttt::game::State::Opts opts;
  opts.rows = opts.cols = 5;
  opts.win_len = 4;
  opts.max_moves = 0;
  ttt::game::State state(opts);

  state.process_move(ttt::game::Sign::X, 1, 0);
  state.process_move(ttt::game::Sign::O, 0, 0);
  state.process_move(ttt::game::Sign::X, 1, 1);
  state.process_move(ttt::game::Sign::O, 0, 1);
  state.process_move(ttt::game::Sign::X, 1, 2);
  state.process_move(ttt::game::Sign::O, 0, 2);

  ttt::my_player::MyPlayer player("test");
  player.set_sign(ttt::game::Sign::O);

  assert(player.check_win(state, 0, 3, ttt::game::Sign::O) == true);
  // (1,3) — там нет цепочки O
  assert(player.check_win(state, 1, 3, ttt::game::Sign::O) == false);

  std::cout << "[OK] test_check_win_vertical\n";
}

static void test_check_win_diagonal() {
  // O стоит в (0,0)(1,1)(2,2) — ход в (3,3) даёт диагональ длиной 4
  ttt::game::State::Opts opts;
  opts.rows = opts.cols = 5;
  opts.win_len = 4;
  opts.max_moves = 0;
  ttt::game::State state(opts);

  state.process_move(ttt::game::Sign::X, 4, 0);
  state.process_move(ttt::game::Sign::O, 0, 0);
  state.process_move(ttt::game::Sign::X, 4, 1);
  state.process_move(ttt::game::Sign::O, 1, 1);
  state.process_move(ttt::game::Sign::X, 4, 2);
  state.process_move(ttt::game::Sign::O, 2, 2);

  ttt::my_player::MyPlayer player("test");
  player.set_sign(ttt::game::Sign::O);

  assert(player.check_win(state, 3, 3, ttt::game::Sign::O) == true);
  // (0,1) — не продолжает диагональ до 4
  assert(player.check_win(state, 0, 1, ttt::game::Sign::O) == false);

  std::cout << "[OK] test_check_win_diagonal\n";
}

static void test_check_win_occupied_cell() {
  // check_win должен вернуть false если клетка уже занята
  ttt::game::State::Opts opts;
  opts.rows = opts.cols = 5;
  opts.win_len = 4;
  opts.max_moves = 0;
  ttt::game::State state(opts);

  state.process_move(ttt::game::Sign::X, 0, 0);

  ttt::my_player::MyPlayer player("test");
  player.set_sign(ttt::game::Sign::O);

  // (0,0) занята X — нельзя ставить туда O
  assert(player.check_win(state, 0, 0, ttt::game::Sign::O) == false);
  // (0,0) занята X — и для X тоже false (клетка уже занята)
  assert(player.check_win(state, 0, 0, ttt::game::Sign::X) == false);

  std::cout << "[OK] test_check_win_occupied_cell\n";
}

 
// Тесты evaluate
 

static void test_evaluate_empty_board() {
  // На пустом поле Score = 0
  ttt::game::State::Opts opts;
  opts.rows = opts.cols = 5;
  opts.win_len = 4;
  opts.max_moves = 0;
  ttt::game::State state(opts);

  ttt::my_player::MyPlayer player("test");
  player.set_sign(ttt::game::Sign::O);

  assert(player.evaluate(state) == 0);

  std::cout << "[OK] test_evaluate_empty_board\n";
}

static void test_evaluate_o_better_than_x() {
  // O имеет цепочку 2 с двумя открытыми концами
  // X стоит одиночно — его вес меньше
  // Score для O должен быть положительным
  ttt::game::State::Opts opts;
  opts.rows = opts.cols = 7;
  opts.win_len = 4;
  opts.max_moves = 0;
  ttt::game::State state(opts);

  // X одиноко в углу
  state.process_move(ttt::game::Sign::X, 6, 6);
  // O цепочка из двух с открытыми концами в середине
  state.process_move(ttt::game::Sign::O, 2, 3);
  state.process_move(ttt::game::Sign::X, 6, 5);
  state.process_move(ttt::game::Sign::O, 3, 3);

  ttt::my_player::MyPlayer player("test");
  player.set_sign(ttt::game::Sign::O);

  assert(player.evaluate(state) > 0);

  std::cout << "[OK] test_evaluate_o_better_than_x\n";
}

 
// Тест make_move: шаг 2 — немедленная победа
 

static void test_make_move_wins_immediately() {
  // O стоит в (1,0)(2,0)(3,0), X не угрожает.
  // Единственный выигрышный ход — (4,0) (слева заблокировано X).
  ttt::game::State::Opts opts;
  opts.rows = opts.cols = 6;
  opts.win_len = 4;
  opts.max_moves = 0;
  ttt::game::State state(opts);

  state.process_move(ttt::game::Sign::X, 0, 0); // блокирует левый конец
  state.process_move(ttt::game::Sign::O, 1, 0);
  state.process_move(ttt::game::Sign::X, 5, 5);
  state.process_move(ttt::game::Sign::O, 2, 0);
  state.process_move(ttt::game::Sign::X, 5, 4);
  state.process_move(ttt::game::Sign::O, 3, 0);

  ttt::my_player::MyPlayer player("test", 2, 3);
  player.set_sign(ttt::game::Sign::O);

  ttt::game::Point move = player.make_move(state);

  // Ход должен быть выигрышным
  assert(player.check_win(state, move.x, move.y, ttt::game::Sign::O) == true);

  std::cout << "[OK] test_make_move_wins_immediately\n";
}

 
// Тест make_move: шаг 3 — блокировка противника
 

static void test_make_move_blocks_opponent() {
  // X стоит в (1,0)(2,0)(3,0), слева (0,0) занята O — блокирована.
  // X угрожает победить в (4,0). O должен туда пойти.
  ttt::game::State::Opts opts;
  opts.rows = opts.cols = 6;
  opts.win_len = 4;
  opts.max_moves = 0;
  ttt::game::State state(opts);

  state.process_move(ttt::game::Sign::X, 1, 0);
  state.process_move(ttt::game::Sign::O, 0, 0); // O блокирует левый конец
  state.process_move(ttt::game::Sign::X, 2, 0);
  state.process_move(ttt::game::Sign::O, 5, 5);
  state.process_move(ttt::game::Sign::X, 3, 0);
  state.process_move(ttt::game::Sign::O, 5, 4);

  ttt::my_player::MyPlayer player("test", 2, 3);
  player.set_sign(ttt::game::Sign::O);

  ttt::game::Point move = player.make_move(state);

  // Ход O должен блокировать X (то есть это выигрышная клетка для X)
  assert(player.check_win(state, move.x, move.y, ttt::game::Sign::X) == true);

  std::cout << "[OK] test_make_move_blocks_opponent\n";
}

 
// main
 

int main(int argc, char *argv[]) {
  std::cout << "--- Unit-tests MyPlayer ---\n\n";

  if (argc >= 2)
    std::srand(std::atoi(argv[1]));

  test_check_win_horizontal();
  test_check_win_vertical();
  test_check_win_diagonal();
  test_check_win_occupied_cell();
  test_evaluate_empty_board();
  test_evaluate_o_better_than_x();
  test_make_move_wins_immediately();
  test_make_move_blocks_opponent();

  std::cout << "\n--- All tests are passed! --- \n\n";

  // Демо-игра
  std::cout << "--- Demo-game P1 vs P2 ---\n\n";

  ttt::game::State::Opts opts;
  opts.rows = opts.cols = 20;
  opts.win_len = 5;
  opts.max_moves = 0;

  auto field_initializer = ttt::game::RandomObstaclesFI(0.75, 50, 1);

  ttt::my_player::MyPlayer p1("p1", 3, 3);
  ttt::my_player::MyPlayer p2("p2", 3, 3);
  ttt::my_player::ConsoleWriter obs;

  ttt::game::Game game(opts, &field_initializer);
  game.add_player(ttt::game::Sign::X, &p1);
  game.add_player(ttt::game::Sign::O, &p2);
  game.add_observer(&obs);

  obs.print_game_state(game.get_state());
  while (game.process() == ttt::game::MoveResult::OK) {
    obs.print_game_state(game.get_state());
  }
  obs.print_game_state(game.get_state());

  return 0;
}
