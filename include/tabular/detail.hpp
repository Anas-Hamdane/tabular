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
#include <list>
#include <string.h>
#include <string>
#include <unordered_set>

#include <tabular/codec.hpp>
#include <tabular/global.hpp>
#include <tabular/stds.hpp>
#include <tabular/table.hpp>

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
      inline unsigned short get_terminal_width(FILE* stream) {
        // in case of stdout we could check the environnement variable
        if (stream == stdout) {
          const char* columns_env = std::getenv("COLUMNS");
          if (columns_env != nullptr) {
            try {
              int width_int = std::stoi(columns_env);
              if (width_int > 0 && width_int <= USHRT_MAX)
                return static_cast<unsigned short>(width_int);
            } catch (...) {}
          }
        }

        unsigned short width = 0;

        #if defined(UNIX_LIKE)
          int fd = fileno(stream);
          if (!isatty(fd)) return 0; // Note a terminal

          struct winsize ws;
          if (ioctl(fd, TIOCGWINSZ, &ws) == 0)
            width = ws.ws_col;

        #elif defined(WINDOWS)
          int fd = _fileno(stream);
          if (!_isatty(fd)) return 0; // Not a terminal

          HANDLE hConsole = reinterpret_cast<HANDLE>(_get_osfhandle(fd)); 
          CONSOLE_SCREEN_BUFFER_INFO csbi;

          if (hConsole != INVALID_HANDLE_VALUE && GetConsoleScreenBufferInfo(hConsole, &csbi))
            width = csbi.srWindow.Right - csbi.srWindow.Left + 1;

        #endif
        return width;
      }

      /*
       * return value:
       *  1: when everything is fine (success)
       *  2: can't enable VTP for windows 
       *  3: `stream` is not a valid tty
       */
      inline int enable_ansi_support(FILE* stream) {
        /*
         * for windows we should enable VTP mode
         * to ensure that ansi escape sequences
         * will be displayed correctly.
         */
        #if defined(WINDOWS)
          int fd = _fileno(stream);
          if (!_isatty(fd))
            return 3;
      
          HANDLE hOut = reinterpret_cast<HANDLE>(_get_osfhandle(fd));
          if (hOut == INVALID_HANDLE_VALUE)
            return 2;
      
          DWORD dwMode = 0;
          if (!GetConsoleMode(hOut, &dwMode))
            return 2;
      
          dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
          if (!SetConsoleMode(hOut, dwMode))
            return 2;
      
          return 1;
      
        /*
         * In Unix-like systems most consoles support ansi escape
         * sequences by default.
         */
        #else
          if (!isatty(fileno(stream)))
            return 3;
      
          return 1;
        #endif
      }
      // clang-format on
    } // namespace utils

    namespace printer {
      // returns the processed line
      inline void append_column_line(std::string& line, size_t& line_width,
                                     size_t max_width, Column& column,
                                     std::vector<ColumnLines>& result,
                                     bool disabled_styles) {
        std::string formatted_line;
        Alignment align = column.get().alignment();

        std::string styles;

        if (!disabled_styles) {
          // `suffix` to complete the text styles statement.
          styles.append(column.get().text_attributes());

          if (!styles.empty()) styles.push_back(ansi::suffix);

          styles.append(column.get().content_color());
          styles.append(column.get().content_background_color());
        }

        // Adjusting Alignment
        int start;
        if (align == Alignment::center)
          start = (max_width - line_width) / 2;
        else if (align == Alignment::right)
          start = (max_width - line_width);
        else
          start = 0;

        // calculating line size
        size_t line_len = start + styles.size() +
                          line.size() + (styles.empty() ? 0 : strlen(ansi::RESET));
        formatted_line.reserve(line_len);

        // auto horizontal padding of 1
        formatted_line = " ";

        // alignment
        formatted_line.append(start, ' ');

        // counting spaces
        line_width += start + 1;

        // appending the line with styles
        formatted_line.append(styles + line);
        if (!styles.empty())
          formatted_line.append(ansi::RESET);

        result.emplace_back(formatted_line, line_width);

        line.clear();
        line_width = 0;
      }

      inline void process_word(const std::string& word, size_t& word_width,
                               std::string& line, size_t& line_width,
                               size_t max_width, size_t back_limit,
                               std::vector<ColumnLines>& result, Column& column,
                               bool disabled_styles) {
        bool multi_byte_characters = column.get().multi_byte_characters();

        if (line.empty() && word == " ")
          return;

        // add existing content if we reach new line
        if (word == "\n") {
          append_column_line(line, line_width, max_width, column, result, disabled_styles);
          return;
        }

        // normal case: append the word
        if ((line_width + word_width) <= max_width) {
          line += word;
          line_width += word_width;

          return;
        }

        /*
         * if we reach here, that means (line_width + word_width) > width
         * we should append the line after processing the last word.
         */

        // space left
        size_t remaining_space = max_width - line_width;

        // append the line, and leave the current word for the next line,
        // since the remaining_space doesn't exceed the back_limit
        if (remaining_space <= back_limit) {
          if (!line.empty() && line.back() == ' ') {
            line.pop_back(); // pop the last space if exist
            line_width--;
          }

          append_column_line(line, line_width, max_width, column, result, disabled_styles);

          process_word(word, word_width, line, line_width,
                       max_width, back_limit, result, column, disabled_styles);

          return;
        }

        /*
         * In this case we will be splitting the word into two parts.
         * the first parts should fill int this line, with a hyphen
         * ONLY if multi_byte_characters = false, otherwise we will not,
         * since other languages like Japanese doesn't need (actually they shouldn't have)
         * a hyphen at the end of a splitted word.
         *
         * The hardest part is handling multi-byte strings splitting.
         */

        // word parts
        std::string first_part;
        std::string remainder;

        size_t first_part_width = 0;

        // no multi-byte strings, basic case
        if (!multi_byte_characters) {
          // -1 for the hyphen
          first_part = word.substr(0, remaining_space - 1) + '-';
          remainder = word.substr(remaining_space - 1);

          first_part_width = first_part.length();
        }

        else {
          // initialize variables
          size_t pos = 0, len = 0, w = 0;

          char buffer[5];
          while (pos < word.length()) {
            // getting the next utf-8 sequence
            codec::next_utf8_sequence(word, buffer, pos, len);

            // don't exceed the remaining space
            if (pos + len > remaining_space)
              break;

            // otherwise increase the width w
            w += codec::utf8dw(buffer);

            // to iterate for the next utf-8 sequence
            pos += len;
          }

          // we found the right position to split
          if (pos < word.length()) {
            first_part = word.substr(0, pos);
            remainder = word.substr(pos);
          }

          // in case something goes wrong fallback into
          // using remaining_space, nothing will break
          // but the layout may have some issues.
          else {
            first_part = word.substr(0, remaining_space);
            remainder = word.substr(remaining_space);
          }

          // in all cases w is positive
          first_part_width = w;
        }

        // append the first part and his width
        line += first_part;
        line_width += first_part_width;

        append_column_line(line, line_width, max_width, column, result, disabled_styles);

        size_t remainder_width = word_width - first_part_width + ((multi_byte_characters) ? 0 : 1);
        process_word(remainder, remainder_width, line, line_width,
                     max_width, back_limit, result, column, disabled_styles);
      }

      inline void format_column(Column& column, uint8_t back_limit_percent,
                                bool disabled_styles, bool& multi_byte_characters_flag) {
        const std::string& content = column.content;
        const bool multi_byte_characters = column.get().multi_byte_characters();

        if (multi_byte_characters && !multi_byte_characters_flag)
          multi_byte_characters_flag = true;

        if (content.empty()) {
          column.set().lines(std::vector<ColumnLines>());
          return;
        }

        unsigned int top_padding = column.get().top_padding();
        unsigned int bottom_padding = column.get().bottom_padding();

        // to avoid empty columns
        if (column.get().width() <= 2)
          column.set().width(3);

        // MAX line width POSSIBLE, - 2 for two sides spaces
        const size_t max_width = (column.get().width() - 2);

        // the return result
        std::vector<ColumnLines> result;
        result.reserve(content.length() / max_width);

        // split the content into words to easily manipulate it
        auto words = string_utils::split_text(content);

        // TOP padding
        result.insert(result.end(), top_padding, ColumnLines("", 0));

        // space we shouldn't exceed if we decided to leave the current word
        // for the next line, meaning if the remaining_space > back_limit
        // we should FORCE CUT the word, else we can just append this line
        // and leave that word for the next line
        const size_t back_limit = (max_width * back_limit_percent) / 100;

        std::string line;
        size_t line_width = 0;
        for (auto it = words.begin(); it != words.end(); ++it) {
          const std::string& word = *it;

          // we need split
          size_t word_width = string_utils::display_width(word, multi_byte_characters);

          process_word(word, word_width, line, line_width,
                       max_width, back_limit, result, column, disabled_styles);
        }

        // any remaining words
        if (!line.empty())
          append_column_line(line, line_width, max_width, column, result, disabled_styles);

        // BOTTOM padding
        result.insert(result.end(), bottom_padding, ColumnLines("", 0));

        column.set().lines(result);
      }

      // return the size of the tallest splitted_content vector
      inline size_t tallest_cell(const Row& row) {
        size_t result = 0;
        for (const Column& column : row.columns) {
          size_t splitted_content_size = column.get().lines().size();
          result = std::max(result, splitted_content_size);
        }

        return result;
      }

      inline void adjust_width(Table& table) {
        unsigned int table_width = table.get().width();

        if (table_width <= 0)
          return;

        for (size_t i = 0; i < table.rows.size(); i++) {
          Row& row = table.rows[i];

          size_t columns_num = row.columns.size();
          if (columns_num == 0)
            continue;

          // minus table splits
          int table_usable_width = table_width - (columns_num + 1);
          int columns_width = 0;

          // when inconsistent widths are given we will force
          // overriding their original width
          bool forced = false;

          for (Column& column : row.columns) {
            unsigned int col_width = column.get().width();
            if (col_width != 0) {
              table_usable_width -= col_width;
              columns_width += col_width;
              columns_num--;
            }
          }

          // At this point, all column widths for the current row are known.
          if (columns_num == 0 && i == 0) {
            // First row: initialize table width to fit all columns plus separators.
            // make sure to update table_width too ;)
            table_width = columns_width + row.columns.size() + 1;
            table.set().width(table_width);
            continue;
          }

          // One of two issues:
          // Not the first row and has a full width initialization
          // We've consumed all usable width and there are still more columns
          else if ((columns_num == 0 && table_usable_width != 0) ||
                   (columns_num != 0 && table_usable_width <= 0)) {

            // Restore full width and update columns count for recalculating layout.
            table_usable_width += columns_width;
            columns_num = row.columns.size();

            // force overriding the original width
            forced = true;
          }

          if (columns_num == 0 && !forced)
            continue;

          int indiv_column_width = 0;
          int rest = 0;
          if (columns_num != 0) {
            indiv_column_width = table_usable_width / columns_num;
            rest = table_usable_width % columns_num;
          }

          for (Column& column : row.columns) {
            unsigned int width = column.get().width();

            if (width == 0 || forced) {
              width = indiv_column_width + (rest > 0 ? 1 : 0);
              if (rest > 0) rest--;
              column.set().width(width);
            }
          }
        }
      }

      inline std::unordered_set<size_t> separators_positions(const Row& row) {
        std::unordered_set<size_t> result;
        result.reserve(row.columns.size() - 1);

        size_t track = 1;
        for (const Column& column : row.columns) {
          track += column.get().width();
          result.insert(track);
          track++;
        }

        return result;
      }

      inline void resolve_border(const std::string*& left_corner, const std::string*& right_corner,
                                 const std::string*& intersection, const std::string*& top_connector,
                                 const std::string*& bottom_connector, const BorderGlyphs& glyphs,
                                 bool first, bool last) {

        if (first) {
          left_corner = &glyphs.top_left_corner;
          right_corner = &glyphs.top_right_corner;
          intersection = &glyphs.top_connector;
          top_connector = &glyphs.top_connector;
          bottom_connector = &glyphs.top_connector;
        }

        else if (last) {
          left_corner = &glyphs.bottom_left_corner;
          right_corner = &glyphs.bottom_right_corner;
          intersection = &glyphs.bottom_connector;
          top_connector = &glyphs.bottom_connector;
          bottom_connector = &glyphs.bottom_connector;
        }

        else {
          left_corner = &glyphs.left_connector;
          right_corner = &glyphs.right_connector;
          intersection = &glyphs.intersection;
          top_connector = &glyphs.top_connector;
          bottom_connector = &glyphs.bottom_connector;
        }
      }

      inline std::string print_border(std::vector<Row>::const_iterator& it,
                                      const BorderGlyphs& glyphs,
                                      bool first, bool last,
                                      size_t width) {
        // result
        std::string border;
        border.reserve(width);

        const Row& reference = *it;

        std::unordered_set<size_t> separators;
        // first row separators
        if (first)
          separators = separators_positions(reference);

        // next row separators
        else if (!last)
          separators = separators_positions(*(it + 1));

        if (!first)
          border.push_back('\n');

        const std::string& horizontal = glyphs.horizontal;

        const std::string* left_corner;
        const std::string* right_corner;
        const std::string* intersection;
        const std::string* top_connector;
        const std::string* bottom_connector;

        resolve_border(left_corner, right_corner,
                       intersection, top_connector,
                       bottom_connector, glyphs, first, last);

        border.append(*left_corner);

        const size_t columns_number = reference.columns.size();
        size_t tracker = 0;

        for (size_t j = 0; j < columns_number; ++j) {
          const Column& column = reference.columns[j];
          const unsigned int column_width = column.get().width();

          for (unsigned int k = 0; k < column_width; ++k) {
            ++tracker;
            border += separators.count(tracker) ? *top_connector : horizontal;
          }
          ++tracker;

          if (j + 1 != columns_number)
            border += separators.count(tracker) ? *intersection : *bottom_connector;
        }

        border += *right_corner;
        return border;
      }

      inline std::string print_row(const Row& row, const BorderGlyphs& glyphs, size_t width) {
        std::string result;

        const std::string& vertical = glyphs.vertical;
        const std::string& reset = ansi::RESET;
        const size_t tallest_cell_size = tallest_cell(row);

        result.reserve(width * tallest_cell_size);

        for (size_t i = 0; i < tallest_cell_size; ++i) {
          result += '\n' + vertical;

          for (const Column& column : row.columns) {
            const auto& lines = column.get().lines();
            const size_t lines_size = lines.size();
            const std::string column_background_color =
                column.get().disabled_styles() ? "" : column.get().column_background_color();

            size_t rest = column.get().width();

            if (lines_size > i) {
              const std::string& current_line = lines[i].line;
              const size_t current_line_size = lines[i].display_width;
              const bool multi_byte_characters = column.get().multi_byte_characters();

              // appending the column
              result += column_background_color + current_line;

              rest -= current_line_size;
            }

            result += column_background_color;
            result.append(rest, ' ');

            if (!column_background_color.empty())
              result += reset;

            result += vertical;
          }
        }

        return result;
      }

      inline void handle_output(const std::string& formatted_table,
                                bool multi_byte_characters_flag, STD stream) {
        // clang-format off
        #if defined(WINDOWS)
          HANDLE handle;
          if (std == STD::Out)
            handle = GetStdHandle(STD_OUTPUT_HANDLE);
          else
            handle = GetStdHandle(STD_ERROR_HANDLE);

          DWORD mode;
          DWORD written;
          bool is_console_output = GetConsoleMode(handle, &mode);

          if (!is_console_output) {
            // Output is redirected: write raw UTF-8 bytes
            WriteFile(handle, formatted_table.c_str(),
                formatted_table.length(), &written, nullptr);
            return;
          }

          if (multi_byte_characters_flag) {
            // Convert UTF-8 to UTF-16 for proper console rendering
            int wlen = MultiByteToWideChar(CP_UTF8, 0,
                formatted_table.c_str(), -1, nullptr, 0);
            if (wlen <= 0) {
              WriteFile(handle, formatted_table.c_str(),
                  formatted_table.length(), &written, nullptr);
              return;
            }

            std::wstring wstr(wlen - 1, L'\0'); // exclude null terminator
            int result = MultiByteToWideChar(CP_UTF8, 0, formatted_table.c_str(),
                -1, &wstr[0], wlen);
            if (result <= 0) {
              WriteFile(handle, formatted_table.c_str(), formatted_table.length(),
                  &written, nullptr);
              return;
            }

            WriteConsoleW(handle, wstr.c_str(), static_cast<DWORD>(wstr.length()),
                &written, nullptr);
          } else {
            // ASCII or simple UTF-8 output
            WriteConsoleA(handle, formatted_table.c_str(),
                static_cast<DWORD>(formatted_table.length()), &written, nullptr);
          }
        #else
          FILE* out_stream = stream == STD::Out ? stdout : stderr;
          fwrite(formatted_table.c_str(), 1, formatted_table.length(), out_stream);
          fflush(out_stream);
        #endif
        // clang-format on
      }

      inline std::string format_table(Table& table, bool disabled_styles,
                                      bool& multi_byte_characters_flag, FILE* stream) {
        if (table.rows.empty())
          return "";

        // result
        std::string formatted_table;

        if (table.get().width() == 0) {
          unsigned short terminal_width = detail::utils::get_terminal_width(stream);

          // setting the width via the percent
          if (terminal_width != 0)
            table.set().width((terminal_width * table.get().width_percent()) / 100);
        }

        // return code
        int rc = detail::utils::enable_ansi_support(stream);

        /*
         * In case enabling ansi escape sequences fail, or the `stream`
         * is not a valid TTY disable the styles to prevent printing raw
         * ansi escape sequences
         */
        if (rc != 1) {
          // if something goes wrong disable the styles
          // to prevent printing raw ansi escape sequences
          disabled_styles = true;

          // in case it is not a TTY (for example the output stream is a file)
          // we will be applying the non_tui_width to prevent infinity loops
          // due to the large value of table.width because non-tty streams
          // most of the times, don't have a limited columns number.
          if (rc == 3)
            table.set().width(table.get().non_tty_width());
        }

        detail::printer::adjust_width(table);

        uint8_t back_limit_percent = table.get().back_limit_percent();

        for (Row& row : table.rows) {
          for (Column& column : row.columns) {
            detail::printer::format_column(column, back_limit_percent,
                                           disabled_styles, multi_byte_characters_flag);
          }
        }

        const BorderStyle old_style = table.border().get().style();

        if (disabled_styles && old_style == BorderStyle::ansi)
          table.border().set().style(BorderStyle::standard);

        // border parts
        BorderGlyphs glyphs = table.border().get().processed_glyphs();

        if (table.border().get().style() != old_style)
          table.border().set().style(old_style);

        /* Starting printing */
        const auto& rows = table.rows;

        bool is_first = true, is_last = (rows.size() == 1) ? true : false;
        bool separated_rows = table.get().separated_rows();
        size_t width = table.get().width();

        auto it = rows.begin();
        formatted_table += detail::printer::print_border(it, glyphs, is_first, is_last, width);

        is_first = false;
        for (it = rows.begin(); it != rows.end(); ++it) {
          const Row& row = *it;

          formatted_table += detail::printer::print_row(row, glyphs, width);

          if ((it + 1) == rows.end())
            is_last = true;

          if (separated_rows)
            formatted_table += detail::printer::print_border(it, glyphs,
                                                             is_first, is_last, width);
        }

        // appending last new line
        formatted_table.push_back('\n');

        return formatted_table;
      }
    } // namespace printer
  } // namespace detail
} // namespace tabular

#endif // !TABULAR_DETAIL_HPP
