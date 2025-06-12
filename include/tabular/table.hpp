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

/*
    TODO:
      -  [x] implement Dynamic table logic
      -  [x] default padding
      -  [x] ANSI Support
      -  [x] Alignment support
      -  [x] terminal colors and 16 colors support
      -  [x] terminal colors rgb support
      -  [x] terminal font styles support
      -  [x] padding control
      -  [x] width control
      -  [x] range columns setters (functions)
      -  [x] support for multi byte characters (automatic and manual)
      -  [x] full column background color support
      -  [x] full border styling control
*/

#include <algorithm>
#include <ostream>
#include <vector>

// this include all headers
#include <tabular/border.hpp>
#include <tabular/utils.hpp>

namespace tabular {
  class Table {
    Border table_border;
    unsigned int width;
    bool forced_width;
    bool force_ansi;

    float width_percent;

    std::vector<Row> rows;

    class Config {
      Table& table;

  public:
      Config(Table& table) : table(table) {}

      /* -----------------ALIGNMENT--------------------- */
      Config& alignment(Alignment align) {
        for (Row& row : table.rows)
          row.config().alignment(align);

        return *this;
      }

      /* -----------------PADDING--------------------- */
      Config& padding(int pad) {
        if (pad < 0)
          return *this; // Invalid padding

        for (Row& row : table.rows)
          row.config().padding(pad);

        return *this;
      }

      /* -----------------TOP_PADDING--------------------- */
      Config& top_padding(int pad) {
        if (pad < 0)
          return *this; // Invalid padding

        for (Row& row : table.rows)
          row.config().top_padding(pad);

        return *this;
      }

      /* -----------------BOTTOM_PADDING--------------------- */
      Config& bottom_padding(int pad) {
        if (pad < 0)
          return *this; // Invalid padding

        for (Row& row : table.rows)
          row.config().bottom_padding(pad);

        return *this;
      }

      /* -----------------WIDTH--------------------- */
      Config& table_width(int width) {
        if (width <= 0)
          return *this; // Invalid width

        table.width = width;

        return *this;
      }

      /* -------------------COLORS------------------------- */
      Config& color(Color color) {
        for (Row& row : table.rows)
          row.config().color(color);

        return *this;
      }

      Config& color(RGB rgb) {
        for (Row& row : table.rows)
          row.config().color(rgb);

        return *this;
      }

      /* ---------------BACKGROUND COLORS--------------------- */
      Config& content_background_color(Color back_color) {
        for (Row& row : table.rows)
          row.config().content_background_color(back_color);

        return *this;
      }

      Config& content_background_color(RGB back_rgb) {
        for (Row& row : table.rows)
          row.config().content_background_color(back_rgb);

        return *this;
      }

      Config& column_background_color(Color back_color) {
        for (Row& row : table.rows)
          row.config().column_background_color(back_color);

        return *this;
      }

      Config& column_background_color(RGB back_rgb) {
        for (Row& row : table.rows)
          row.config().column_background_color(back_rgb);

        return *this;
      }
    };

    class Setters {
      Table& table;

  public:
      Setters(Table& table) : table(table) {}

      Setters& width(int width) {
        if (width > 0)
          table.width = static_cast<unsigned int>(width);
        else
          table.width = 0;

        return *this;
      }

      // * for testing set a fixed, forced width
      Setters& forced_width(int width, bool forced) {
        if (width < 0)
          table.width = 0;
        else
          table.width = static_cast<unsigned int>(width);

        table.forced_width = forced;

        return *this;
      }

      // * for testing force using ansi (used when redirecting output to files)
      Setters& forced_ansi(bool forced) {
        table.table_border.set().style(BorderStyle::ANSI);

        table.force_ansi = forced;

        return *this;
      }

      Setters& multi_byte_characters(bool is_multi_byte) {
        for (Row& row : table.rows)
          row.set().multi_byte_characters(is_multi_byte);

        return *this;
        ;
      }

