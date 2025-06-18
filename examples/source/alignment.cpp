/*
      __        ___.         .__
    _/  |______ \_ |__  __ __|  | _____ _______
    \   __\__  \ | __ \|  |  \  | \__  \\_  __ \
     |  |  / __ \| \_\ \  |  /  |__/ __ \|  | \/
     |__| (____  /___  /____/|____(____  /__|
               \/    \/                \/

    *  Author: Anas Hamdane
    *  Github: https://github.com/Anas-Hamdane

    * EXAMPLE of aligning table content
*/

#include <tabular/printer.hpp>
// #include <tabular/tabular.hpp>

#include <iostream>

int main() {
  using namespace tabular;

  Table table;
  table.add_row({"Countries Capitals"});
  table.add_row({"Canada", "Ottawa"});
  table.add_row({"Peru", "Lima"});
  table.add_row({"United States", "Washington"});
  table.add_row({"Brazil", "Brasilia"});

  table[1][0].config().alignment(Alignment::left);
  table[2][0].config().alignment(Alignment::left);
  table[3][0].config().alignment(Alignment::left);
  table[4][0].config().alignment(Alignment::left);

  table[1][1].config().alignment(Alignment::right);
  table[2][1].config().alignment(Alignment::right);
  table[3][1].config().alignment(Alignment::right);
  table[4][1].config().alignment(Alignment::right);

  table[0][0].config().alignment(Alignment::center);

  table.set().width(60);

  std::cout << table << std::endl;
  return 0;
}
