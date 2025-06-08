/*
      __        ___.         .__
    _/  |______ \_ |__  __ __|  | _____ _______
    \   __\__  \ | __ \|  |  \  | \__  \\_  __ \
     |  |  / __ \| \_\ \  |  /  |__/ __ \|  | \/
     |__| (____  /___  /____/|____(____  /__|
               \/    \/                \/

    *  Author: Anas Hamdane
    *  Github: https://github.com/Anas-Hamdane

    * EXAMPLE of using background column coloring in a table
*/

#include <tabular/table.hpp>
// #include <tabular/tabular.hpp>

#include <iostream>

int main() {
    using namespace tabular;

    Table table;

    table.add_row({"BLACK"});
    table.add_row({"MAGENTA"});
    table.add_row({"CYAN"});
    table.add_row({"WHITE"});

    table.add_row({"rgb(0,0,255)"});
    table.add_row({"rgb(255,0,0)"});
    table.add_row({"rgb(0,255,0)"});
    table.add_row({"rgb(255,255,0)"});
    
    table.config().border(BorderStyle::ANSI);

    table[0][0].config().column_background_color(BackgroundColor::black);
    table[1][0].config().column_background_color(BackgroundColor::magenta);
    table[2][0].config().column_background_color(BackgroundColor::cyan);
    table[3][0].config().column_background_color(BackgroundColor::white);

    table[4][0].config().column_background_rgb(RGB(0, 0, 255));
    table[5][0].config().column_background_rgb(RGB(255, 0, 0));
    table[6][0].config().column_background_rgb(RGB(0, 255, 0));
    table[7][0].config().column_background_rgb(RGB(255, 255, 0));

    // for testing because "ninja" change the output stream of the program
    table.set().forced_width(60, true);

    std::cout << table << std::endl;
    return 0;
}
