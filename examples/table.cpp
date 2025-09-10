#include "../include/tabular/table.h"
#include <iostream>

int main()
{
  using namespace tabular;
  Column column("Hello");
  column.config().width(20);

  Row row({column});
  Table table({row});

  std::cout << table.toStr() << '\n';
  return 0;
}