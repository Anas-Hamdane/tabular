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

int main() {
  using namespace tabular;

  Table table;
  table.add_row({"Countries Capitals"})
      .add_row({"United States", "Washington"})
      .add_row({"Brazil", "Brasilia"})
      .add_row({"France", "Paris"})
      .add_row({"Japan", "Tokyo"})
      .add_row({"India", "New Delhi"});

  print(table);
  return 0;
}
