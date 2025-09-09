#pragma once

#include "row.h"
#include "border.h"

namespace tabular {
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
    : rows_(std::move(rows)), border(std::move(border)) {}

  std::vector<Row> rows();
  void rows(std::vector<Row> rows);

  Config& config() { return this->config_; }
  const Config& config() const { return this->config_; }

  std::string toStr() const
  {
    adjustWidth();
    // and put them in a std::string
  }

private:
  mutable std::vector<Row> rows_ = {};
  Config config_ = {};
  Border border = {};

  void adjustWidth() const
  {
    if (this->rows_.empty()) return;
    const size_t width = config().width();

    for (auto& row : this->rows_)
    {
      row.config().width(width);
    }
  }
};
}
