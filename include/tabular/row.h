#pragma once

#include "column.h"

namespace tabular {
class Row {
public:
  class Config {
  public:
    explicit Config(Row& parent)
      : parent(parent)
    {
    }

    void width(const size_t width) { this->width_ = width; }

    size_t width() const { return this->width_; }

  private:
    Row& parent;
    size_t width_ = 50;
  };

public:
  constexpr Row() = default;

  Row(std::vector<Column> columns)
    : columns_(std::move(columns))
  {
  }

  std::vector<Column> columns() const { return this->columns_; }
  void columns(std::vector<Column> columns) { this->columns_ = std::move(columns); }

  Config& config() { return this->config_; }
  const Config& config() const { return this->config_; }

  std::string toStr() const
  {
    if (this->columns_.empty()) return "";
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
      if (!hasColumn) break;

      row += '\n';
      ++i;
    }

    return row;
  }

private:
  std::vector<Column> columns_ = {};
  Config config_ = Config(*this);

  friend class Table;
};
}