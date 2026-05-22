#include "player/my_observer.hpp"
#include "player/my_player.hpp"

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <iomanip>

 
// Вспомогательная функция: создаёт состояние поля и заполняет его по строке.
// Пример layout: { ".....", ".OO..", ".XX..", ".....", "....." }
 
static ttt::game::State make_state(int size, int win_len,
                                   const std::vector<std::string> &layout) {
  ttt::game::State::Opts opts;
  opts.rows = opts.cols = size;
  opts.win_len = win_len;
  opts.max_moves = 0;

  ttt::game::State state(opts);

  // Определяем, кто ходит первым: X
  // Сначала расставляем знаки через process_move, чередуя X и O.
  // Но для юнит-тестов удобнее задать поле напрямую.
  // State::set_value доступен не во всех реализациях, поэтому
  // используем только те методы, которые есть в API: просто
  // проверяем get_value и передаём позицию в check_win.
  // Для теста создаём реальное состояние через ходы.
  (void)layout; // layout используется ниже через process_move

  // Воспроизводим позицию: X ходит первым, чередуем X-O-X-O...
  // layout[y][x]: '.' = пусто, 'X' = X, 'O' = O, '#' = стена (пропускаем)
  std::vector<ttt::game::Point> x_moves, o_moves;
  for (int y = 0; y < size && y < (int)layout.size(); ++y) {
    for (int x = 0; x < size && x < (int)layout[y].size(); ++x) {
      char c = layout[y][x];
      if (c == 'X') x_moves.push_back({x, y});
      if (c == 'O') o_moves.push_back({x, y});
    }
  }

  // Чередуем ходы: X, O, X, O, ...
  ttt::game::State result(opts);
  size_t xi = 0, oi = 0;
  while (xi < x_moves.size() || oi < o_moves.size()) {
    if (xi < x_moves.size()) result.process_move(x_moves[xi++]);
    if (oi < o_moves.size()) result.process_move(o_moves[oi++]);
  }
  return result;
}

 
// Тесты check_win
 

static void test_check_win_horizontal() {
  // Поле 5x5, L=4. O стоит в (1,0)(2,0)(3,0) — ход в (4,0) даёт линию 4.
  //  . O O O .
  //  . . . . .
  //  . . . . .
  //  . . . . .
  //  . . . . .
  ttt::game::State::Opts opts;
  opts.rows = opts.cols = 5;
  opts.win_len = 4;
  opts.max_moves = 0;
  ttt::game::State state(opts);

  // X(0,1), O(1,0), X(0,2), O(2,0), X(0,3), O(3,0)
  state.process_move({0, 1}); // X
  state.process_move({1, 0}); // O
  state.process_move({0, 2}); // X
  state.process_move({2, 0}); // O
  state.process_move({0, 3}); // X
  state.process_move({3, 0}); // O

  ttt::my_player::MyPlayer player("test");
  player.set_sign(ttt::game::Sign::O);

  // (4,0) должна давать победу O
  assert(player.check_win(state, 4, 0, ttt::game::Sign::O) == true);
  // (0,0) не даёт победу (там только 1 O рядом)
  assert(player.check_win(state, 0, 0, ttt::game::Sign::O) == false);

  std::cout << "[OK] test_check_win_horizontal\n";
}

static void test_check_win_vertical() {
  // O стоит в (0,0)(0,1)(0,2) — ход в (0,3) даёт линию 4.
  ttt::game::State::Opts opts;
  opts.rows = opts.cols = 5;
  opts.win_len = 4;
  opts.max_moves = 0;
  ttt::game::State state(opts);

  state.process_move({1, 0}); // X
  state.process_move({0, 0}); // O
  state.process_move({1, 1}); // X
  state.process_move({0, 1}); // O
  state.process_move({1, 2}); // X
  state.process_move({0, 2}); // O

  ttt::my_player::MyPlayer player("test");
  player.set_sign(ttt::game::Sign::O);

  assert(player.check_win(state, 0, 3, ttt::game::Sign::O) == true);
  assert(player.check_win(state, 0, 4, ttt::game::Sign::O) == false);

  std::cout << "[OK] test_check_win_vertical\n";
}

static void test_check_win_diagonal() {
  // O стоит в (0,0)(1,1)(2,2) — ход в (3,3) даёт диагональ 4.
  ttt::game::State::Opts opts;
  opts.rows = opts.cols = 5;
  opts.win_len = 4;
  opts.max_moves = 0;
  ttt::game::State state(opts);

  state.process_move({4, 0}); // X
  state.process_move({0, 0}); // O
  state.process_move({4, 1}); // X
  state.process_move({1, 1}); // O
  state.process_move({4, 2}); // X
  state.process_move({2, 2}); // O

  ttt::my_player::MyPlayer player("test");
  player.set_sign(ttt::game::Sign::O);

  assert(player.check_win(state, 3, 3, ttt::game::Sign::O) == true);
  assert(player.check_win(state, 3, 0, ttt::game::Sign::O) == false);

  std::cout << "[OK] test_check_win_diagonal\n";
}

