#pragma once

#include "column.h"

namespace tabular {
class Row {
public:
  class Config {
  public:
    constexpr Config() = default;

    void width(const size_t width) { this->width_ = width; }
    size_t width() const { return this->width_; }

  private:
    size_t width_ = 50;
  };

public:
  constexpr Row() = default;

  Row(std::vector<Column> columns)
    : columns_(std::move(columns))
  {
  }

  std::vector<Column> columns() const { return this->columns_; }
  void columns(std::vector<Column> columns) const
  {
    this->columns_ = std::move(columns);
  }

  Config& config() { return this->config_; }
  const Config& config() const { return this->config_; }

  std::string toStr() const
  {
    if (columns_.empty()) return "";

    const size_t estimatedWidth = config().width();

    size_t unspecified = 0;
    size_t rowWidth = calculateWidth(unspecified);

    if (rowWidth < estimatedWidth && unspecified != 0)
      adjustUnspecifiedWidth(unspecified);
    else
      adjustWidth();

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
  // make sure it is not changeable EXCEPT in `adjustWidth()`
  // since before printing we have to make sure all the columns
  // have the correct width set
  mutable std::vector<Column> columns_ = {};
  Config config_;

  size_t calculateWidth(size_t& unspecified) const
  {
    if (columns_.empty()) return 0;
    size_t width = this->columns_.size() + 1;

    for (const auto& column : this->columns_)
    {
      size_t w = column.config().width();

      if (w == 0) unspecified++;
      else width += w;
    }

    return width;
  }
  void adjustWidth() const
  {
    size_t width = config().width();

    // subtract the splits
    width -= (this->columns_.size() + 1);
    size_t indivWidth = width / this->columns_.size();
    size_t rest = width % this->columns_.size();

    for (auto& column : this->columns_)
    {
      column.config().width(indivWidth + (rest > 0));
      rest -= (rest > 0);
    }

    // if there's still a rest
    if (rest > 0) this->columns_.back().config().width(indivWidth + rest);
  }
  void adjustUnspecifiedWidth(size_t unspecified) const
  {
    size_t width = config().width();

    // subtract the splits
    width -= (this->columns_.size() + 1);
    size_t indivWidth = width / unspecified;
    size_t rest = width % unspecified;

    size_t lastUnsp = 0;
    for (size_t i = 0; i < this->columns_.size(); ++i)
    {
      auto& column = this->columns_[i];
      if (column.config().width() != 0) continue;

      column.config().width(indivWidth + (rest > 0));
      rest -= (rest > 0);
      lastUnsp = i;
    }

    // if there's still a rest
    if (rest > 0) this->columns_[lastUnsp].config().width(indivWidth + rest);
  }
};
}