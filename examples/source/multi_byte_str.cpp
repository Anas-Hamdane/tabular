/*
      __        ___.         .__
    _/  |______ \_ |__  __ __|  | _____ _______
    \   __\__  \ | __ \|  |  \  | \__  \\_  __ \
     |  |  / __ \| \_\ \  |  /  |__/ __ \|  | \/
     |__| (____  /___  /____/|____(____  /__|
               \/    \/                \/

    *  Author: Anas Hamdane
    *  Github: https://github.com/Anas-Hamdane

    * EXAMPLE of using multi byte characters in a table
*/

#include <tabular/table.hpp>
// #include <tabular/tabular.hpp>

#include <iostream>

int main() {
    using namespace tabular;

    Table table;
    table.add_row({"English", "Hello"});
    table.add_row({"Korean", "안녕하세요"});
    table.add_row({"Japanese", "こんにちは"});
    table.add_row({"Russian", "Привет"});

    // for testing because "ninja" change the output stream of the program
    table.set().forced_width(60, true);

    std::cout << table << std::endl;
    return 0;
}
