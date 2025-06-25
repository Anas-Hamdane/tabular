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

#ifndef TABULAR_HPP
#define TABULAR_HPP

#include <tabular/detail.hpp>

namespace tabular {
  inline void print(Table& table, const STD& stream = STD::Out) {
    bool multi_byte_characters_flag = false;

    FILE* file_stream = stream == STD::Out ? stdout : stderr;

    const std::string formatted_table =
        detail::printer::format_table(table, table.get().disabled_styles(),
                                      multi_byte_characters_flag, file_stream);

    detail::printer::handle_output(formatted_table, multi_byte_characters_flag, stream);
  }

  inline void print(Table& table, FILE* file) {
    if (!file)
      return;

    bool multi_byte_characters_flag = false;

    std::string formatted_table =
        detail::printer::format_table(table, table.get().disabled_styles(),
                                      multi_byte_characters_flag, file);

    // clang-format off
    #if defined(WINDOWS)
      int fd = _fileno(file);
      HANDLE handle = reinterpret_cast<HANDLE>(_get_osfhandle(fd));
      DWORD written;

      if (handle != INVALID_HANDLE_VALUE) {
        WriteFile(handle, formatted_table.c_str(), formatted_table.length(), &written, nullptr);
        return;
      }
    #elif defined (UNIX_LIKE)
      fwrite(formatted_table.c_str(), 1, formatted_table.length(), file);
      fflush(file);
      return;
    #endif
    // clang-format on

    // fallback
    fprintf(file, "%s", formatted_table.c_str());
  }
} // namespace tabular

#endif // !TABULAR_HPP
