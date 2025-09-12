#pragma once

#include "column.h"
#include "border.h"

namespace tabular {
class Row {
public:
  class Config {
  public:
    Config(Row& parent)
      : parent(parent)
    {
    }

    void width(const size_t width)
    {
      parent.regenerate_ = true;
      this->width_ = width;
    }
    void vertical(Border::Part vertical)
    {
      parent.regenerate_ = true;
      this->vertical_ = std::move(vertical);
    }

    size_t width() const { return this->width_; }
    Border::Part vertical() const { return this->vertical_; }

  private:
    Row& parent;
    size_t width_ = 50;
    Border::Part vertical_ = '\0';
  };

public:
  Row() = default;

  Row(std::vector<Column> columns)
    : columns_(std::move(columns))
  {
  }

  std::vector<Column>& columns()
  {
    this->regenerate_ = true;
    return this->columns_;
  }
  const std::vector<Column>& columns() const { return this->columns_; }
  void columns(std::vector<Column> columns)
  {
    this->regenerate_ = true;
    this->columns_ = std::move(columns);
  }

  Config& config() { return this->config_; }
  const Config& config() const { return this->config_; }

  std::string toStr() const
  {
    if (this->regenerate_) this->str = reGenStr();
    return this->str;
  }

  std::string reGenStr() const
  {
    if (this->columns_.empty()) return "";
    const size_t maxLines = getMaxLines();

    std::string rowStr;
    rowStr.reserve(this->config_.width() * maxLines + (this->columns_.size() + 1));
    const auto vertical = config().vertical();

    for (size_t i = 0; i < maxLines; ++i)
    {
      if (!rowStr.empty()) rowStr.push_back('\n');
      rowStr += vertical;

      for (const auto& column : this->columns_)
      {
        const auto& lines = column.lines();

        if (lines.size() > i)
          rowStr += lines[i];
        else
          rowStr += column.genEmptyLine();

        rowStr += vertical;
      }
    }

    return rowStr;
  }

private:
  std::vector<Column> columns_ = {};
  Config config_ = Config(*this);

  // cache
  mutable bool regenerate_ = false;
  mutable std::string str;

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