      Setters& width_percent(float percent) {
        if (percent <= 1 && percent > 0)
          table.width_percent = percent;
          
        return *this;
      }
    };

    class Getters {
      Table& table;

      public:
      Getters(Table& table) : table(table) {}

      std::vector<Row> rows() { return table.rows; }

      unsigned int width() { return table.width; }
    };

    class Columns {
      std::vector<Row>& rows;
      int index;

      class Config {
        std::vector<Row>& rows;
        int index;

    public:
        Config(std::vector<Row>& rows, int index) : rows(rows), index(index) {}

        Config& alignment(Alignment align) {
          for (Row& row : rows)
            if (index >= 0 && index < static_cast<int>(row.columns.size()))
              row.columns[index].config().alignment(align);

          return *this;
        }

        Config& width(int w) {
          unsigned int safe_width = (w <= 0) ? 0 : static_cast<unsigned int>(w);

          for (Row& row : rows)
            if (index >= 0 && index < static_cast<int>(row.columns.size()))
              row.columns[index].config().width(safe_width);

          return *this;
        }

        Config& padding(int padd) {
          unsigned int safe_padding = (padd <= 0) ? 0 : static_cast<unsigned int>(padd);

          for (Row& row : rows) {
            if (index >= 0 && index < static_cast<int>(row.columns.size())) {
              row.columns[index].config().top_padding(safe_padding);
              row.columns[index].config().bottom_padding(safe_padding);
            }
          }

          return *this;
        }

        Config& top_padding(int padd) {
          unsigned int safe_padding = (padd <= 0) ? 0 : static_cast<unsigned int>(padd);

          for (Row& row : rows)
            if (index >= 0 && index < static_cast<int>(row.columns.size()))
              row.columns[index].config().top_padding(safe_padding);

          return *this;
        }

        Config& bottom_padding(int padd) {
          unsigned int safe_padding = (padd <= 0) ? 0 : static_cast<unsigned int>(padd);

          for (Row& row : rows)
            if (index >= 0 && index < static_cast<int>(row.columns.size()))
              row.columns[index].config().bottom_padding(safe_padding);

          return *this;
        }

        Config& text_style(Style style) {
          for (Row& row : rows)
            if (index >= 0 && index < static_cast<int>(row.columns.size()))
              row.columns[index].config().text_style(style);

          return *this;
        }

        Config& text_style(const std::vector<Style>& styles) {
          for (Row& row : rows)
            if (index >= 0 && index < static_cast<int>(row.columns.size()))
              row.columns[index].config().text_style(styles);

          return *this;
        }

        Config& color(Color color) {
          for (Row& row : rows)
            if (index >= 0 && index < static_cast<int>(row.columns.size()))
              row.columns[index].config().color(color);

          return *this;
        }

        Config& color(RGB rgb) {
          for (Row& row : rows)
            if (index >= 0 && index < static_cast<int>(row.columns.size()))
              row.columns[index].config().color(rgb);

          return *this;
        }

        Config& content_background_color(Color color) {
          for (Row& row : rows)
            if (index >= 0 && index < static_cast<int>(row.columns.size()))
              row.columns[index].config().content_background_color(color);

          return *this;
        }

        Config& content_background_color(RGB rgb) {
          for (Row& row : rows)
            if (index >= 0 && index < static_cast<int>(row.columns.size()))
              row.columns[index].config().content_background_color(rgb);

          return *this;
        }

        Config& column_background_color(Color color) {
          for (Row& row : rows)
            if (index >= 0 && index < static_cast<int>(row.columns.size()))
              row.columns[index].config().column_background_color(color);

          return *this;
        }

        Config& column_background_color(RGB rgb) {
          for (Row& row : rows)
            if (index >= 0 && index < static_cast<int>(row.columns.size()))
              row.columns[index].config().column_background_color(rgb);

          return *this;
        }
      };

  public:
      Columns(std::vector<Row>& rows, int index) : rows(rows), index(index) {
      }

