/*
      __        ___.         .__
    _/  |______ \_ |__  __ __|  | _____ _______
    \   __\__  \ | __ \|  |  \  | \__  \\_  __ \
     |  |  / __ \| \_\ \  |  /  |__/ __ \|  | \/
     |__| (____  /___  /____/|____(____  /__|
               \/    \/                \/

    *  Author: Anas Hamdane
    *  Github: https://github.com/Anas-Hamdane

    * EXAMPLE of using background and foreground rgb colors of the content of the column in a table
*/

#include <tabular/table.hpp>
// #include <tabular/tabular.hpp>

#include <iostream>

int main() {
    using namespace tabular;

    Table table;

    table.add_row({"rgb(0,0,0)", "rgb(0,0,0) background"});
    table.add_row({"rgb(255,0,0)", "rgb(255,0,0) background"});
    table.add_row({"rgb(0,255,0)", "rgb(0,255,0) background"});
    table.add_row({"rgb(255,255,0)", "rgb(255,255,0) background"});
    table.add_row({"rgb(0,0,255)", "rgb(0,0,255) background"});
    table.add_row({"rgb(255,0,255)", "rgb(255,0,255) background"});
    table.add_row({"rgb(0,255,255)", "rgb(0,255,255) background"});
    table.add_row({"rgb(255, 255, 255)", "rgb(255, 255, 255) background"});
    
    table.config().border(BorderStyle::ANSI);

    table[0][0].config().rgb(RGB(0, 0, 0));
    table[1][0].config().rgb(RGB(255, 0, 0));
    table[2][0].config().rgb(RGB(0, 255, 0));
    table[3][0].config().rgb(RGB(255, 255, 0));
    table[4][0].config().rgb(RGB(0, 0, 255));
    table[5][0].config().rgb(RGB(255, 0, 255));
    table[6][0].config().rgb(RGB(0, 255, 255));
    table[7][0].config().rgb(RGB(255, 255, 255));

    table[0][1].config().content_background_rgb(RGB(0, 0, 0));
    table[1][1].config().content_background_rgb(RGB(255, 0, 0));
    table[2][1].config().content_background_rgb(RGB(0, 255, 0));
    table[3][1].config().content_background_rgb(RGB(255, 255, 0));
    table[4][1].config().content_background_rgb(RGB(0, 0, 255));
    table[5][1].config().content_background_rgb(RGB(255, 0, 255));
    table[6][1].config().content_background_rgb(RGB(0, 255, 255));
    table[7][1].config().content_background_rgb(RGB(255, 255, 255));

    // for testing because "ninja" change the output stream of the program
    table.set().forced_width(60, true);

    std::cout << table << std::endl;
    return 0;
}
