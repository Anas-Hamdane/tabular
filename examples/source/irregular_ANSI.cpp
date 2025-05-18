/*
      __        ___.         .__
    _/  |______ \_ |__  __ __|  | _____ _______
    \   __\__  \ | __ \|  |  \  | \__  \\_  __ \
     |  |  / __ \| \_\ \  |  /  |__/ __ \|  | \/
     |__| (____  /___  /____/|____(____  /__|
               \/    \/                \/

    *  Author: Anas Hamdane
    *  Github: https://github.com/Anas-Hamdane

    * EXAMPLE of an irregular ANSI table
*/

#include <tabular/table.hpp>
// #include <tabular/tabular.hpp>

#include <iostream>

int main() {
    using namespace tabular;

    Table table;
    table.add_row({"Top 5 Math universities in the world"});
    table.add_row({"Massachusetts Institute of Technology (MIT)", "95.5"});
    table.add_row({"Harvard University", "93.2"});
    table.add_row({"University of Oxford", "93.1"});
    table.add_row({"University of Cambridge", "92.5"});
    table.add_row({"Stanford University", "91.6"});

    table[0].config().padding(1);
    table.config().alignment(Alignment::center);

    table.format().border(BorderStyle::ANSI);
    
    std::cout << table << std::endl;
    return 0;
}