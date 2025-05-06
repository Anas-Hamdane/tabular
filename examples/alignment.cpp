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

#include "../include/tabular/tabular.hpp"

int main() {
    using namespace tabular;

    Table table;
    table.addRow({"Countries Capitals"});
    table.addRow({"Canada", "Ottawa"});
    table.addRow({"Peru", "Lima"});
    table.addRow({"United States", "Washington"});
    table.addRow({"Brazil", "Brasilia"});

    table.rows.at(0).setRowAlign(Alignment::center);
    
    table.rows.at(1).columns.at(0).setColumnAlign(Alignment::left);
    table.rows.at(2).columns.at(0).setColumnAlign(Alignment::left);
    table.rows.at(3).columns.at(0).setColumnAlign(Alignment::left);
    table.rows.at(4).columns.at(0).setColumnAlign(Alignment::left);

    table.rows.at(1).columns.at(1).setColumnAlign(Alignment::right);
    table.rows.at(2).columns.at(1).setColumnAlign(Alignment::right);
    table.rows.at(3).columns.at(1).setColumnAlign(Alignment::right);
    table.rows.at(4).columns.at(1).setColumnAlign(Alignment::right);

    table.printTable();
    return 0;
}