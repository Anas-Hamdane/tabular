/*
      __        ___.         .__
    _/  |______ \_ |__  __ __|  | _____ _______
    \   __\__  \ | __ \|  |  \  | \__  \\_  __ \
     |  |  / __ \| \_\ \  |  /  |__/ __ \|  | \/
     |__| (____  /___  /____/|____(____  /__|
               \/    \/                \/

    *  Author: Anas Hamdane
    *  Github: https://github.com/Anas-Hamdane

    * EXAMPLE of a Simple table
*/

// #include <tabular/table.hpp>
#include <tabular/table.hpp>

int main() {
    using namespace tabular;

    Table table;
    table.add_row({"Country", "Capital"});
    table.add_row({"Canada", "Ottawa"});
    table.add_row({"Peru", "Lima"});
    table.add_row({"United States", "Washington"});
    table.add_row({"Brazil", "Brasilia"});

    std::cout << table << std::endl;
    return 0;
}