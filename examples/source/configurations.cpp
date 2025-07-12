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

int main(int argc, char* argv[]) {
  using namespace tabular;

  Table table;

  // Add Rows
  table.add_row({"Item", "Quantity", "Unit Price"})
      .add_row({"Apple", "4", "$0.50"})
      .add_row({"Banana", "12", "$0.15"})
      .add_row({"Cherry", "100", "$0.02"});

  table[0][0].config().top_padding(1).alignment(Alignment::center);
  table[0][1].config().top_padding(1).alignment(Alignment::center);
  table[0][2].config().top_padding(1).alignment(Alignment::center);

  table[0][2].config().width(12);
  table[1][2].config().width(12);
  table[2][2].config().width(12);
  table[3][2].config().width(12);

  table[0][1].config().width(10);
  table[1][1].config().width(10);
  table[2][1].config().width(10);
  table[3][1].config().width(10);

  table[0][0].config().alignment(Alignment::center).width(15);
  table[1][0].config().alignment(Alignment::center).width(15);
  table[2][0].config().alignment(Alignment::center).width(15);
  table[3][0].config().alignment(Alignment::center).width(15);

  table[3][0].config().bottom_padding(1);
  table[3][1].config().bottom_padding(1);
  table[3][2].config().bottom_padding(1);

  table.border().set().style(BorderStyle::ansi);
  print(table);

  return 0;
}
