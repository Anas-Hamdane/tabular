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
#include <tabular/table.hpp>

int main() {
    using namespace tabular;

    Table table;
    table.add_row({"Countries Capitals"});
    table.add_row({"United States", "Washington"});
    table.add_row({"Brazil", "Brasilia"});
    table.add_row({"Countries Population"});
    table.add_row({"United States", "340.1 million (2024)"});
    table.add_row({"Brazil", "211.1 million (2023)"});

    std::cout << table << std::endl;
    return 0;
}