#pragma once

#include "column.h"
#include "border.h"

namespace tabular {
// clang-format off
class Row {
public:
  class Config {
  public:
    explicit Config() = default;

    void hasBottom(bool has)
    {
      setModified(true);
      hasBottom_ = has;
    }
    void vertical(Border::Part part)
    {
      setModified(true);
      vertical_ = std::move(part);
    }

    bool hasBottom() const { return hasBottom_; }
    const Border::Part& vertical() const { return vertical_; }

    void reset()
    {
      vertical_ = 0;
      hasBottom_ = true;
      setModified(true);
    }


  private:
    bool hasBottom_ = true;
    Border::Part vertical_ = 0;

    // for caching
    friend class Row;
    mutable bool modified = false;
    bool isModified() const { return this->modified; }
    void setModified(bool modified) const { this->modified = modified; }
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
    setModified(true);
    columns_ = std::move(columns);
  }

  Config& config() { return config_; }
  const Config& config() const { return config_; }
  std::vector<Column>& columns() { return columns_; }
  const std::vector<Column>& columns() const { return columns_; }

  Column& column(int index) { return columns_.at(index); }
  const Column& column(int index) const { return columns_.at(index); }

  Column& operator[](int index) { return columns_.at(index); }
  const Column& operator[](int index) const { return columns_.at(index); }

  void clr()
  {
    columns_.clear();
    str_.clear();
    config_.reset();
    setModified(false);
  }

  const std::string& str() const
  {
    if (isModified())
    {
      // note that when genStr() calls column.lines() which sets
      // modified to false for each column so we don't need to
      str_ = genStr();
      this->modified = false;
      config_.setModified(false);
    }

    return str_;
  }

  bool isModified() const
  {
    if (this->modified || config_.isModified()) return true;
    for (const auto& column : columns_)
    {
      if (column.isModified()) return true;
    }

    return false;
  }
  void setModified(bool modified)
  {
    this->modified = modified;
  }

private:
  // cache
  mutable bool modified = false;
  mutable std::string str_;

  std::vector<Column> columns_;
  Config config_;

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
