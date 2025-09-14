#pragma once

#include "column.h"
#include "border.h"

namespace tabular {
// clang-format off
class Row {
public:
  class Config {
  public:
    explicit Config(Row& parent)
      : parent(parent)
    {
    }

    void width(const size_t width)
    {
      parent.makeDirty();
      this->width_ = width;
    }
    void vertical(Border::Part part)
    {
      parent.makeDirty();
      this->vertical_ = std::move(part);
    }

    size_t width() const { return this->width_; }
    const Border::Part& vertical() const { return this->vertical_; }

    void reset()
    {
      this->width_ = 50;
      this->vertical_ = 0;
    }

  private:
    Row& parent;
    size_t width_ = 50;
    Border::Part vertical_ = 0;
  };

public:
  Row() = default;

  explicit Row(std::vector<Column> columns)
    : columns_(std::move(columns))
  {
  }
  explicit Row(std::vector<std::string> columns)
  {
    this->columns_.reserve(columns.size());

    for (auto& column : columns)
    {
      this->columns_.emplace_back(std::move(column));
    }
  }

  void columns(std::vector<Column> columns)
  {
    makeDirty();
    this->columns_ = std::move(columns);
  }

  Config& config() { return this->config_; }
  const Config& config() const { return this->config_; }

  std::vector<Column>& columns()
  {
    makeDirty();
    return this->columns_;
  }
  const std::vector<Column>& columns() const { return this->columns_; }

  Column& column(int index)
  {
    makeDirty();
    return this->columns_.at(index);
  }
  const Column& column(int index) const
  {
    return this->columns_.at(index);
  }

  Column& operator[](int index)
  {
    makeDirty();
    return this->columns_.at(index);
  }
  const Column& operator[](int index) const
  {
    return this->columns_.at(index);
  }

  void clr()
  {
    this->columns_.clear();
    this->str_.clear();
    this->config_.reset();
    this->dirty_ = false;
  }

  const std::string& str() const
  {
    if (this->dirty_)
    {
      this->str_ = genStr();
      makeClean();
    }

    return this->str_;
  }

private:
  std::vector<Column> columns_;
  Config config_ = Config(*this);

  // cache
  mutable bool dirty_ = false;
  mutable std::string str_;

  void makeDirty() const { this->dirty_ = true; }
  void makeClean() const { this->dirty_ = false; }

  std::string genStr() const
  {
    if (this->columns_.empty()) return "";
    const size_t maxLines = getMaxLines();

    std::string rowStr;
    rowStr.reserve(this->config_.width() * maxLines + (this->columns_.size() + 1));
    const auto vertical = config().vertical();

    for (size_t i = 0; i < maxLines; ++i)
    {
      if (!rowStr.empty()) rowStr.push_back('\n');
      rowStr.append(vertical);

      for (const auto& column : this->columns_)
      {
        const auto& lines = column.lines();

        if (lines.size() > i)
          rowStr.append(lines[i]);
        else
          rowStr.append(column.genEmptyLine());

        rowStr.append(vertical);
      }
    }

    return rowStr;
  }
  size_t getMaxLines() const
  {
    size_t maxLines = 0;

    for (const auto& column : this->columns_)
      maxLines = (std::max)(maxLines, column.lines().size());

    return maxLines;
  }
};
// clang-format off
}