      Config config() { return Config(rows, index); }
    };

    std::vector<int> find_stops(Row row) {
      std::vector<int> result;

      int track = 1;
      for (Column column : row.columns) {
        track += column.get().width();

        result.push_back(int(track));
        track++;
      }

      return result;
    }

    void print_row(std::ostream& stream, Row& row, BorderGlyphs glyphs) {
      std::string vertical = glyphs.vertical;

      size_t max_splitted_content_size = utils::find_max_splitted_content_size(row); // tallest vector of splitted strings
      for (unsigned int i = 0; i < max_splitted_content_size; i++) {
        // side border
        stream << '\n'
               << vertical;

        // printing the n element of splitted_content for each column
        for (Column column : row.columns) {
          std::string reset = RESET;

          int rest = column.get().width();
          int splitted_content_size = column.get().splitted_content().size();
          std::string current_line;

          // column background
          std::string column_background_color = column.get().column_background_color();
          stream << column_background_color;

          if (i < splitted_content_size) {
            current_line = column.get().splitted_content().at(i);
            stream << current_line;

            std::string styles = column.get().text_styles() + column.get().content_color() + column.get().content_background_color();
            int special_characters = 0;

            // for each splitted_content element has a one or more global_styles, it has one RESET at the end
            if (!(styles.empty() || current_line.empty())) {
              special_characters += reset.size();
              special_characters += styles.size();
            }

            int curr_line_size = string_utils::display_width(current_line, column.get().is_multi_byte());

            // special_characters will not be displayed so they are not counted
            rest -= (curr_line_size - special_characters); // to balance the line
          }

          stream << column_background_color;

          for (int k = 0; k < rest; k++)
            stream << ' ';

          if (!column_background_color.empty())
            stream << reset;

          stream << vertical;
        }
      }
    }

    void print_border(std::ostream& stream, std::vector<Row>::iterator& it, BorderGlyphs border_glyphs, bool is_first, bool is_last, bool regular) {

      Row reference = *it;
      std::vector<int> next_row_corners;

      if (is_first)
        next_row_corners = find_stops(*it);
      else if (!is_last)
        next_row_corners = find_stops(*(it + 1));

      if (!is_first)
        stream << '\n';

      std::string horizontal = border_glyphs.horizontal;

      // (vertical separators)/corners
      std::string left_corner;
      std::string right_corner;

      // horizontal separator
      std::string middle_separator;
      std::string top_to_bottom;
      std::string bottom_to_top;

      if (is_first) {
        left_corner = border_glyphs.top_left_corner;
        right_corner = border_glyphs.top_right_corner;

        middle_separator = border_glyphs.top_connector;
        top_to_bottom = border_glyphs.top_connector;
        bottom_to_top = border_glyphs.top_connector;
      } else if (is_last) {
        left_corner = border_glyphs.bottom_left_corner;
        right_corner = border_glyphs.bottom_right_corner;

        middle_separator = border_glyphs.bottom_connector;
        top_to_bottom = border_glyphs.bottom_connector;
        bottom_to_top = border_glyphs.bottom_connector;
      } else {
        left_corner = border_glyphs.left_connector;
        right_corner = border_glyphs.right_connector;

        middle_separator = border_glyphs.intersection;
        top_to_bottom = border_glyphs.top_connector;
        bottom_to_top = border_glyphs.bottom_connector;
      }

      stream << left_corner;

      size_t cols_num = reference.columns.size();

      int tracker = 0;
      for (size_t j = 0; j < cols_num; j++) {
        Column column = reference.columns[j];
        unsigned col_width = column.get().width();

        for (unsigned int k = 0; k < col_width; k++) {
          tracker++;

          if (std::find(next_row_corners.begin(), next_row_corners.end(), tracker) != next_row_corners.end())
            stream << top_to_bottom;
          else
            stream << horizontal;
        }

        tracker++;

        if (j + 1 != cols_num) {

          // column separator at the bottom and at the top
          if (std::find(next_row_corners.begin(), next_row_corners.end(), tracker) != next_row_corners.end())
            stream << middle_separator;
          else
            stream << bottom_to_top; // just at the top
        }
      }

      stream << right_corner;
    }

