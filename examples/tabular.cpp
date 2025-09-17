#include "../include/tabular/table.h"
#include "../include/tabular/render.h"

int main()
{
  using namespace tabular;
  Table table;

  table.addRow({
    "   __        ___.         .__\n"
  " _/  |______ \\_ |__  __ __|  | _____ _______\n"
  " \\   __\\__  \\ | __ \\|  |  \\  | \\\\__  \\_  __ \\\n"
   "  |  |  / __ \\| \\_\\ \\  |  /  |__/ __ \\|  | \\/\n"
   "  |__| (____  /___  /____/|____(____  /__|\n"
             "            \\/    \\/                \\/\n"
  });

  table[0][0].config().skipEmptyLineIndent(false);
  render(table.str(), stdout);
  return 0;
}
