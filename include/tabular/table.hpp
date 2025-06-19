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

#include <vector>

// this include all headers
#include <tabular/border.hpp>

namespace tabular {
  class Table {
    Border table_border;

    unsigned int width;
    unsigned int non_tui_width;

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
      Setters(Table& table) : table(table) {}

      Setters& width(int width) {
        if (width > 0)
          table.width = static_cast<unsigned int>(width);
        else
          table.width = 0;

        return *this;
      }

      // percent should be between 1 and 100
      Setters& width_percent(int percent) {
        if (percent > 0 && percent <= 100)
          table.width_percent = static_cast<uint8_t>(percent);

        return *this;
      }

      // percent should be between 1 and 100
      Setters& back_limit_percent(int percent) {
        if (percent > 0 && percent <= 100)
          table.back_limit_percent = static_cast<uint8_t>(percent);

        return *this;
      }

      Setters& non_tui_width(int width) {
        if (width > 0)
          table.non_tui_width = static_cast<unsigned int>(width);

        return *this;
      }

      Setters& multi_byte_characters(bool is_multi_byte) {
        for (Row& row : table.rows)
          for (Column& column : row.columns)
            column.set().multi_byte_characters(is_multi_byte);

        return *this;
      }

      Setters& separated_rows(bool is_separated) {
        table.separated_rows = is_separated;
        return *this;
      }

      Setters& disabled_styles(bool is_disabled) {
        table.disabled_styles = is_disabled;
        return *this;
      }
    };

    class Getters {
      const Table& table;

  public:
      Getters(const Table& table) : table(table) {}

      unsigned int width() const { return table.width; }

      uint8_t width_percent() const { return table.width_percent; }

      uint8_t back_limit_percent() const { return table.back_limit_percent; }

      unsigned int non_tui_width() const { return table.non_tui_width; }

      bool disabled_styles() const { return table.disabled_styles; }

      bool regular() const { return table.regular; }

      bool separated_rows() const { return table.separated_rows; }
    };

public:
    std::vector<Row> rows;

    Table() : width(0), width_percent(60), non_tui_width(60), columns_number(0), back_limit_percent(30), separated_rows(true), disabled_styles(false), regular(true) {}

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

    Row& operator[](int index) {
      return this->rows.at(index);
    }
  };
} // namespace tabular
