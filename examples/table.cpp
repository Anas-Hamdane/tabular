#include "../include/tabular/table.h"
#include "../include/tabular/render.h"
#include <iostream>

int main()
{
  using namespace tabular;
  Table table;

  table.addRow({ "Hello World!", "Bonjour le monde!", "こんにちは世界！", "你好世界！" });

  table.border(Border::Modern());
  table[0].config().hasBottom(false);
  table.config().width(100);
  render(table.str(), stdout);
  return 0;
}