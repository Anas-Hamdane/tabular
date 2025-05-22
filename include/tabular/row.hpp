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

#ifndef TABULAR_ROW_HPP
#define TABULAR_ROW_HPP

#include <tabular/column.hpp>

#include <stdexcept>

namespace tabular {
    class Row {
        Alignment alignment;

        class Config {
            Row& row;

        public:
            Config(Row& row) : row(row) {}

            Config& alignment(Alignment alignment) {
                row.alignment = alignment;

                for (Column& col : row.columns)
                    col.config().alignment(alignment);

                return *this;
            }

            Config& padding(int padding) {
                for (Column& col : row.columns)
                    col.config().padding(padding);

                return *this;
            }

            Config& top_padding(int padding) {
                for (Column& col : row.columns)
                    col.config().top_padding(padding);

                return *this;
            }

            Config& bottom_padding(int padding) {
                for (Column& col : row.columns)
                    col.config().bottom_padding(padding);

                return *this;
            }

            Config& add_font_styles(const FontStylesVector& styles) {
                for (Column& column : row.columns)
                    column.config().add_font_styles(styles);

                return *this;
            }

            Config& remove_font_styles(const FontStylesVector& styles) {
                for (Column& col : row.columns)
                    col.config().remove_font_styles(styles);

                return *this;
            }
        };

    public:
        std::vector<Column> columns;

        Row(std::vector<Column> columns)
            : columns(columns), alignment(Alignment::left) {}

        Config config() { return Config(*this); }

        int get_columns_number() { return columns.size(); }

        unsigned int get_full_row_width() {
            unsigned int width = columns.size() + 1;

            for (Column column : columns)
                width += column.get().width();

            return width;
        }

        Column& operator[](int index) {
            if (index >= this->columns.size() || index < 0)
                throw std::out_of_range("Index out of bounds");

            return this->columns[index];
        }
    };
} // namespace tabular

#endif // TABULAR_ROW_HPP
