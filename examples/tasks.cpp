#include "../include/tabular/table.h"
#include "../include/tabular/render.h"

/*
 * The width system
 *
 * Since the library aims to be as lightweight as possible, the width
 * system is a little bit tricky.
 *
 * By default, the width of a fresh constructed `Table` instance will be 50
 * you can change it using `table.config().width(size_t)` call which forces
 * the table to match that width.
 *
 * With 'forces' I mean it literally forces the sum of the columns widths to be
 * exactly equal to the width of the table minus the space preserved for the vertical
 * borders, in other words it should be equal to `width - (columns.size() + 1)`.
 *
 * So to set a specific width for the table you just need to call
 * `table.config().width(size_t)`.
 *
 * And to set the width of SOME columns (NOT ALL OF THEM), the sum of their widths
 * MUST NOT exceed the total width of the table, and must leave enough space for
 * the other columns.
 *
 * But to set the width of ALL the columns the sum of their widths MUST be exactly
 * equal to the width of the table minus the space preserved for the vertical borders
 * which is `width - (columns.size() + 1)`.
 */

// it's bad but for convenience
using namespace tabular;

void adjustWidth(Table& table)
{
  // the first column
  for (size_t i = 0; i < 6; ++i) table[i][0].config().width(5);

  // the second column
  for (size_t i = 0; i < 6; ++i) table[i][1].config().width(20);

  // the third column
  for (size_t i = 0; i < 6; ++i) table[i][2].config().width(16);

  // the fourth column
  for (size_t i = 0; i < 6; ++i) table[i][3].config().width(10);

  // the fifth column
  for (size_t i = 0; i < 6; ++i) table[i][4].config().width(13);

  // the total width set: 5 + 20 + 16 + 10 + 13 = 64
  // `width - (columns.size() + 1)`: 70 - (5 + 1) = 64
  // so the width will get applied without any problems
  table.config().width(70);
}
void styleTable(Table& table)
{
  // underline the numbers
  for (size_t i = 1; i < 6; ++i) table[i][0].style().attrs(Attr::Underline);

  // priority colors
  Rgb high = {255, 0, 0};
  Rgb medium = {255, 165, 0};
  Rgb low = {165, 255, 0};

  // the base of the names
  table[1][1].style().base(high);
  table[2][1].style().base(medium);
  table[3][1].style().base(high);
  table[4][1].style().base(low);
  table[5][1].style().base(medium);

  // dim the task description
  for (size_t i = 1; i < 6; ++i) table[i][2].style().attrs(Attr::Dim);

  // the color of the priorities
  table[1][3].style().fg(high);
  table[2][3].style().fg(medium);
  table[3][3].style().fg(high);
  table[4][3].style().fg(low);
  table[5][3].style().fg(medium);

  // the status
  table[1][4].style().fg(Color::Blue);
  table[2][4].style().fg(Color::Green);
  table[3][4].style().fg(Color::Yellow);
  table[4][4].style().fg(Color::Red);
  table[5][4].style().fg(Color::Blue);
}

// center all the columns
void alignColumns(Table& table)
{
  for (auto& row : table.rows())
  {
    for (auto& column : row.columns())
    {
      column.config().align(Align::Center);
    }
  }
}

int main()
{
  Table table;

  table.addRow({"ID", "Dev", "Task", "Priority", "Status"})
      .addRow({"001", "Alice Carlisle", "Authentication", "High", "In Progress"})
      .addRow({"002", "Henry Dawson", "Database", "Medium", "Completed"})
      .addRow({"003", "Carol Bennett", "Frontend", "High", "Testing"})
      .addRow({"004", "Amelia Turner", "API", "Low", "Not Started"})
      .addRow({"005", "Charlotte Whitaker", "Documentation", "Medium", "In Progress"});

  adjustWidth(table);
  styleTable(table);
  alignColumns(table);

  // rounded borders
  table.border(Border::Rounded());

  render(table.str() + '\n', stdout);
  return 0;
}