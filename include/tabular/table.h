#pragma once

#include "row.h"
#include "border.h"

namespace tabular {

namespace detail {
inline bool bisearch(const std::vector<size_t>& vec, size_t value)
{
  size_t left = 0;
  size_t right = vec.size();

  while (left < right)
  {
    size_t mid = left + (right - left) / 2;

    if (vec[mid] == value) return true;

    if (vec[mid] < value) left = mid + 1;
    else right = mid;
  }
  return false;
}
}

class Table {
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
  constexpr Table() = default;

  Table(std::vector<Row> rows, Border border = {})
    : rows_(std::move(rows)), border_(border)
  {
  }

  std::vector<Row> rows();
  void rows(std::vector<Row> rows);

  Config& config() { return this->config_; }
  const Config& config() const { return this->config_; }

  Border& border() { return this->border_; }
  const Border& border() const { return this->border_; }

  std::string toStr() const
  {
    if (this->rows_.empty()) return "";

    std::string table;
    adjustWidth();

    table += getBorderHeader() + '\n';

    const size_t rowsSize = this->rows_.size();
    for (size_t i = 0; i < rowsSize; ++i)
    {
      const auto& row = this->rows_[i];

      table += row.toStr() + '\n';
      if (i + 1 < rowsSize) table += getBorderMiddle(i) + '\n';
    }

    table += getBorderFooter();
    return table;
  }

private:
  mutable std::vector<Row> rows_ = {};
  Config config_ = {};
  Border border_ = {};

  size_t calculateWidth(const Row& row, size_t& unspecified) const
  {
    const auto& columns = row.columns();
    if (columns.empty()) return 0;
    size_t width = columns.size() + 1;

    for (const auto& column : columns)
    {
      size_t w = column.config().width();

      if (w == 0) unspecified++;
      else width += w;
    }

    return width;
  }
  void setWidth(Row& row) const
  {
    auto& columns = row.columns_;
    size_t width = config().width();

    // subtract the splits
    width -= (columns.size() + 1);
    size_t indivWidth = width / columns.size();
    size_t rest = width % columns.size();

    for (auto& column : columns)
    {
      column.config().width(indivWidth + (rest > 0));
      rest -= (rest > 0);
    }

    // if there's still a rest
    if (rest > 0) columns.back().config().width(indivWidth + rest);
  }
  void setUnspecifiedWidth(Row& row, size_t unspecified) const
  {
    auto& columns = row.columns_;
    size_t width = config().width();

    // subtract the splits
    width -= (columns.size() + 1);
    size_t indivWidth = width / unspecified;
    size_t rest = width % unspecified;

    size_t lastUnsp = 0;
    for (size_t i = 0; i < columns.size(); ++i)
    {
      auto& column = columns[i];
      if (column.config().width() != 0) continue;

      column.config().width(indivWidth + (rest > 0));
      rest -= (rest > 0);
      lastUnsp = i;
    }

    // if there's still a rest
    if (rest > 0) columns[lastUnsp].config().width(indivWidth + rest);
  }

  void adjustWidth() const
  {
    if (this->rows_.empty()) return;
    const size_t width = config().width();

    for (Row& row : this->rows_)
    {
      const size_t estimatedWidth = config().width();

      size_t unspecified = 0;
      const size_t rowWidth = calculateWidth(row, unspecified);

      if (rowWidth < estimatedWidth && unspecified != 0)
        setUnspecifiedWidth(row, unspecified);
      else
        setWidth(row);
    }
  }

  std::vector<size_t> connections(const Row& row) const
  {
    std::vector<size_t> connections;
    connections.reserve(row.columns().size());

    size_t track = 1;
    for (const Column& column : row.columns())
    {
      track += column.config().width();
      connections.push_back(track++);
    }

    return connections;
  }
  std::string getBorderHeader() const
  {
    const auto& columns = this->rows_[0].columns();

    std::string header = this->border_.cornerTopLeft();
    header.reserve(config().width());

    for (size_t i = 0; i < columns.size(); ++i)
    {
      size_t width = columns[i].config().width();

      for (size_t j = 0; j < width; ++j)
          header += this->border_.horizontal();

      if (i + 1 < columns.size()) header += this->border_.connectorTop();
    }

    header += this->border_.cornerTopRight();
    return header;
  }
  std::string getBorderFooter() const
  {
    const auto& columns = this->rows_.back().columns();

    std::string footer = this->border_.cornerBottomLeft();
    footer.reserve(config().width());

    for (size_t i = 0; i < columns.size(); ++i)
    {
      size_t width = columns[i].config().width();

      for (size_t j = 0; j < width; ++j)
        footer += this->border_.horizontal();

      if (i + 1 < columns.size()) footer += this->border_.connectorBottom();
    }

    footer += this->border_.cornerBottomRight();
    return footer;
  }
  std::string getBorderMiddle(size_t index) const
  {
    const auto nextRowConnections = connections(this->rows_[index + 1]);
    const auto& border = this->border_; // I like this alias

    std::string middle = border.connectorLeft();
    middle.reserve(config().width());

    const auto& columns = this->rows_[index].columns();

    size_t tracker = 0;
    for (size_t i = 0; i < columns.size(); ++i)
    {
      size_t width = columns[i].config().width();

      for (size_t j = 0; j < width; ++j)
      {
        if (detail::bisearch(nextRowConnections, ++tracker))
          middle += border.connectorTop();
        else
          middle += border.horizontal();
      }
      tracker++;

      if (i + 1 >= columns.size()) continue;

      if (detail::bisearch(nextRowConnections, tracker))
        middle += border.intersection();
      else
        middle += border.connectorBottom();
    }

    middle += border.connectorRight();
    return middle;
  }
};
}