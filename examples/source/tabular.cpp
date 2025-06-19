/*
      __        ___.         .__
    _/  |______ \_ |__  __ __|  | _____ _______
    \   __\__  \ | __ \|  |  \  | \__  \\_  __ \
     |  |  / __ \| \_\ \  |  /  |__/ __ \|  | \/
     |__| (____  /___  /____/|____(____  /__|
               \/    \/                \/

    *  Author: Anas Hamdane
    *  Github: https://github.com/Anas-Hamdane
*/

#include <tabular/tabular.hpp>
// #include <tabular/tabular.hpp>

int main() {
  using namespace tabular;

  Table table;
  table.add_row({"tabular"});

  table.border().set().style(BorderStyle::ansi);
  table[0][0].config().alignment(Alignment::center);

  table.set().width(11);

  print(table);
  return 0;
}
