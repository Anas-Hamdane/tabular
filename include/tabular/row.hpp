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
        unsigned int width;

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

            Config& color(Color color) {
                for (Column& col : row.columns)
                    col.config().color(color);

                return *this;
            }

            Config& background_color(BackgroundColor back_color) {
                for (Column& col : row.columns)
                    col.config().background_color(back_color);

                return *this;
            }

            Config& rgb(RGB rgb) {
                for (Column& col : row.columns)
                    col.config().rgb(rgb);

                return *this;
            }

            Config& background_rgb(RGB background_rgb) {
                for (Column& col : row.columns)
                    col.config().background_rgb(background_rgb);

                return *this;
            }

            Config& multi_byte_chars(bool is_multi_byte) {
                for (Column& col : row.columns)
                    col.config().multi_byte_chars(is_multi_byte);

                return *this;
            }
        };

    public:
        std::vector<Column> columns;

        Row(std::vector<Column> columns)
            : columns(columns), alignment(Alignment::left) {}

        Config config() { return Config(*this); }

        Column& operator[](int index) {
            if (index >= this->columns.size() || index < 0)
                throw std::out_of_range("Index out of bounds");

            return this->columns[index];
        }
    };
} // namespace tabular

#endif // TABULAR_ROW_HPP
