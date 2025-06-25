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
#ifndef TABULAR_TABLE_HPP
#define TABULAR_TABLE_HPP

#include <vector>

#include <tabular/border.hpp>

namespace tabular {
  class Table {
    Border table_border;

    unsigned int width;

    // used when the output stream is not
    // a valid TTY
    unsigned int non_tty_width;

    // columns number to track Regularity
    unsigned int columns_number;

    uint8_t width_percent;
    uint8_t back_limit_percent;

    bool separated_rows;
    bool disabled_styles;
    bool regular;

    class Setters {
      Table& table;

  public:
      explicit Setters(Table& table) : table(table) {}

      Setters& width(const int width) {
        if (width > 0)
          table.width = static_cast<unsigned int>(width);
        else
          table.width = 0;

        return *this;
      }

      // Sets the table width as a percentage of the total width of the terminal.
      // Values outside the 1–100 range are ignored.
      Setters& width_percent(const int percent) {
        if (percent > 0 && percent <= 100)
          table.width_percent = static_cast<uint8_t>(percent);

        return *this;
      }

      // Sets the wrap threshold as a percentage of the total width.
      // Values outside the 1–100 range are ignored.
      Setters& back_limit_percent(const int percent) {
        if (percent > 0 && percent <= 100)
          table.back_limit_percent = static_cast<uint8_t>(percent);

        return *this;
      }

      // Sets the table width for invalid output streams.
      Setters& non_tty_width(const int width) {
        if (width > 0)
          table.non_tty_width = static_cast<unsigned int>(width);

        return *this;
      }

      // multibyte strings support
      Setters& multi_byte_characters(const bool is_multi_byte) {
        for (Row& row : table.rows)
          for (Column& column : row.columns)
            column.set().multi_byte_characters(is_multi_byte);

        return *this;
      }

      // border between rows
      Setters& separated_rows(const bool is_separated) {
        table.separated_rows = is_separated;
        return *this;
      }

      // disable the whole table styles.
      // useful when dealing with non-tty streams
      Setters& disabled_styles(const bool is_disabled) {
        table.disabled_styles = is_disabled;
        return *this;
      }
    };

    class Getters {
      const Table& table;

  public:
      explicit Getters(const Table& table) : table(table) {}

      unsigned int width() const { return table.width; }

      uint8_t width_percent() const { return table.width_percent; }

      uint8_t back_limit_percent() const { return table.back_limit_percent; }

      unsigned int non_tty_width() const { return table.non_tty_width; }

      bool disabled_styles() const { return table.disabled_styles; }

      bool regular() const { return table.regular; }

      bool separated_rows() const { return table.separated_rows; }
    };

public:
    std::vector<Row> rows;

    Table()
        : width(50),
          non_tty_width(50),
          columns_number(0),
          width_percent(50),
          back_limit_percent(25),
          separated_rows(true),
          disabled_styles(false),
          regular(true) {}

    Border& border() { return table_border; }

    Setters set() { return Setters(*this); }

    Getters get() const { return Getters(*this); }

    Table& add_row(const std::vector<std::string>& contents) {
      std::vector<Column> columns;
      columns.reserve(contents.size());

      for (const std::string& content : contents)
        columns.emplace_back(content);

      // tracking Regularity
      if (regular) {
        if (this->rows.empty())
          this->columns_number = columns.size();

        else if (columns_number != columns.size())
          regular = false;
      }

      this->rows.emplace_back(std::move(columns));
      return *this;
    }

    Row& operator[](const int index) {
      return this->rows.at(index);
    }
  };
} // namespace tabular

#endif // TABULAR_TABLE_HPP