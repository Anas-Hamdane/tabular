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

#include <tabular/table.hpp>
// #include <tabular/tabular.hpp>

#include <iostream>

int main() {
    using namespace tabular;

    Table table;
    table.add_row({"Company", "Market Cap"});
    table.add_row({ "Microsoft", "$3.256 T"});
    table.add_row({ "Apple", "$2.949 T"});
    table.add_row({ "NVIDIA", "$2.863 T"});
    table.add_row({ "Amazon", "$2.039 T"});
    table.add_row({ "Google", "$1.880 T"});

    table.rows.at(0).set_row_align(Alignment::center);
    table.set_width(50);

    table.rows.at(0).set_row_padding(1);
    
    std::cout << table << std::endl;
    return 0;
}