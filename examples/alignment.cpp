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

// #include <tabular/table.hpp>
#include <tabular/tabular.hpp>

int main() {
    using namespace tabular;

    Table table;
    table.addRow({"Countries Capitals"});
    table.addRow({"Canada", "Ottawa"});
    table.addRow({"Peru", "Lima"});
    table.addRow({"United States", "Washington"});
    table.addRow({"Brazil", "Brasilia"});
    
    table.setAllColsAlign(Alignment::left, 0);
    table.setAllColsAlign(Alignment::right, 1);

    table.rows.at(0).setRowAlign(Alignment::center);

    table.printTable();
    return 0;
}