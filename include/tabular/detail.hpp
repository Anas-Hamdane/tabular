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

#ifndef TABULAR_DETAIL_HPP
#define TABULAR_DETAIL_HPP

#include <climits>
#include <string>
#include <list>

#include <tabular/codec.hpp>
#include <tabular/global.hpp>
#include <tabular/STDS.hpp>

namespace tabular {
  namespace detail {
    namespace string_utils {
      inline size_t display_width(const std::string& str, bool is_multi_byte) {
        return is_multi_byte ? codec::utf8dw(str.c_str()) : str.length();
      }

      // split by words AND save both '\n' and ' ' as words too
      // works at least for UTF-8 encoding strings
      inline std::list<std::string> split_text(const std::string& str) {
        std::list<std::string> result;

        size_t start = 0;
        const size_t len = str.size();

        for (size_t i = 0; i < len; ++i) {
          char ch = str[i];

          if (ch == ' ' || ch == '\n') {
            if (i > start)
              result.emplace_back(str.begin() + start, str.begin() + i);

            result.emplace_back(1, ch); // Push " " or "\n"
            start = i + 1;
          }
        }

        if (start < len)
          result.emplace_back(str.begin() + start, str.end());

        return result;
      }
    } // namespace string_utils

    namespace utils {
      // to align PPDirectives
      // clang-format off
      inline unsigned short get_terminal_width() {
        // first case: defined env var of COLUMNS
        const char* columns_env = std::getenv("COLUMNS");
        if (columns_env != nullptr) {
          try {
            int width_int = std::stoi(columns_env);
            if (width_int > 0 && width_int <= USHRT_MAX)
              return static_cast<unsigned short>(width_int);
          } catch (...) {
          }
        }

        unsigned short width = 0;

        #if defined(UNIX_LIKE)
          struct winsize ws;
          if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1)
            width = 0;
          else
            width = ws.ws_col;
        #elif defined(WINDOWS)
          CONSOLE_SCREEN_BUFFER_INFO csbi;
          if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
            width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        #else
          width = 0;
        #endif
        return width;
      }

      inline bool enable_ansi_support(STD std) {
        #if defined(WINDOWS)
          static bool support_enabled = []() -> bool {
            HANDLE hOut;
            if (std == STD::OUT)
              hOut = GetStdHandle(STD_OUTPUT_HANDLE);
            else
              hOut = GetStdHandle(STD_ERROR_HANDLE)
            if (hOut == INVALID_HANDLE_VALUE) return false;

            DWORD dwMode = 0;
            if (!GetConsoleMode(hOut, &dwMode)) return false;

            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            if (!SetConsoleMode(hOut, dwMode)) return false;

            return true;
          }();
          return support_enabled;
        #else
          return true; // by default Unix like systems has this ability
        #endif
      }
      // clang-format on
    } // namespace utils
  } // namespace detail
} // namespace tabular

#endif // !TABULAR_DETAIL_HPP
