/*
      __        ___.         .__
    _/  |______ \_ |__  __ __|  | _____ _______
    \   __\__  \ | __ \|  |  \  | \__  \\_  __ \
     |  |  / __ \| \_\ \  |  /  |__/ __ \|  | \/
     |__| (____  /___  /____/|____(____  /__|
               \/    \/                \/

    *  Author: Anas Hamdane
    *  Github: https://github.com/Anas-Hamdane

    * EXAMPLE of using font in a table
*/

#include <tabular/printer.hpp>
// #include <tabular/tabular.hpp>

#include <iostream>

int main() {
  using namespace tabular;
  Table table;
  table.add_row({"Normal", "Bold", "Dim"});
  table.add_row({"Italic", "Underline", "Blink"});
  table.add_row({"Reverse", "Concealed", "Crossed"});
  table.add_row({" "});
  table.add_row({"Bold & Italic", "Bold & Underline"});
  table.add_row({"Dim & Italic", "Dim & Underline"});
  table.add_row({"Blink & Italic", "Blink & Underline"});
  table.add_row({"Reverse & Italic", "Reverse & Underline"});
  table.add_row({" "});
  table.add_row({"Bold & Italic & Underline"});
  table.add_row({"Dim & Italic & Underline"});
  table.add_row({"Blink & Italic & Underline"});
  table.add_row({"Reverse & Italic & Underline"});

  table[0][1].config().text_attribute(Attribute::bold);
  table[0][2].config().text_attribute(Attribute::dim);
  table[1][0].config().text_attribute(Attribute::italic);
  table[1][1].config().text_attribute(Attribute::underline);
  table[1][2].config().text_attribute(Attribute::blink);
  table[2][0].config().text_attribute(Attribute::reverse);
  table[2][1].config().text_attribute(Attribute::concealed);
  table[2][2].config().text_attribute(Attribute::crossed);

  table[4][0].config().text_attribute({Attribute::bold, Attribute::italic});
  table[4][1].config().text_attribute({Attribute::bold, Attribute::underline});
  table[5][0].config().text_attribute({Attribute::dim, Attribute::italic});
  table[5][1].config().text_attribute({Attribute::dim, Attribute::underline});
  table[6][0].config().text_attribute({Attribute::blink, Attribute::italic});
  table[6][1].config().text_attribute({Attribute::blink, Attribute::underline});
  table[7][0].config().text_attribute({Attribute::reverse, Attribute::italic});
  table[7][1].config().text_attribute({Attribute::reverse, Attribute::underline});

  table[9][0].config().text_attribute({Attribute::bold, Attribute::italic, Attribute::underline});
  table[10][0].config().text_attribute({Attribute::dim, Attribute::italic, Attribute::underline});
  table[11][0].config().text_attribute({Attribute::blink, Attribute::italic, Attribute::underline});
  table[12][0].config().text_attribute({Attribute::reverse, Attribute::italic, Attribute::underline});

  table.border().set().style(BorderStyle::ansi);

  table.set().width(60);
  std::cout << table << std::endl;
  return 0;
}
