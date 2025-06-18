/*
      __        ___.         .__
    _/  |______ \_ |__  __ __|  | _____ _______
    \   __\__  \ | __ \|  |  \  | \__  \\_  __ \
     |  |  / __ \| \_\ \  |  /  |__/ __ \|  | \/
     |__| (____  /___  /____/|____(____  /__|
               \/    \/                \/

    *  Author: Anas Hamdane
    *  Github: https://github.com/Anas-Hamdane

    * EXAMPLE of using background column coloring in a table
*/

#include <tabular/printer.hpp>
// #include <tabular/tabular.hpp>

#include <iostream>

int main() {
  using namespace tabular;

  Table table;

  table.add_row({"BLACK"});
  table.add_row({"MAGENTA"});
  table.add_row({"CYAN"});
  table.add_row({"WHITE"});

  table.add_row({"rgb(0,0,255)"});
  table.add_row({"rgb(255,0,0)"});
  table.add_row({"rgb(0,255,0)"});
  table.add_row({"rgb(255,255,0)"});

  table.border().set().style(BorderStyle::ansi);

  table[0][0].style().column_background_color(Color::black);
  table[1][0].style().column_background_color(Color::magenta);
  table[2][0].style().column_background_color(Color::cyan);
  table[3][0].style().column_background_color(Color::white);

  table[4][0].style().column_background_color(RGB(0, 0, 255));
  table[5][0].style().column_background_color(RGB(255, 0, 0));
  table[6][0].style().column_background_color(RGB(0, 255, 0));
  table[7][0].style().column_background_color(RGB(255, 255, 0));

  table.set().width(60);

  std::cout << table << std::endl;
  return 0;
}
