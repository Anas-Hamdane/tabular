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
    
    table.add_row({"Name", "Age", "Occupation"});
    table.add_row({"Alice", "30", "Engineer"});
    table.add_row({"Bob", "25", "Designer"});
    table.add_row({"Charlie", "35", "Teacher"});
    table.add_row({"Harley", "32", "Business Man"});

    table.format().border(BorderStyle::ANSI);

    table.rows[0].font_styles({FontStyle::bold, FontStyle::underline, FontStyle::italic});
    table.rows[1].font_styles({FontStyle::blink, FontStyle::dark});
    table.rows[2].font_styles({FontStyle::bold, FontStyle::crossed});
    table.rows[3].font_styles({FontStyle::italic, FontStyle::concealed});
    table.rows[4].font_styles({FontStyle::reverse});

    std::cout << table << std::endl;
    return 0;
}