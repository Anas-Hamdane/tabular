/*
      __        ___.         .__
    _/  |______ \_ |__  __ __|  | _____ _______
    \   __\__  \ | __ \|  |  \  | \__  \\_  __ \
     |  |  / __ \| \_\ \  |  /  |__/ __ \|  | \/
     |__| (____  /___  /____/|____(____  /__|
               \/    \/                \/

    *  Author: Anas Hamdane
    *  Github: https://github.com/Anas-Hamdane

    * EXAMPLE of irregular tables
*/

// #include <tabular/table.hpp>
#include <tabular/tabular.hpp>

int main() {
    using namespace tabular;

    Table table;
    table.addRow({"Countries Capitals"});
    table.addRow({"United States", "Washington"});
    table.addRow({"Brazil", "Brasilia"});
    table.addRow({"Countries Population"});
    table.addRow({"United States", "340.1 million (2024)"});
    table.addRow({"Brazil", "211.1 million (2023)"});

    table.printTable();
    return 0;
}