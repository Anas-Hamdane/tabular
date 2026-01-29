#pragma once

#include "row.h"
#include <stdexcept>

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

// clang-format off
class Table {
public:
  class Config {
  public:
    explicit Config(bool& dirty_) : dirty_(dirty_)
    {
    }

    void width(const size_t width)
    {
      dirty_ = true;
      width_ = width;
    }

    size_t width() const { return width_; }

    void reset()
    {
      width_ = DEFAULT_WIDTH;
      dirty_ = true;
    }

  private:
    bool& dirty_;
    size_t width_ = DEFAULT_WIDTH;
  };

public:
  Table() = default;

  explicit Table(std::vector<Row> rows, Border border = {})
    : rows_(std::move(rows)), border_(std::move(border)), dirty_(true)
  {
  }

  void rows(std::vector<Row> rows)
  {
    rows_ = std::move(rows);
    dirty_ = true;
  }
  void border(Border border)
  {
    border_ = std::move(border);
    dirty_ = true;
  }

  Table& addRow(Row row)
  {
    rows_.emplace_back(std::move(row));
    dirty_ = true;
    return *this;
  }
  Table& addRow(std::vector<std::string> row)
  {
    rows_.emplace_back(std::move(row));
    dirty_ = true;
    return *this;
  }

  Config& config() { return config_; }
  const Config& config() const { return config_; }

  Border& border()
  {
    dirty_ = true;
    return border_;
  }
  const Border& border() const { return border_; }

  std::vector<Row>& rows()
  {
    dirty_ = true;
    return rows_;
  }
  const std::vector<Row>& rows() const { return rows_; }

  Row& row(int index)
  {
    dirty_ = true;
    return rows_.at(index);
  }
  const Row& row(int index) const { return rows_.at(index); }

  Row& operator[](int index)
  {
    dirty_ = true;
    return rows_.at(index);
  }
  const Row& operator[](int index) const
  {
    return rows_.at(index);
  }

  void clr()
  {
    rows_.clear();
    config_.reset();
    border_.reset();
    str_.clear();
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

  std::vector<Row> rows_;
  Config config_{dirty_};
  Border border_;

  std::string genStr() const
  {
    // avoid reference
    auto rows = rows_;
    if (rows.empty()) return "";

    std::string tableStr;
    tableStr.reserve(rows.size() * config_.width());

    adjustWidth(rows);
    configureRows(rows);

    tableStr += getBorderHeader(rows) + '\n';

    const size_t rowsSize = rows_.size();
    for (size_t i = 0; i < rowsSize; ++i)
    {
      auto& row = rows[i];

      tableStr += row.str() + '\n';

      if (i + 1 < rowsSize && row.config().hasBottom())
        tableStr += getBorderMiddle(rows, i) + '\n';
    }

    tableStr += getBorderFooter(rows);
    return tableStr;
  }
  static size_t calculateWidth(const Row& row, size_t& unspecified)
  {
    const auto& columns = row.columns();
    if (columns.empty()) return 0;
    size_t width = 0;

    for (const auto& column : columns)
    {
      size_t w = column.config().width();

      if (w == 0) unspecified++;
      else width += w;
    }

    return width;
  }

  static void setWidth(Row& row, size_t width)
  {
    auto& columns = row.columns();

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

  static void setUnspecifiedWidth(Row& row, size_t unspecified, size_t width)
  {
    std::vector<Column>& columns = row.columns();

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

  void adjustWidth(std::vector<Row>& rows) const
  {
    if (rows.empty()) return;

    size_t width = config_.width();
    for (size_t i = 0; i < rows.size(); ++i)
    {
      Row& row = rows[i];

      // check the minimum width first
      {
        size_t minWidth = (row.columns().size() * (MIN_COLUMN_WIDTH + 1)) + 1;
        if (minWidth > width)
        {
          throw std::runtime_error(
            "layout error: row " + std::to_string(i) +
            " must a minimum width of " + std::to_string(minWidth) +
            ", but found " + std::to_string(width));
        }
      }

      const size_t estimatedWidth = width - (row.columns().size() + 1);

      size_t unspecified = 0;
      const size_t rowWidth = calculateWidth(row, unspecified);

      // everything is fine skip
      if (rowWidth == estimatedWidth && unspecified == 0)
        continue;

      // set just the unspecified columns widths
      if (rowWidth < estimatedWidth && unspecified != 0)
        setUnspecifiedWidth(row, unspecified, estimatedWidth - rowWidth);

      // set/restore the width of all the columns
      else
        setWidth(row, estimatedWidth);
    }
  }
  void configureRows(std::vector<Row>& rows) const
  {
    auto& verticalBorder = border_.vertical();
    for (auto& row : rows)
    {
      auto& config = row.config();

      if (config.vertical().glyph() == '\0')
        config.vertical(verticalBorder);
    }
  }

  static std::vector<size_t> connections(const Row& row)
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
  std::string getBorderHeader(std::vector<Row>& rows) const
  {
    const auto& columns = rows[0].columns();

    std::string header = border_.cornerTopLeft().str();
    header.reserve(config_.width());

    const std::string& horizontal = border_.horizontal().str();
    for (size_t i = 0; i < columns.size(); ++i)
    {
      size_t width = columns[i].config().width();

      for (size_t j = 0; j < width; ++j)
        header += horizontal;

      if (i + 1 < columns.size()) header += border_.connectorTop().str();
    }

    header += border_.cornerTopRight().str();
    return header;
  }
  std::string getBorderFooter(std::vector<Row>& rows) const
  {
    const auto& columns = rows.back().columns();

    std::string footer = border_.cornerBottomLeft().str();
    footer.reserve(config_.width());

    const std::string& horizontal = border_.horizontal().str();
    for (size_t i = 0; i < columns.size(); ++i)
    {
      size_t width = columns[i].config().width();

      for (size_t j = 0; j < width; ++j)
        footer += horizontal;

      if (i + 1 < columns.size()) footer += border_.connectorBottom().str();
    }

    footer += border_.cornerBottomRight().str();
    return footer;
  }
  std::string getBorderMiddle(std::vector<Row>& rows, size_t index) const
  {
    const auto nextRowConnections = connections(rows[index + 1]);

    std::string middle = border_.connectorLeft().str();
    middle.reserve(config_.width());

    const auto& columns = rows[index].columns();

    size_t tracker = 0;
    const std::string& horizontal = border_.horizontal().str();
    for (size_t i = 0; i < columns.size(); ++i)
    {
      size_t width = columns[i].config().width();

      for (size_t j = 0; j < width; ++j)
      {
        if (detail::bisearch(nextRowConnections, ++tracker))
          middle += border_.connectorTop().str();
        else
          middle += horizontal;
      }
      tracker++;

      if (i + 1 >= columns.size()) continue;

      if (detail::bisearch(nextRowConnections, tracker))
        middle += border_.intersection().str();
      else
        middle += border_.connectorBottom().str();
    }

    middle += border_.connectorRight().str();
    return middle;
  }
};
// clang-format off
}