    /* Regularity means it has the same number of columns in each row */
    bool is_regular() {
      size_t reference = rows[0].columns.size();
      size_t rows_num = rows.size();

      for (int i = 0; i < rows_num; i++)
        if (rows[i].columns.size() != reference)
          return false;

      return true;
    }

    inline void adjust_widths(unsigned int table_width) {
      if (table_width <= 0)
        return;

      for (Row& row : this->rows) {
        size_t columns_num = row.columns.size();
        if (columns_num == 0)
          continue;

        unsigned int table_usable_width = table_width - (columns_num + 1);

        for (Column& column : row.columns) {
          if (column.get().width() != 0) {
            table_usable_width -= column.get().width();
            columns_num--;
          }
        }

        int indiv_column_width = table_usable_width / columns_num;
        int rest = table_usable_width % columns_num;

        for (Column& column : row.columns) {
          unsigned int width = column.get().width();

          if (width == 0) {
            width = indiv_column_width + (rest > 0 ? 1 : 0);
            if (rest > 0) rest--;
            column.set().width(width);
          }

          /* no need because it is the work of format_row() :) */
          // column.set().splitted_content(utils::format_column_lines(column, width));
        }
      }
    }

public:
    Table() : width(0), forced_width(false), width_percent(0.5) {}

    // configure the table
    Config config() { return Config(*this); }

    Border& border() { return table_border; }

    Setters set() { return Setters(*this); }

    Getters get() { return Getters(*this); }

    Columns column(int index) {
      return Columns(rows, index);
    }

    Row& row(int index) {
      return rows.at(index); // Throws std::out_of_range if index is invalid
    }

    Table& add_row(std::vector<std::string> contents) {
      std::vector<Column> columns;
      for (std::string content : contents)
        columns.push_back(Column(content));

      rows.push_back(Row(columns));

      return *this;
    }

    // * return 2 for empty rows and 3 for terminal space problems
    int print(std::ostream& stream) {
      if (rows.empty())
        return 2;

      // chose width to use
      unsigned int usable_width;
      if (forced_width)
        usable_width = width;
      else {
        unsigned short terminal_width = utils::get_terminal_width();
        usable_width = terminal_width * width_percent;
        if (this->width <= 0 || this->width > terminal_width)
          width = usable_width;
        else
          usable_width = width;
      }

      // check if the table has consistent number of columns across all rows
      bool regular = is_regular();

      // adjust the width of all columns once
      adjust_widths(usable_width);

      for (Row& row : rows) {
        size_t cols_num = row.columns.size();
        size_t row_usable_width = usable_width - (cols_num + 1); // ... - tableSplits

        if (row_usable_width <= cols_num)
          return 3;

        utils::format_row(row_usable_width, row);
      }

      if (table_border.get().style() == BorderStyle::ANSI && !utils::check_terminal() && !force_ansi)
        table_border.set().style(BorderStyle::standard);

      BorderGlyphs glyphs = table_border.get().glyphs();

      /* ------ printing the table ------- */
      bool is_first = true, is_last = false;
      if (rows.size() == 1)
        is_last = true;

      auto it = rows.begin();
      print_border(stream, it, glyphs, is_first, is_last, regular);

      is_first = false;
      for (it = rows.begin(); it != rows.end(); ++it) {
        Row& row = *it;

        print_row(stream, row, glyphs);

        if ((it + 1) == rows.end())
          is_last = true;

        print_border(stream, it, glyphs, is_first, is_last, regular);
      }

      return 0;
    }

    Row& operator[](int index) {
      return this->rows.at(index);
    }
  };

  inline std::ostream& operator<<(std::ostream& stream, const Table& table) {
    const_cast<Table&>(table).print(stream);
    return stream;
  }

} // namespace tabular
