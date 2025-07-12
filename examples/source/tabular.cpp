/*
      __        ___.         .__
    _/  |______ \_ |__  __ __|  | _____ _______
    \   __\__  \ | __ \|  |  \  | \__  \\_  __ \
     |  |  / __ \| \_\ \  |  /  |__/ __ \|  | \/
     |__| (____  /___  /____/|____(____  /__|
               \/    \/                \/

    *  Author: Anas Hamdane
    *  Github: https://github.com/Anas-Hamdane
    *  LICENSE: https://github.com/Anas-Hamdane/tabular/blob/main/LICENSE
*/

#include <tabular/tabular.hpp>
// #include <tabular/tabular.hpp>

int main() {
  using namespace tabular;

  std::string tabular = "tabular";

  Table table;
  table.add_row({tabular});

  table.border().set().style(BorderStyle::ansi);
  table[0][0].config().alignment(Alignment::center);

  // 4 = 2 + 2
  // 2 for default padding (one space each side)
  // 2 for table border sides (left and right vertical border)
  table.set().width(tabular.length() + 4);

  print(table);
  return 0;
}
