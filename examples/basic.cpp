#include "../include/tabular/table.h"
#include "../include/tabular/render.h"

int main()
{
  using namespace tabular;
  Table table;

  table.addRow({"Countries Capitals"})
      .addRow({"United States", "Washington"})
      .addRow({"Brazil", "Brasilia"})
      .addRow({"France", "Paris"})
      .addRow({"Japan", "Tokyo"})
      .addRow({"India", "New Delhi"});

  render(table.str(), stdout);
  return 0;
}
