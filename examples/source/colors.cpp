/*
      __        ___.         .__
    _/  |______ \_ |__  __ __|  | _____ _______
    \   __\__  \ | __ \|  |  \  | \__  \\_  __ \
     |  |  / __ \| \_\ \  |  /  |__/ __ \|  | \/
     |__| (____  /___  /____/|____(____  /__|
               \/    \/                \/

    *  Author: Anas Hamdane
    *  Github: https://github.com/Anas-Hamdane

    * EXAMPLE of using background and foreground colors of the content of a column in a table
*/

#include <tabular/printer.hpp>
// #include <tabular/tabular.hpp>

#include <iostream>

int main() {
  using namespace tabular;

  Table table;

  table.add_row({"BLACK", "BLACK_BACK"});
  table.add_row({"RED", "RED_BACK"});
  table.add_row({"GREEN", "GREEN_BACK"});
  table.add_row({"YELLOW", "YELLOW_BACK"});
  table.add_row({"BLUE", "BLUE_BACK"});
  table.add_row({"MAGENTA", "MAGENTA_BACK"});
  table.add_row({"CYAN", "CYAN_BACK"});
  table.add_row({"WHITE", "WHITE_BACK"});

  table.border().set().style(BorderStyle::ansi);

  table[0][0].style().content_color(Color::black);
  table[1][0].style().content_color(Color::red);
  table[2][0].style().content_color(Color::green);
  table[3][0].style().content_color(Color::yellow);
  table[4][0].style().content_color(Color::blue);
  table[5][0].style().content_color(Color::magenta);
  table[6][0].style().content_color(Color::cyan);
  table[7][0].style().content_color(Color::white);

  table[0][1].style().content_background_color(Color::black);
  table[1][1].style().content_background_color(Color::red);
  table[2][1].style().content_background_color(Color::green);
  table[3][1].style().content_background_color(Color::yellow);
  table[4][1].style().content_background_color(Color::blue);
  table[5][1].style().content_background_color(Color::magenta);
  table[6][1].style().content_background_color(Color::cyan);
  table[7][1].style().content_background_color(Color::white);

  table.set().width(60);

  std::cout << table << std::endl;
  return 0;
}