static void test_check_win_occupied_cell() {
  // check_win должен возвращать false если клетка уже занята
  ttt::game::State::Opts opts;
  opts.rows = opts.cols = 5;
  opts.win_len = 4;
  opts.max_moves = 0;
  ttt::game::State state(opts);

  state.process_move({0, 0}); // X на (0,0)

  ttt::my_player::MyPlayer player("test");
  player.set_sign(ttt::game::Sign::O);

  assert(player.check_win(state, 0, 0, ttt::game::Sign::O) == false);

  std::cout << "[OK] test_check_win_occupied_cell\n";
}

 
// Тесты evaluate
 

static void test_evaluate_empty_board() {
  // На пустом поле оценка должна быть 0 (нет цепочек ни у кого)
  ttt::game::State::Opts opts;
  opts.rows = opts.cols = 5;
  opts.win_len = 4;
  opts.max_moves = 0;
  ttt::game::State state(opts);

  ttt::my_player::MyPlayer player("test");
  player.set_sign(ttt::game::Sign::O);

  int score = player.evaluate(state);
  assert(score == 0);

  std::cout << "[OK] test_evaluate_empty_board\n";
}

static void test_evaluate_o_better_than_x() {
  // O имеет цепочку длиной 2 с 2 открытыми концами,
  // X — одиночную фишку. Score должен быть положительным.
  //  . . . . .
  //  . O O . .
  //  . . X . .
  //  . . . . .
  //  . . . . .
  ttt::game::State::Opts opts;
  opts.rows = opts.cols = 5;
  opts.win_len = 4;
  opts.max_moves = 0;
  ttt::game::State state(opts);

  state.process_move({2, 2}); // X
  state.process_move({1, 1}); // O
  state.process_move({4, 4}); // X (в стороне)
  state.process_move({2, 1}); // O

  ttt::my_player::MyPlayer player("test");
  player.set_sign(ttt::game::Sign::O);

  int score = player.evaluate(state);
  assert(score > 0); // O в лучшем положении

  std::cout << "[OK] test_evaluate_o_better_than_x\n";
}

 
// Тест make_move: немедленная победа (шаг 2)
 

static void test_make_move_wins_immediately() {
  // O стоит в (0,0)(1,0)(2,0), X не угрожает.
  // Единственный выигрышный ход — (3,0).
  ttt::game::State::Opts opts;
  opts.rows = opts.cols = 5;
  opts.win_len = 4;
  opts.max_moves = 0;
  ttt::game::State state(opts);

  state.process_move({0, 4}); // X (в стороне)
  state.process_move({0, 0}); // O
  state.process_move({1, 4}); // X
  state.process_move({1, 0}); // O
  state.process_move({2, 4}); // X
  state.process_move({2, 0}); // O

  ttt::my_player::MyPlayer player("test", 2, 3);
  player.set_sign(ttt::game::Sign::O);

  ttt::game::Point move = player.make_move(state);

  // Ход должен быть выигрышным
  assert(player.check_win(state, move.x, move.y, ttt::game::Sign::O) == true);

  std::cout << "[OK] test_make_move_wins_immediately\n";
}


// Тест make_move: блокировка победы X (шаг 3) 

static void test_make_move_blocks_opponent() {
  // X стоит в (0,0)(1,0)(2,0) — угрожает победить в (3,0).
  // O не угрожает. Алгоритм должен заблокировать (3,0).
  ttt::game::State::Opts opts;
  opts.rows = opts.cols = 5;
  opts.win_len = 4;
  opts.max_moves = 0;
  ttt::game::State state(opts);

  state.process_move({0, 0}); // X
  state.process_move({0, 4}); // O (в стороне)
  state.process_move({1, 0}); // X
  state.process_move({1, 4}); // O
  state.process_move({2, 0}); // X
  state.process_move({2, 4}); // O

  ttt::my_player::MyPlayer player("test", 2, 3);
  player.set_sign(ttt::game::Sign::O);

  ttt::game::Point move = player.make_move(state);

  // Ход должен блокировать X
  assert(player.check_win(state, move.x, move.y, ttt::game::Sign::X) == true);

  std::cout << "[OK] test_make_move_blocks_opponent\n";
}


// main

int main(int argc, char *argv[]) {
  std::cout << "=== Юнит-тесты MyPlayer ===\n\n";

  if (argc >= 2)
    std::srand(std::atoi(argv[1]));

  // Тесты check_win
  test_check_win_horizontal();
  test_check_win_vertical();
  test_check_win_diagonal();
  test_check_win_occupied_cell();

  // Тесты evaluate
  test_evaluate_empty_board();
  test_evaluate_o_better_than_x();

  // Тесты make_move
  test_make_move_wins_immediately();
  test_make_move_blocks_opponent();

  // Финальная демонстрация игры
  std::cout << "\n=== Демо-игра P1 vs P2 ===\n\n";

  ttt::game::State::Opts opts;
  opts.rows = opts.cols = 20;
  opts.win_len = 5;
  opts.max_moves = 0;

  auto field_initializer = ttt::game::RandomObstaclesFI(0.75, 50, 1);

  ttt::my_player::MyPlayer p1("p1", 4, 3);
  ttt::my_player::MyPlayer p2("p2", 4, 3);
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

  std::cout << "\nВсе тесты пройдены!\n";
  return 0;
}
