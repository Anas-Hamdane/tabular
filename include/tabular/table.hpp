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
      -  [ ] ANSI Support
      -  [x] Alignment support
      -  [ ] terminal colors and highlights support
      -  [ ] padding control
      -  [x] width control
*/

#include <iostream>
#include <sstream>
#include <vector>

// headers
#include <tabular/column.hpp>
#include <tabular/row.hpp>

#include <tabular/style.hpp>
#include <tabular/utilities.hpp>

#include <tabular/enums.hpp>
#include <tabular/glVars.hpp>

namespace tabular {
    class Table {
        BorderStyle border;
        style::BorderTemplates templates;
        Alignment alignment;
        int horizontalPadding;
        int width;

    public:
        std::vector<Row> rows;

        Table() : border(BorderStyle::standard), alignment(Alignment::left), horizontalPadding(1), width(0) {}

        void setWidth(int width) { this->width = width; }

        int getWidth() { return this->width; }

        void addRow(std::vector<std::string> columns) {
            std::vector<Column> Columns;
            for (std::string str : columns)
                Columns.push_back(Column(str));

            rows.push_back(Row(Columns));
        }

        void setBorder(BorderStyle border) { this->border = border; }

        void setAllTableAlign(Alignment align) {
            this->alignment = align;

            for (Row& row : rows)
                row.setRowAlign(align);
        }

        void setAllColsAlign(Alignment align, int colsIndex) {
            if (colsIndex < 0)
                return;

            for (Row row : rows) {
                if (row.columns.size() > colsIndex)
                    row.columns.at(colsIndex).setColumnAlign(align);
            }
        }

        bool checkRegularity() {
            size_t referenceWidth = rows.front().columns.size();

            for (Row row : rows)
                if (row.columns.size() != referenceWidth)
                    return false;

            return true;
        }

        void corner(std::string corner) {
            if (corner.size() == 1)
                templates.corner = corner;
        }

        void horizontal(std::string horizontal) {
            if (horizontal.size() == 1)
                templates.horizontal = horizontal;
        }

        void vertical(std::string vertical) {
            if (vertical.size() == 1)
                templates.vertical = vertical;
        }

        std::string printRow(style::BorderTemplates borderTemplates, Row row, int widthReference) {
            std::string result;

            if (widthReference != 0) {
                // tableSplits = (row.columns.size() + 1)
                unsigned int usableWidth = widthReference - (row.columns.size() + 1);
                utilities::formatRow(usableWidth, horizontalPadding, row);
            }

            size_t maxSplittedContentSize = utilities::findMaxSplittedContentSize(
                row); // tallest vector of splitted strings
            for (unsigned int j = 0; j < maxSplittedContentSize; j++) {
                result.append(borderTemplates.vertical);

                for (Column col : row.columns) {
                    int rest = col.getWidth();
                    int splittedContentSize = col.getSplittedContent().size();
                    std::string currLine;

                    if (j < splittedContentSize) {
                        currLine = col.getSplittedContent().at(j);
                        result.append(currLine);
                        rest -= currLine.size(); // to balance the line
                    }

                    for (int k = 0; k < rest; k++)
                        result.append(" ");

                    result.append(borderTemplates.vertical);
                }

                result.append("\n");
            }

            return result;
        }

        std::string printBorder(style::BorderTemplates borderTemplates, Row reference) {
            std::string result;

            result.append(borderTemplates.corner);
            size_t colsNum = reference.columns.size();
            for (size_t j = 0; j < colsNum; j++) {
                Column col = reference.columns[j];
                unsigned colWidth = col.getWidth();

                for (unsigned int k = 0; k < colWidth; k++)
                    result.append(borderTemplates.horizontal);

                result.append(borderTemplates.corner);
            }

            result.append("\n");
            return result;
        }

        void printTable() {
            if (rows.size() == 0)
                return;

            std::ostringstream oss;

            // todo: add ANSI support
            // if (!OsSpecific::checkTerminal())
            //     std::cerr << "can't configure os specific terminal things\n";

            unsigned short terminalWidth = utilities::getTerminalWidth();
            unsigned int usableWidth = terminalWidth * DEFAULT_WIDTH_PERCENT;
            if (this->width <= 0 || this->width > terminalWidth)
                width = usableWidth;
            else
                usableWidth = width;

            // edit rows to match the width
            for (Row& row : rows) {
                size_t colsNum = row.columns.size();
                size_t rowUsableWidth = usableWidth - (colsNum + 1); // ... - tableSplits

                if (rowUsableWidth <= colsNum) {
                    // todo: change to error codes
                    std::cout << "Not Enough Space\n";
                    return;
                }

                utilities::formatRow(rowUsableWidth, horizontalPadding, row);
            }

            // check if the table has consistent number of columns across all rows
            bool isRegular = checkRegularity();

            // adjusting border style
            style::BorderTemplates borderTemplates = style::getBorderTemplates(border);
            if (!templates.corner.empty()) borderTemplates.corner = templates.corner;
            if (!templates.horizontal.empty()) borderTemplates.horizontal = templates.horizontal;
            if (!templates.vertical.empty()) borderTemplates.vertical = templates.vertical;
            if (!isRegular) borderTemplates.corner = borderTemplates.horizontal; // for styling

            // ------printing the table-------
            size_t i = 0;
            Row rowReference = rows.at(i);
            // 0 to check in printRow
            unsigned int rowWidthReference = isRegular ? 0 : rowReference.getFullRowWidth();

            oss << printBorder(borderTemplates, rowReference);
            for (size_t j = i; j < rows.size(); j++) {
                Row row = rows.at(j);

                oss << printRow(borderTemplates, row, rowWidthReference);

                oss << printBorder(borderTemplates, rowReference);
            }

            // ! ****** REMEMBER PADDING *******
            std::cout << oss.str();
        }
    };
} // namespace tabular