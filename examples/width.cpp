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

#include <tabular/table.hpp>

int main() {
    using namespace tabular;

    Table table;
    table.addRow({"5-Top Largest Countries"});
    table.addRow({"#1", "Russia"});
    table.addRow({"#2", "Canada"});
    table.addRow({"#3", "USA"});
    table.addRow({"#4", "China"});
    table.addRow({"#5", "Brazil"});

    table.setWidth(50);
 
    table.rows.at(1).columns.at(0).setWidth(4);
    table.rows.at(2).columns.at(0).setWidth(4);
    table.rows.at(3).columns.at(0).setWidth(4);
    table.rows.at(4).columns.at(0).setWidth(4);
    table.rows.at(5).columns.at(0).setWidth(4);
 
    table.printTable();
    return 0;
}