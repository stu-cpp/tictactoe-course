#pragma once

#include "core/game.hpp"
#include <iostream>
#include <fstream>

namespace ttt::my_player {

// ============================================================
// КЛАСС ДЛЯ ВЫВОДА ИГРОВОГО ПОЛЯ
// Штрафное задание 1: вывод в любой поток (консоль или файл)
// ============================================================
class OstreamWriter : public game::IObserver {
private:
  std::ostream& out;   // ссылка на поток вывода (cout или файл)

public:
  // Конструктор по умолчанию — вывод в консоль
  OstreamWriter() : out(std::cout) {}
  
  // Конструктор с произвольным потоком (например, для записи в файл)
  OstreamWriter(std::ostream& os) : out(os) {}
  
  // Обработчик событий игры (требуется интерфейсом IObserver)
  void handle_event(const game::State& state, const game::Event& event) override {
    print_game_state(state);   // при любом событии печатаем поле
  }
  
  // ОТРИСОВКА ИГРОВОГО ПОЛЯ
  void print_game_state(const game::State& state) {
    const auto& opts = state.get_opts();   // получаем настройки поля (20x20)
    
    // Печатаем номера столбцов (0-19, но только последняя цифра)
    out << "  ";
    for (int x = 0; x < opts.cols; ++x) {
      out << (x % 10) << " ";
    }
    out << "\n";
    
    // Печатаем каждую строку поля
    for (int y = 0; y < opts.rows; ++y) {
      out << (y % 10) << " ";          // номер строки (последняя цифра)
      for (int x = 0; x < opts.cols; ++x) {
        game::Sign val = state.get_value(x, y);   // получаем значение в клетке
        
        // Рисуем символ в зависимости от содержимого
        if (val == game::Sign::X) {
          out << "X ";
        } else if (val == game::Sign::O) {
          out << "O ";
        } else if (val == game::Sign::WALL) {
          out << "# ";
        } else {
          out << ". ";   // пустая клетка
        }
      }
      out << "\n";
    }
    out << "\n";
  }
};

} // namespace ttt::my_player