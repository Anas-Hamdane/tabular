/*
      __        ___.         .__
    _/  |______ \_ |__  __ __|  | _____ _______
    \   __\__  \ | __ \|  |  \  | \__  \\_  __ \
     |  |  / __ \| \_\ \  |  /  |__/ __ \|  | \/
     |__| (____  /___  /____/|____(____  /__|
               \/    \/                \/

    *  Author: Anas Hamdane
    *  Github: https://github.com/Anas-Hamdane

    * EXAMPLE of a customizable border in a table
*/

#include <tabular/table.hpp>
// #include <tabular/tabular.hpp>

#include <iostream>

int main() {
    using namespace tabular;

    Table table;
    table.add_row({"À l’extérieur du café, José rêvait d’une vie plus simple :\ndes journées ensoleillées, des promenades à la campagne, et des soirées tranquilles à écouter de la musique douce\npendant que les étoiles brillaient au-dessus des toits."});

    RGB bg(20, 21, 35);
    RGB fg(255, 255, 255);

    table.border().set().style(BorderStyle::ANSI);
    table.set().multi_byte_characters(true);

    table.border().color().horizontal(RGB(166, 227, 161));
    table.border().color().vertical(RGB(0, 175, 255));
    table.border().color().corners(RGB(215, 0, 0));

    table.border().set().top_left_corner("◜");
    table.border().set().top_right_corner("◝");

    table.border().set().bottom_left_corner("◟");
    table.border().set().bottom_right_corner("◞");

    table.border().background_color().full(bg);
    table[0][0].config().column_background_color(bg);

    table[0][0].config().color(fg);
    table[0][0].config().text_style(Style::bold);

    // for testing because "ninja" change the output stream of the program
    table.set().forced_width(70, true);

    std::cout << table << std::endl;
    return 0;
}
