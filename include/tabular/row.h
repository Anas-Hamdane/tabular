#pragma once

#include "column.h"

namespace tabular {
class Row {
public:
  constexpr Row() = default;

  Row(std::vector<Column> columns)
    : columns_(std::move(columns))
  {
  }

  std::vector<Column> columns() const { return this->columns_; }
  void columns(std::vector<Column> columns) { this->columns_ = std::move(columns); }

  std::string toStr() const
  {
    if (columns_.empty()) return "";

    std::string row;

    size_t i = 0;
    while (true)
    {
      bool hasColumn = false;
      for (const auto& column : this->columns_)
      {
        const auto lines = column.lines();

        row += '|';
        if (lines.size() <= i)
        {
          row += column.genEmptyLine();
        }
        else
        {
          row += lines[i];
          hasColumn = true;
        }
      }

      row += '|';
      row += '\n';

      if (!hasColumn) break;
      ++i;
    }

    return row;
  }

private:
  std::vector<Column> columns_ = {};
};
}