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

namespace tabular {
    class Row {
        Alignment alignment;
        std::vector<FontStyle> font_styles;

    public:
        std::vector<Column> columns;

        Row(std::vector<Column> columns)
            : columns(columns), alignment(Alignment::left) {}

        int get_columns_number() { return columns.size(); }

        Row& set_row_align(Alignment alignment) {
            this->alignment = alignment;

            for (Column& col : columns)
                col.set_column_align(alignment);

            return *this;
        }
        
        Row& set_row_padding(int padding) {
            for (Column& col : columns)
                col.set_column_padding(padding);

            return *this;
        }

        Row& set_row_top_padding(int padding) {
            for (Column& col : columns)
                col.set_column_top_padding(padding);

            return *this;
        }
        
        Row& set_row_bottom_padding(int padding) {
            for (Column& col : columns)
                col.set_column_bottom_padding(padding);

            return *this;
        }

        unsigned int get_full_row_width() {
            unsigned int width = columns.size() + 1;

            for (Column col : columns)
                width += col.get_width();

            return width;
        }
    };
} // namespace tabular

#endif // TABULAR_ROW_HPP