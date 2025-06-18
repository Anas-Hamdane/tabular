/*
      __        ___.         .__
    _/  |______ \_ |__  __ __|  | _____ _______
    \   __\__  \ | __ \|  |  \  | \__  \\_  __ \
     |  |  / __ \| \_\ \  |  /  |__/ __ \|  | \/
     |__| (____  /___  /____/|____(____  /__|
               \/    \/                \/

    *  Author: Anas Hamdane
    *  Github: https://github.com/Anas-Hamdane

    * EXAMPLE of an ANSI table
*/

#include <tabular/printer.hpp>
// #include <tabular/tabular.hpp>

#include <iostream>

int main() {
  using namespace tabular;

  Table table;
  table.add_row({"Top 5 IT universities in the world"});
  table.add_row({"Massachusetts Institute of Technology (MIT)", "94.2"});
  table.add_row({"Stanford University", "92.1"});
  table.add_row({"Carnegie Mellon University", "91.2"});
  table.add_row({"National University of Singapore (NUS)", "90.6"});
  table.add_row({"University of Oxford", "89.8"});

  table[0][0].config().padding(1);

  for (Row& row : table.rows)
    for (Column& column : row.columns)
      column.config().alignment(Alignment::center);

  table.border().set().style(BorderStyle::ansi);

  table.set().width(60);

  std::cout << table << std::endl;
  return 0;
}
