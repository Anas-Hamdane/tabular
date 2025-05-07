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

#include <tabular/tabular.hpp>

int main() {
    using namespace tabular;

    Table table;
    
    table.addRow({"This a normal paragraph with a very loooooooooooooooooooooooooooooooooooooooooooooooooooooong word", "And this another paragraph\n with a '\\n' inside it"});

    table.printTable();
    return 0;
}