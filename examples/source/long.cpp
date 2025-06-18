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

#include <tabular/printer.hpp>
// #include <tabular/tabular.hpp>

#include <iostream>

int main() {
  using namespace tabular;

  Table table;

  table.add_row({"This is a normal paragraph with a very loooooooooooooooooooooooooooooooooooooooooooooooooooooong word", "And this is another paragraph\nwith a '\\n' inside it"});

  table.set().width(60);

  std::cout << table << std::endl;
  return 0;
}
