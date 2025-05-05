/*
      __        ___.         .__
    _/  |______ \_ |__  __ __|  | _____ _______
    \   __\__  \ | __ \|  |  \  | \__  \\_  __ \
     |  |  / __ \| \_\ \  |  /  |__/ __ \|  | \/
     |__| (____  /___  /____/|____(____  /__|
               \/    \/                \/

    *  Author: Anas Hamdane
    *  Github: https://github.com/Anas-Hamdane

    * EXAMPLE of a Long text in a table
*/

#include "../include/tabular/tabular.hpp"

int main() {
    using namespace tabular;

    Table table;
    table.addRow({"this is a very long paragraph with a very long wooooooooooooooooooooooooooooooooooooooooooooooooooooooord"}); // 6

    table.printTable();
    return 0;
}