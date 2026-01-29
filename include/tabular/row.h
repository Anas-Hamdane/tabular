#pragma once

#include "column.h"
#include "border.h"

namespace tabular {
// clang-format off
class Row {
public:
  class Config {
  public:
    explicit Config(bool& dirty_)
      : dirty_(dirty_)
    {
    }

    void hasBottom(bool has)
    {
      dirty_ = true;
      hasBottom_ = has;
    }
    void vertical(Border::Part part)
    {
      dirty_ = true;
      vertical_ = std::move(part);
    }

    bool hasBottom() const { return hasBottom_; }
    const Border::Part& vertical() const { return vertical_; }

    void reset()
    {
      vertical_ = 0;
      dirty_ = true;
    }

  private:
    friend class Row;

    bool& dirty_;
    bool hasBottom_ = true;
    Border::Part vertical_ = 0;
  };

public:
  Row() = default;

  // copy contructor
  // NOTE: it's mandatory to initialize config_ with dirty_ to avoid a heap-use-after-free error
  // from the address sanitizer, then copy the members of the other.config_ into our new one
  // See column.h:200 note
  Row(const Row& other)
    : columns_(other.columns_), dirty_(other.dirty_), str_(other.str_), 
    config_(dirty_) // CRUCIAL
  {
    config_.hasBottom_ = other.config_.hasBottom_;
    config_.vertical_ = other.config_.vertical_;
  }

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
    dirty_ = true;
    columns_ = std::move(columns);
  }

  Config& config() { return config_; }
  const Config& config() const { return config_; }

  std::vector<Column>& columns()
  {
    dirty_ = true;
    return columns_;
  }
  const std::vector<Column>& columns() const { return columns_; }

  Column& column(int index)
  {
    dirty_ = true;
    return columns_.at(index);
  }
  const Column& column(int index) const
  {
    return columns_.at(index);
  }

  Column& operator[](int index)
  {
    dirty_ = true;
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
    dirty_ = false;
  }

  const std::string& str() const
  {
    if (dirty_)
    {
      str_ = genStr();
      dirty_ = false;
    }

    return str_;
  }

private:
  // cache
  mutable bool dirty_ = false;
  mutable std::string str_;

  std::vector<Column> columns_;
  Config config_{dirty_};

  std::string genStr() const
  {
    if (columns_.empty()) return "";
    const size_t maxLines = getMaxLines();

    std::string rowStr;
    // average
    rowStr.reserve(DEFAULT_WIDTH * maxLines + (columns_.size() + 1));

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
