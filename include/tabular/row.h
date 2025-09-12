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
    const size_t maxLines = getMaxLines();

    std::string rowStr;
    rowStr.reserve(config().width() * maxLines + (this->columns_.size() + 1));

    for (size_t i = 0; i < maxLines; ++i)
    {
      if (!rowStr.empty()) rowStr.push_back('\n');
      rowStr.push_back('|');

      for (const auto& column : this->columns_)
      {
        const auto& lines = column.lines();

        if (lines.size() > i)
          rowStr += lines[i];
        else
          rowStr += column.genEmptyLine();

        rowStr.push_back('|');
      }
    }

    return rowStr;
  }

private:
  std::vector<Column> columns_ = {};
  Config config_ = Config(*this);

  size_t getMaxLines() const
  {
    size_t maxLines = 0;

    for (const auto& column : this->columns_)
      maxLines = (std::max)(maxLines, column.lines().size());

    return maxLines;
  }
  friend class Table;
};
}