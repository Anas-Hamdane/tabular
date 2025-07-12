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
#ifndef TABULAR_TABLE_HPP
#define TABULAR_TABLE_HPP

#include <tabular/border.hpp>
#include <tabular/row.hpp>
#include <vector>

namespace tabular {
  class Table {
    Border table_border;

    unsigned int width;

    // columns number to track Regularity
    unsigned int columns_number;
    uint8_t back_limit_percent;

    // FLAGS
    bool multi_byte_characters;
    bool separate_rows;
    bool disable_styles;
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

      // Sets the wrap threshold as a percentage of the total width.
      // Values outside the 1–100 range are ignored.
      Setters& back_limit_percent(const int percent) {
        if (percent > 0 && percent <= 100)
          table.back_limit_percent = static_cast<uint8_t>(percent);

        return *this;
      }

      // multi-byte strings support
      Setters& multi_byte_characters(const bool is_multi_byte) {
        table.multi_byte_characters = is_multi_byte;

        for (Row& row : table.rows) {
          for (Column& column : row.columns) {
            column.set().multi_byte_characters(is_multi_byte);
          }
        }

        return *this;
      }

      // border between rows
      Setters& separated_rows(const bool is_separated) {
        table.separate_rows = is_separated;
        return *this;
      }

      // disable the whole table styles.
      // useful when dealing with non-tty streams
      Setters& disable_styles(const bool disable) {
        table.disable_styles = disable;

        for (Row& row : table.rows) {
          for (Column& column : row.columns) {
            column.set().disable_styles(disable);
          }
        }

        table.table_border.set().disable_styles(disable);
        return *this;
      }
    };

    class Getters {
      const Table& table;

  public:
      explicit Getters(const Table& table) : table(table) {}

      unsigned int width() const { return table.width; }

      uint8_t back_limit_percent() const { return table.back_limit_percent; }

      bool disable_styles() const { return table.disable_styles; }

      bool regular() const { return table.regular; }

      bool separate_rows() const { return table.separate_rows; }

      bool multi_byte_characters() const { return table.multi_byte_characters; }
    };

public:
    std::vector<Row> rows;

    Table()
        : width(50),
          columns_number(0),
          back_limit_percent(25),
          separate_rows(true),
          disable_styles(false),
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
