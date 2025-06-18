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

#ifndef TABULAR_COLUMN_LINES_HPP
#define TABULAR_COLUMN_LINES_HPP

#include <string>

namespace tabular {
  struct ColumnLines {
    std::string line;
    size_t display_width;

    ColumnLines(std::string line, size_t width) : line(std::move(line)), display_width(width) {}
  };
} // namespace tabular

#endif // !TABULAR_COLUMN_LINES_HPP
