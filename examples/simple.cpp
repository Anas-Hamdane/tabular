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
#include <tabular/tabular.hpp>

int main() {
    using namespace tabular;

    Table table;
    table.addRow({"Country", "Capital"});
    table.addRow({"Canada", "Ottawa"});
    table.addRow({"Peru", "Lima"});
    table.addRow({"United States", "Washington"});
    table.addRow({"Brazil", "Brasilia"});

    table.printTable();
    return 0;
}