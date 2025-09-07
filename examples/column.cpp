#include "../include/tabular/column.h"
#include <iostream>

int main()
{
  using namespace tabular;
  Column column("Hello, World!");

  column.config().width(15);
  column.style().fg(Color::BrightRed);

  const auto lines = column.lines();

  for (const auto& line : lines)
    std::cout << line << '\n';

  return 0;
}