#pragma once

#include "Column.h"

namespace tabular {
  class Row {
    public:
      Row(std::vector<Column> columns);
      Row();

      std::vector<Column> getColumns();
      void setColumns(std::vector<Column> columns);

    private:
      std::vector<Column> columns;
  };
}
