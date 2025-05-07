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

#include <tabular/column.hpp>

#ifndef TABULAR_ROW_HPP
#define TABULAR_ROW_HPP

namespace tabular {
    class Row {
        Alignment alignment;
        std::vector<FontStyle> fontStyles;

    public:
        std::vector<Column> columns;

        Row(std::vector<Column> columns)
            : columns(columns), alignment(Alignment::left) {}
        // bool isHeader = false;

        int getColumnsNumber() { return columns.size(); }

        void setRowAlign(Alignment alignment) {
            this->alignment = alignment;

            for (Column& col : columns)
                col.setColumnAlign(alignment);
        }

        // full width including table splits
        unsigned int getRowWidth() {
            unsigned int width = columns.size() + 1; // table splits

            for (Column col : columns)
                width += col.getWidth();

            return width;
        }
    };
} // namespace tabular

#endif // TABULAR_ROW_HPP