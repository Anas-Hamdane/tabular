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

    table.add_row({"Bold", "Italic", "Bold & Italic", "Blinking"});
    table.add_row({"Underline", "Crossed", "Dark", "Bold, Italic & Underlined"});

    table.config().border(BorderStyle::ANSI);

    table[0][0].config().width(12).add_font_styles({FontStyle::bold});
    table[0][1].config().width(9).add_font_styles({FontStyle::italic});
    table[0][2].config().width(16).add_font_styles({FontStyle::italic, FontStyle::bold});
    table[0][3].config().width(28).add_font_styles({FontStyle::blink});
    
    table[1][0].config().width(12).add_font_styles({FontStyle::underline});
    table[1][1].config().width(9).add_font_styles({FontStyle::crossed});
    table[1][2].config().width(16).add_font_styles({FontStyle::dark});
    table[1][3].config().width(28).add_font_styles({FontStyle::bold, FontStyle::italic, FontStyle::underline});

    table.config().border(BorderStyle::ANSI);

    // for testing because "ninja" change the output stream of the program
    table.set().forced_width(60, true);

    std::cout << table << std::endl;
    return 0;
}
