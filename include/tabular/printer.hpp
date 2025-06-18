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

#ifndef TABULAR_PRINTER_HPP
#define TABULAR_PRINTER_HPP

#include <string.h>
#include <unordered_set>

#include <tabular/border.hpp>
#include <tabular/detail.hpp>
#include <tabular/table.hpp>

namespace tabular {
  namespace detail {
    namespace printer {
      // returns the processed line
      inline std::string process_line(std::string& line, size_t& sub_width, size_t width, Column& column, bool disabled_styles) {
        std::string result;
        Alignment align = column.get().alignment();

        std::string styles;

        // styling
        // "m" to complete the text styles statement
        // mentioned in the Column class.
        styles += column.get().text_attributes();

        if (!styles.empty())
          styles += "m";

        styles += column.get().content_color();
        styles += column.get().content_background_color();

        // for non-tui output streams styles are disabled
        if (disabled_styles)
          styles.clear();

        // 0 if styles are disabled
        column.set().special_characters(styles.empty() ? 0 : styles.size() + strlen(ansi::RESET));

        int start;
        if (align == Alignment::center)
          start = (width - sub_width) / 2;
        else if (align == Alignment::right)
          start = (width - sub_width);
        else
          start = 0;

        size_t line_size = start + styles.size() + line.size() + result.empty() + (styles.empty() ? 0 : strlen(ansi::RESET));

        result.reserve(line_size);

        // auto horizontal padding of 1
        if (result.empty())
          result.append(" ");

        result.append(start, ' ');

        result.append(styles + line);

        if (!styles.empty())
          result.append(ansi::RESET);

        line.clear();
        sub_width = 0;

        return result;
      }

      inline void process_word(size_t width, size_t back_limit,
                               size_t& line_width, size_t word_width,
                               bool multi_byte_characters, bool disabled_styles,
                               std::list<std::string>& words, std::list<std::string>::iterator& it,
                               std::string& line, Column& column,
                               std::vector<std::string>& result) {

        std::string word = *it;
        if (line.empty() && word == " ")
          return;

        // add existing content if we reach new line
        if (word == "\n") {
          result.push_back(
              process_line(line, line_width, width, column, disabled_styles));

          return;
        }

        // adding the word will exceed the width
        if (line_width + word_width > width) {
          /*
           * if the remaining space is larger than the back_limit
           * we should force split to avoid a large empty space in the line
           */
          size_t remaining_space = width - line_width;

          /*
           * Why insert remainder instead of processing directly?
           * The remainder itself might be longer than a full line width,
           * so it may need to be split again in subsequent iterations.
           */
          if (remaining_space > back_limit) {
            std::string first_part;
            std::string remainder;

            size_t first_part_width = 0;

             // ASCII characters are the standard for English
             // so adding a hyphen would be better when wrapping a word
            if (!multi_byte_characters) {
              first_part = word.substr(0, remaining_space - 1) + '-';
              remainder = word.substr(remaining_space - 1);

              first_part_width = string_utils::display_width(first_part, multi_byte_characters);
            }

            // we prefer to not using a hyphen for multi byte characters
            // because some languages like Japanese shouldn't have it.
            else {
              size_t width_limit = remaining_space - 1;

              size_t pos = 0;
              size_t len = 0;
              size_t w = 0;

              char buffer[5];
              while (pos < word.length()) {
                // getting the next utf-8 character
                codec::next_utf8_sequence(word, buffer, pos, len);

                if (w + len > width_limit)
                  break;

                w += codec::utf8dw(buffer);

                // to iterate for the next utf-8 sequence
                pos += len;
              }

              if (pos < word.length()) {
                first_part = word.substr(0, pos);
                remainder = word.substr(pos);
              }

              // should rarely happen
              else {
                first_part = word.substr(0, width_limit);
                remainder = word.substr(width_limit);
              }
            }

            // finally we add the first part to the sub
            // and commit the stuff
            line += first_part;
            line_width += first_part_width;

            result.push_back(
                process_line(line, line_width, width, column, disabled_styles));

            line.clear();
            line_width = 0;

            // insert the second part to ensure it will be
            // well processed in the next iteration
            words.insert(std::next(it), remainder);
          }

          // does not exceed the back_limit
          else {
            if (!line.empty() && line.back() == ' ') {
              line.pop_back(); // pop the last space if exist
              line_width--;
            }

            result.push_back(
                process_line(line, line_width, width, column, disabled_styles));

            line.clear();
            line_width = 0;

            --it;
          }
        }

        // just append the word we are not at the end of the line
        else {
          line += word;
          line_width += word_width;
        }
      }

