/*
      __        ___.         .__
    _/  |______ \_ |__  __ __|  | _____ _______
    \   __\__  \ | __ \|  |  \  | \__  \\_  __ \
     |  |  / __ \| \_\ \  |  /  |__/ __ \|  | \/
     |__| (____  /___  /____/|____(____  /__|
               \/    \/                \/

    *  Author: Anas Hamdane
    *  Github: https://github.com/Anas-Hamdane

*/

#ifndef TABULAR_ROW_HPP
#define TABULAR_ROW_HPP

#include <tabular/column.hpp>

namespace tabular {
  class Row {

public:
    std::vector<Column> columns;

    Row(std::vector<Column>&& columns)
        : columns(std::move(columns)) {}

    Column& operator[](int index) {
      return this->columns.at(index);
    }
  };
} // namespace tabular

#endif // TABULAR_ROW_HPP
