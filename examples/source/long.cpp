/*
      __        ___.         .__
    _/  |______ \_ |__  __ __|  | _____ _______
    \   __\__  \ | __ \|  |  \  | \__  \\_  __ \
     |  |  / __ \| \_\ \  |  /  |__/ __ \|  | \/
     |__| (____  /___  /____/|____(____  /__|
               \/    \/                \/

    *  Author: Anas Hamdane
    *  Github: https://github.com/Anas-Hamdane

    * EXAMPLE of a Long text and a new line in a table
*/

#include <tabular/table.hpp>
// #include <tabular/tabular.hpp>

#include <iostream>

int main() {
    using namespace tabular;

    Table table;
    
    table.add_row({"This a normal paragraph with a very loooooooooooooooooooooooooooooooooooooooooooooooooooooong word", "And this another paragraph\n with a '\\n' inside it"});

    // for testing because "ninja" change the output stream of the program
    table.set().forced_width(60, true);

    std::cout << table << std::endl;
    return 0;
}