      inline void format_column(Column& column, uint8_t back_limit_percent, bool disabled_styles) {
        const std::string& content = column.content;
        const bool multi_byte_characters = column.get().multi_byte_characters();

        if (content.empty()) {
          column.set().lines(std::vector<std::string>());
          return;
        }

        unsigned int top_padding = column.get().top_padding();
        unsigned int bottom_padding = column.get().bottom_padding();

        // the return result
        std::vector<std::string> result;

        // split the content into words to easily manipulate it
        auto words = string_utils::split_text(content);

        // TOP padding
        result.insert(result.end(), top_padding, std::string());

        // to avoid empty columns
        if (column.get().width() <= 2)
          column.set().width(3);

        // e.g: MAX sub size POSSIBLE, - 2 for two sides spaces
        const size_t width = (column.get().width() - 2);

        // back limit used to force split a long word
        const size_t back_limit = (width * back_limit_percent) / 100;

        std::string line;
        size_t line_width = 0;
        for (auto it = words.begin(); it != words.end(); ++it) {
          std::string& word = *it;

          // we need split
          size_t word_width = string_utils::display_width(word, multi_byte_characters);

          process_word(width, back_limit,
                       line_width, word_width,
                       multi_byte_characters, disabled_styles,
                       words, it,
                       line, column,
                       result);
        }

        // any remaining words
        if (!line.empty())
          result.push_back(
              process_line(line, line_width, width, column, disabled_styles));

        // BOTTOM padding
        result.insert(result.end(), bottom_padding, std::string());

        column.set().lines(result);
        return;
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
            if (columns_width != 0) {
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

          int indiv_column_width = table_usable_width / columns_num;
          int rest = table_usable_width % columns_num;

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
              const std::string& current_line = lines[i];
              const size_t special_characters = column.get().special_characters();
              const bool multi_byte_characters = column.get().multi_byte_characters();

              // appending the column
              result += column_background_color + current_line;

              size_t current_line_size = string_utils::display_width(current_line, multi_byte_characters);

              rest -= (current_line_size - special_characters);
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
    } // namespace printer
  } // namespace detail

  inline void print(Table& table, std::ostream& stream) {
    // result
    std::string formated_table;

    if (table.rows.empty())
      return;

    if (table.get().width() == 0) {
      unsigned short terminal_width = detail::utils::get_terminal_width();

      // setting the width via the percent
      table.set().width((terminal_width * table.get().width_percent()) / 100);
    }

    if (!detail::utils::enable_ansi_support()) {
      // if it is not a TUI just disable all the styling stuff
      table.set().disabled_styles(true);

      // in case it is not a TTY (for example the output stream is a file)
      // the usable_width will be a very large value (maybe infinity)
      // if it is not set via table.set().width(int_value)
      // so in this case we set a default value for non_tui streams
      if (table.get().width() == 0)
        table.set().width(table.get().non_tui_width());
    }

    detail::printer::adjust_width(table);

    bool disabled_styles = table.get().disabled_styles();
    uint8_t back_limit_percent = table.get().back_limit_percent();

    for (Row& row : table.rows) {
      for (Column& column : row.columns) {
        detail::printer::format_column(column, back_limit_percent, disabled_styles);
      }
    }

    // border parts
    BorderGlyphs glyphs = table.border().get().processed_glyphs();

    /* Starting printing */
    const auto& rows = table.rows;

    bool is_first = true, is_last = (rows.size() == 1) ? true : false;
    bool separated_rows = table.get().separated_rows();
    size_t width = table.get().width();

    auto it = rows.begin();
    formated_table += detail::printer::print_border(it, glyphs, is_first, is_last, width);

    is_first = false;
    for (it = rows.begin(); it != rows.end(); ++it) {
      const Row& row = *it;

      formated_table += detail::printer::print_row(row, glyphs, width);

      if ((it + 1) == rows.end())
        is_last = true;

      if (separated_rows)
        formated_table += detail::printer::print_border(it, glyphs, is_first, is_last, width);
    }

    stream << formated_table;
  }

  inline std::ostream& operator<<(std::ostream& stream, const Table& table) {
    print(const_cast<Table&>(table), stream);
    return stream;
  }
} // namespace tabular

#endif // !TABULAR_PRINTER_HPP
