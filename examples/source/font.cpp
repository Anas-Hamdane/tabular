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

#include <tabular/table.hpp>
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

  table[0][1].config().text_style(Style::bold);
  table[0][2].config().text_style(Style::dim);
  table[1][0].config().text_style(Style::italic);
  table[1][1].config().text_style(Style::underline);
  table[1][2].config().text_style(Style::blink);
  table[2][0].config().text_style(Style::reverse);
  table[2][1].config().text_style(Style::concealed);
  table[2][2].config().text_style(Style::crossed);

  table[4][0].config().text_style({Style::bold, Style::italic});
  table[4][1].config().text_style({Style::bold, Style::underline});
  table[5][0].config().text_style({Style::dim, Style::italic});
  table[5][1].config().text_style({Style::dim, Style::underline});
  table[6][0].config().text_style({Style::blink, Style::italic});
  table[6][1].config().text_style({Style::blink, Style::underline});
  table[7][0].config().text_style({Style::reverse, Style::italic});
  table[7][1].config().text_style({Style::reverse, Style::underline});

  table[9][0].config().text_style({Style::bold, Style::italic, Style::underline});
  table[10][0].config().text_style({Style::dim, Style::italic, Style::underline});
  table[11][0].config().text_style({Style::blink, Style::italic, Style::underline});
  table[12][0].config().text_style({Style::reverse, Style::italic, Style::underline});

  table.border().set().style(BorderStyle::ANSI);

  // for testing because "ninja" change the output stream of the program
  table.set().forced_width(60, true);
  std::cout << table << std::endl;
  return 0;
}
