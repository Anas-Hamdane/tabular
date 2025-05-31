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
    
    table[0][1].config().add_font_style(Font::bold);
    table[0][2].config().add_font_style(Font::dim);
    table[1][0].config().add_font_style(Font::italic);
    table[1][1].config().add_font_style(Font::underline);
    table[1][2].config().add_font_style(Font::blink);
    table[2][0].config().add_font_style(Font::reverse);
    table[2][1].config().add_font_style(Font::concealed);
    table[2][2].config().add_font_style(Font::crossed);

    table[4][0].config().add_font_style({Font::bold, Font::italic});
    table[4][1].config().add_font_style({Font::bold, Font::underline});
    table[5][0].config().add_font_style({Font::dim, Font::italic});
    table[5][1].config().add_font_style({Font::dim, Font::underline});
    table[6][0].config().add_font_style({Font::blink, Font::italic});
    table[6][1].config().add_font_style({Font::blink, Font::underline});
    table[7][0].config().add_font_style({Font::reverse, Font::italic});
    table[7][1].config().add_font_style({Font::reverse, Font::underline});
    
    table[9][0].config().add_font_style({Font::bold, Font::italic, Font::underline});
    table[10][0].config().add_font_style({Font::dim, Font::italic, Font::underline});
    table[11][0].config().add_font_style({Font::blink, Font::italic, Font::underline});
    table[12][0].config().add_font_style({Font::reverse, Font::italic, Font::underline});
    
    table.config().border(BorderStyle::ANSI);

    // for testing because "ninja" change the output stream of the program
    table.set().forced_width(60, true);
    std::cout << table << std::endl;
    return 0;
}
