#include "../include/tabular/table.h"
#include <iostream>

int main()
{
  using namespace tabular;
  Row row({ Column("Hello"), Column("Hi"), Column("How are you") });
  Table table({row});

  table.border(Border::Modern());
  std::cout << table.str() << '\n';
  return 0;
}