/*
      __        ___.         .__
    _/  |______ \_ |__  __ __|  | _____ _______
    \   __\__  \ | __ \|  |  \  | \__  \\_  __ \
     |  |  / __ \| \_\ \  |  /  |__/ __ \|  | \/
     |__| (____  /___  /____/|____(____  /__|
               \/    \/                \/

    *  Author: Anas Hamdane
    *  Github: https://github.com/Anas-Hamdane

    * EXAMPLE of an ANSI table
*/

#include <tabular/table.hpp>
// #include <tabular/tabular.hpp>

int main() {
    using namespace tabular;

    Table table;
    table.add_row({"University", "Score (IT)"});
    table.add_row({"Massachusetts Institute of Technology (MIT)", "94.2"});
    table.add_row({"Stanford University", "92.1"});
    table.add_row({"Carnegie Mellon University", "91.2"});
    table.add_row({"National University of Singapore (NUS)", "90.6"});

    table.rows[0].set_row_padding(2);

    table.format().border(BorderStyle::ANSI);

    std::cout << table << std::endl;
    return 0;
}