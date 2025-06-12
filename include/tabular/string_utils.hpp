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

#ifndef TABULAR_STRING_UTILS_HPP
#define TABULAR_STRING_UTILS_HPP

#include <list>
#include <string>
#include <utf8conv/utf8conv.hpp>
#include <wcwidth/wcwidth.hpp>

namespace tabular {
  namespace string_utils {
    // multi byte string width
    inline size_t mbswidth(std::string str) {
      std::wstring wstr = conv::utf8stws(str.c_str());
      size_t width = display_width::wcswidth(wstr.c_str(), wstr.length());

      return width;
    }

    inline size_t display_width(std::string str, bool is_multi_byte) {
      return is_multi_byte ? mbswidth(str) : str.length();
    }

    inline bool is_continuation_byte(unsigned char byte) {
      return (byte & 0xC0) == 0x80; // 10xxxxxx
    }

    inline std::size_t safe_split_pos(const std::string& s, std::size_t pos) {
      if (pos > s.size()) pos = s.size();
      while (pos > 0 && is_continuation_byte(static_cast<unsigned char>(s[pos]))) {
        --pos;
      }
      return pos;
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
} // namespace tabular

#endif // !TABULAR_STRING_UTILS_HPP
