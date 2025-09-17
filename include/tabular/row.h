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
      : parent_(parent)
    {
    }

    void width(const size_t width)
    {
      parent_.makeDirty();
      width_ = width;
    }
    void hasBottom(bool has)
    {
      parent_.makeDirty();
      hasBottom_ = has;
    }
    void vertical(Border::Part part)
    {
      parent_.makeDirty();
      vertical_ = std::move(part);
    }

    size_t width() const { return width_; }
    bool hasBottom() const { return hasBottom_; }
    const Border::Part& vertical() const { return vertical_; }

    void reset()
    {
      width_ = 50;
      vertical_ = 0;
      parent_.makeDirty();
    }

  private:
    Row& parent_;
    size_t width_ = 50;
    bool hasBottom_ = true;
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
    columns_.reserve(columns.size());

    for (auto& column : columns)
    {
      columns_.emplace_back(std::move(column));
    }
  }

  void columns(std::vector<Column> columns)
  {
    makeDirty();
    columns_ = std::move(columns);
  }

  Config& config() { return config_; }
  const Config& config() const { return config_; }

  std::vector<Column>& columns()
  {
    makeDirty();
    return columns_;
  }
  const std::vector<Column>& columns() const { return columns_; }

  Column& column(int index)
  {
    makeDirty();
    return columns_.at(index);
  }
  const Column& column(int index) const
  {
    return columns_.at(index);
  }

  Column& operator[](int index)
  {
    makeDirty();
    return columns_.at(index);
  }
  const Column& operator[](int index) const
  {
    return columns_.at(index);
  }

  void clr()
  {
    columns_.clear();
    str_.clear();
    config_.reset();
    makeClean();
  }

  const std::string& str() const
  {
    if (dirty_)
    {
      str_ = genStr();
      makeClean();
    }

    return str_;
  }

private:
  std::vector<Column> columns_;
  Config config_{*this};

  // cache
  mutable bool dirty_ = false;
  mutable std::string str_;

  void makeDirty() const { dirty_ = true; }
  void makeClean() const { dirty_ = false; }

  std::string genStr() const
  {
    if (columns_.empty()) return "";
    const size_t maxLines = getMaxLines();

    std::string rowStr;
    rowStr.reserve(config_.width() * maxLines + (columns_.size() + 1));
    const auto& vertical = config().vertical().str();

    for (size_t i = 0; i < maxLines; ++i)
    {
      if (!rowStr.empty()) rowStr.push_back('\n');
      rowStr.append(vertical);

      for (const auto& column : columns_)
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

    for (const auto& column : columns_)
      maxLines = (std::max)(maxLines, column.lines().size());

    return maxLines;
  }
};
// clang-format off
}
