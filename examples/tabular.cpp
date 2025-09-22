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
    "           \\/    \\/                \\/\n",
  })
  .addRow({
    "lightweight header-only library for constructing well-formatted,"
    " fully-customizable CLI tables."
  });

  // set a padding horizontally (left and right) and vertically (top and bottom)
  table[0][0].config().padd(Padd(0, 0));

  // set the alignment of the content of the column
  table[1][0].config().align(Align::Center);

  // don't skip empty whitespaces at the start of an empty line
  // this helps when spaces at the start of a line are important
  table[0][0].config().skipEmptyLineIndent(false);

  // set the width of the table
  table.config().width(48);

  // set the border style of the table
  table.border(Border::Rounded());

  // render the table(or any string) in a cross-platform way
  render(table.str() + '\n', stdout);
  return 0;
}
