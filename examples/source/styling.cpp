/*
      __        ___.         .__
    _/  |______ \_ |__  __ __|  | _____ _______
    \   __\__  \ | __ \|  |  \  | \__  \\_  __ \
     |  |  / __ \| \_\ \  |  /  |__/ __ \|  | \/
     |__| (____  /___  /____/|____(____  /__|
               \/    \/                \/

    *  Author: Anas Hamdane
    *  Github: https://github.com/Anas-Hamdane
    *  LICENSE: https://github.com/Anas-Hamdane/tabular/blob/main/LICENSE
*/

#include <tabular/tabular.hpp>
// #include <tabular/tabular.hpp>

int main(int argc, char* argv[]) {
  using namespace tabular;

  Table table;

  // Add Rows
  table.add_row({"Task ID", "Developer", "Module", "Priority", "Status"})
      .add_row({"001", "Alice Chen", "Authentication", "High", "In Progress"})
      .add_row({"002", "Bob Martinez", "Database", "Medium", "Completed"})
      .add_row({"003", "Carol Kim", "Frontend", "High", "Testing"})
      .add_row({"004", "David Wong", "API Gateway", "Low", "Not Started"})
      .add_row({"005", "Emma Thompson", "Documentation", "Medium", "In Progress"});

  // Header Styling
  for (int col = 0; col < 5; ++col) {
    table[0][col].config().alignment(Alignment::center);
    table[0][col].style().text_attribute(Attribute::bold);
  }

  // Column Width and Alignment
  for (int row = 0; row <= 5; ++row) {
    table[row][0].config().width(9).alignment(Alignment::center);
    table[row][1].config().width(16).alignment(Alignment::center);
    table[row][2].config().width(16).alignment(Alignment::center);
    table[row][3].config().width(10).alignment(Alignment::center);
    table[row][4].config().width(13).alignment(Alignment::center);
  }

  for (int row = 1; row <= 5; ++row) {
    table[row][0].style().text_attribute(Attribute::underline);
    table[row][2].style().text_attribute({Attribute::dim, Attribute::italic});
    table[row][3].config().alignment(Alignment::center);
  }

  // Status Colors
  const std::vector<RGB> status_colors = {
      RGB(34, 197, 94),  // In Progress
      RGB(59, 130, 246), // Completed
      RGB(249, 115, 22), // Testing
      RGB(239, 68, 68),  // Not Started
      RGB(34, 197, 94),  // In Progress
  };

  for (int row = 1; row <= 5; ++row) {
    table[row][4].style().content_color(status_colors[row - 1]);
  }

  // Priority Colors
  const std::vector<RGB> priority_colors = {
      RGB(255, 20, 50),  // High
      RGB(170, 100, 25), // Medium
      RGB(255, 20, 50),  // High
      RGB(100, 180, 0),  // Low
      RGB(170, 100, 25), // Medium
  };

  for (int row = 1; row <= 5; ++row) {
    table[row][3].style().content_color(priority_colors[row - 1]);
    table[row][1].style().column_background_color(priority_colors[row - 1]);
  }

  table.border().set().style(BorderStyle::ansi);
  print(table);

  return 0;
}
