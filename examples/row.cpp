#include "../include/tabular/row.h"
#include <iostream>

int main()
{
  using namespace tabular;
  std::vector<Column> columns;

  columns.emplace_back("Hello");
  columns.emplace_back("Hi");
  columns.emplace_back("Hi");

  columns[0].config().width(10);
  columns[1].config().width(15);
  columns[2].config().width(10);

  const Row row(columns);

  std::cout << row.str();

  return 0;
}
