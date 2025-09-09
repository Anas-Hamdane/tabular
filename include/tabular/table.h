#pragma once

#include "Row.h"

namespace tabular {
  class Table {
    public:
      Table(std::vector<Row> rows);
      Table();

      std::vector<Row> getRows();
      void setRows(std::vector<Row> rows);

    private:
      std::vector<Row> rows;
  };
}
