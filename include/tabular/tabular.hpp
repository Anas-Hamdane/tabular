/*
      __        ___.         .__
    _/  |______ \_ |__  __ __|  | _____ _______
    \   __\__  \ | __ \|  |  \  | \__  \\_  __ \
     |  |  / __ \| \_\ \  |  /  |__/ __ \|  | \/
     |__| (____  /___  /____/|____(____  /__|
               \/    \/                \/

    *  Author: Anas Hamdane
    *  Github: https://github.com/Anas-Hamdane
    *  LICENSE: https://github.com/Anas-Hamdane/tabular/blob/main/LICENSE
*/

#ifndef TABULAR_HPP
#define TABULAR_HPP

#include <tabular/detail.hpp>

namespace tabular {
  inline bool print(Table& table, const STD& stream = STD::Out) {
    FILE* file_stream = stream == STD::Out ? stdout : stderr;

    const std::string formatted_table = detail::printer::format_table(table, file_stream);
    return detail::printer::render_table(formatted_table, table.get().multi_byte_characters(), file_stream);
  }

  inline bool print(Table& table, FILE* file) {
    if (!file)
      return false;

    const std::string formatted_table = detail::printer::format_table(table, file);
    return detail::printer::render_table(formatted_table, table.get().multi_byte_characters(), file);
  }
} // namespace tabular

#endif // !TABULAR_HPP
