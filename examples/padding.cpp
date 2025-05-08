/*
      __        ___.         .__
    _/  |______ \_ |__  __ __|  | _____ _______
    \   __\__  \ | __ \|  |  \  | \__  \\_  __ \
     |  |  / __ \| \_\ \  |  /  |__/ __ \|  | \/
     |__| (____  /___  /____/|____(____  /__|
               \/    \/                \/

    *  Author: Anas Hamdane
    *  Github: https://github.com/Anas-Hamdane

    * EXAMPLE of a using paddin in a table
*/

// #include <tabular/table.hpp>
#include <tabular/tabular.hpp>

int main() {
    using namespace tabular;

    Table table;
    table.addRow({"Company", "Market Cap"});
    table.addRow({ "Microsoft", "$3.256 T"});
    table.addRow({ "Apple", "$2.949 T"});
    table.addRow({ "NVIDIA", "$2.863 T"});
    table.addRow({ "Amazon", "$2.039 T"});
    table.addRow({ "Google", "$1.880 T"});

    table.rows.at(0).setRowAlign(Alignment::center);
    table.setWidth(50);

    table.rows.at(0).setRowPadding(1);
    
    table.printTable();
    return 0;
}