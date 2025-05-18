/*
      __        ___.         .__
    _/  |______ \_ |__  __ __|  | _____ _______
    \   __\__  \ | __ \|  |  \  | \__  \\_  __ \
     |  |  / __ \| \_\ \  |  /  |__/ __ \|  | \/
     |__| (____  /___  /____/|____(____  /__|
               \/    \/                \/

    *  Author: Anas Hamdane
    *  Github: https://github.com/Anas-Hamdane

    * EXAMPLE of aligning table content
*/

#include <tabular/table.hpp>
// #include <tabular/tabular.hpp>

#include <iostream>

int main() {
    using namespace tabular;

    Table table;
    table.add_row({"Countries Capitals"});
    table.add_row({"Canada", "Ottawa"});
    table.add_row({"Peru", "Lima"});
    table.add_row({"United States", "Washington"});
    table.add_row({"Brazil", "Brasilia"});

    table.config().alignment(Alignment::left, 0);
    table.config().alignment(Alignment::right, 1);

    table[0].config().alignment(Alignment::center);

    std::cout << table << std::endl;
    return 0;
}