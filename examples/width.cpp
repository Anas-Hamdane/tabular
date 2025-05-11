/*
      __        ___.         .__
    _/  |______ \_ |__  __ __|  | _____ _______
    \   __\__  \ | __ \|  |  \  | \__  \\_  __ \
     |  |  / __ \| \_\ \  |  /  |__/ __ \|  | \/
     |__| (____  /___  /____/|____(____  /__|
               \/    \/                \/

    *  Author: Anas Hamdane
    *  Github: https://github.com/Anas-Hamdane

    * EXAMPLE of controling table width
*/

// #include <tabular/table.hpp>
#include <tabular/table.hpp>

int main() {
    using namespace tabular;

    Table table;
    table.add_row({"5-Top Largest Countries"});
    table.add_row({"#1", "Russia"});
    table.add_row({"#2", "Canada"});
    table.add_row({"#3", "USA"});
    table.add_row({"#4", "China"});
    table.add_row({"#5", "Brazil"});

    table.rows.at(0).set_row_align(Alignment::center);

    table.set_width(50);
 
    table.configure().set_cols_width(4, 0, Range(1, 5));
 
    std::cout << table << std::endl;
    return 0;
}