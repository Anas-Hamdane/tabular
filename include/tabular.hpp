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
        -  [ ] Alignment support
        -  [ ] terminal colors and highlights support
        -  [ ] padding control
        -  [ ] width control
*/

#if defined(_WIN32) || defined(_WIN64)
    #define OS_WINDOWS
#elif defined(__APPLE__)
    #define OS_MACOS
#elif defined(__unix__) || defined(__unix)
    #define OS_LINUX_BASED
#else
    #error unsupported platform
#endif

#if defined(OS_WINDOWS)
    #include <windows.h>
#elif defined(OS_LINUX_BASED) || defined(OS_MACOS)
    #include <sys/ioctl.h>
#else
    #error Unsupported platform
#endif

#define AUTOMATIC_WIDTH_PERCENT 0.6
#define ESC "\x1b"
#define CSI "\x1b["

#include <iostream>
#include <map>
#include <sstream>
#include <vector>

namespace tabular {
    enum class Alignment {
        left,
        center,
        right
    };
    enum class FontStyle {
        bold,
        dark,
        italic,
        underline,
        blink,
        reverse,
        concealed,
        crossed
    };
    enum class BorderStyle {
        standard,
        empty,
        ANSI // ! Not implemented
    };

    class Column {
    public:
        std::string content;
        std::vector<std::string> splittedContent;
        Alignment alignment;
        std::vector<FontStyle> fontStyles;
        unsigned int width;

        Column(std::string content)
            : content(content), alignment(Alignment::left), width(0) {};

        void printContent() {}

        void setColumnAlign(Alignment alignment) { this->alignment = alignment; }
    };

    class Row {
    public:
        std::vector<Column> columns;
        std::vector<FontStyle> fontStyles;
        Alignment alignment;

        Row(std::vector<Column> columns)
            : columns(columns), alignment(Alignment::left) {}
        // bool isHeader = false;

        int getColumnsNumber() { return columns.size(); }

        void setRowAlign(Alignment alignment) {
            this->alignment = alignment;

            for (Column col : columns)
                col.setColumnAlign(alignment);
        }

        // full width including table splits
        unsigned int getRowWidth() {
            unsigned int width = columns.size() + 1; // table splits

            for (Column col : columns)
                width += col.width;

            return width;
        }
    };

    namespace style {
        struct BorderTemplates {
            std::string vertical;
            std::string horizontal;
            std::string corner;
        };

        static BorderTemplates getBorderTemplates(BorderStyle borderStyle) {
            static std::map<BorderStyle, BorderTemplates> templates{
                {BorderStyle::empty, {" ", " ", " "}},
                {BorderStyle::standard, {"|", "-", "+"}},
                {BorderStyle::ANSI, {}}};
            return templates[borderStyle];
        }
    }; // namespace style

    namespace utilities {

        // to align PPDirectives
        // clang-format off
        inline unsigned short getTerminalWidth() {
            unsigned short width = 0;

            #if defined(OS_LINUX_BASED) || defined(OS_MACOS)
                struct winsize ws;
                if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) == -1)
                    width = 0;
                else 
                    width = ws.ws_col;
            #elif defined(OS_WINDOWS)
                CONSOLE_SCREEN_BUFFER_INFO csbi;
                if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
                    width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
            #else
                width = 0;
            #endif

            return width;
        }
        // clang-format on

        inline void addSpaces(std::string &in, int spacesNum) {
            for (int i = 0; i < spacesNum; i++)
                in.append(" ");
        }

        inline std::vector<std::string> splitText(const std::string str, int horPadd, int maxWidth) {
            if (str.empty() || maxWidth == 0)
                return std::vector<std::string>();

            std::vector<std::string> result;

            const int redLine = (maxWidth * 0.3); // don't go back more than 30% of the width
            const size_t strSize = str.size();
            size_t prevStart = 0;
            for (size_t i = maxWidth; i < strSize; i += maxWidth) {
                // find the last space within the allowed maxWidth
                size_t spaceIndex = i;
                bool forceCut = false;
                int iterationsNum = 0;

                /*
                    * first condition spaceIndex > 0 to avoid errors.
                    * second condition (str.at(spaceIndex) != ' ') to go back whenever it is not a space until we reach a *VALID* space or the red line.
                    * third condition check for valid spaces: they should be ( > (horPadd * 2)) behind the max width.
                */
                while ((spaceIndex > 0) && ((str.at(spaceIndex) != ' ') || (str.at(spaceIndex) == ' ' && iterationsNum <= (horPadd * 2)))) {
                    if (iterationsNum >= redLine) {
                        forceCut = true;
                        break;
                    }

                    else {
                        spaceIndex--;
                        iterationsNum++;
                    }
                }

                std::string line;
                addSpaces(line, horPadd);

                /*
                    * whenever there's a force cut we decrement 'i' by the space taken by the horPadd and/or the '-' character.
                */
                if (forceCut) {
                    /*
                        if and else replacing
                        ... - (maxWidth > 5? 1 : 0)) + (maxWidth > 5?) "-" : "")
                    */
                    if (maxWidth <= 5)
                        line.append(str.substr(prevStart, i - prevStart - (horPadd * 2)));
                    else
                        line.append(str.substr(prevStart, i - prevStart - 1 - (horPadd * 2)) + "-");

                    i -= maxWidth <= 5? (horPadd * 2) : ((horPadd * 2) + 1);
                }
                /*
                    * whenever there's a *VALID* space just split
                    * note: we don't decrement 'i' because it is already (> (horPadd * 2)) behind the max width, it is a VALID SPACE.
                */
                else if (str.at(spaceIndex) == ' ') {
                    i = spaceIndex + 1; // skip space
                    line.append(str.substr(prevStart, spaceIndex - prevStart));
                } 
                
                
                //  * Probably a very small string case, didn't test it lol.
                else {
                    line.append(str.substr(prevStart, i - prevStart - (horPadd * 2)));
                }

                addSpaces(line, horPadd);

                prevStart = i; // update the previous start point
                result.push_back(line);
            }

            //  * less than a line-string case.
            if (prevStart < strSize) {
                std::string line;
                addSpaces(line, horPadd);
                
                line.append(str.substr(prevStart, strSize - prevStart));

                addSpaces(line, horPadd);

                result.push_back(line);
            }

            return result;
        }

        inline void formatRow(unsigned int width, int horPadding, Row& row) {
            if (row.columns.size() == 0)
                return;

            int colsNum = row.columns.size();
            if (width <= 0 || colsNum <= 0)
                return;

            int individualColWidth = width / colsNum;
            int rest = width % colsNum;

            if (horPadding >= individualColWidth)
                horPadding = 1; // reset horPadding by default value

            for (Column& col : row.columns) {
                if (rest > 0) {
                    col.splittedContent = splitText(col.content, horPadding, individualColWidth + 1);
                    col.width = individualColWidth + 1;
                    rest--;
                } else {
                    col.splittedContent = splitText(col.content, horPadding, individualColWidth);
                    col.width = individualColWidth;
                }
            }
        }

        //   return the size of the tallest splittedContent vector
        inline size_t findMaxSplittedContentSize(Row row) {
            size_t result = 0;
            for (Column col : row.columns)
                if (col.splittedContent.size() > result)
                    result = col.splittedContent.size();

            return result;
        }

    } // namespace utilities

    class Table {
        std::vector<Row> rows;
        BorderStyle border;
        style::BorderTemplates templates;
        Alignment alignment;
        int horizontalPadding;

    public:
        Table() : border(BorderStyle::standard), alignment(Alignment::left), horizontalPadding(1) {}

        void addRow(std::vector<std::string> columns) {
            std::vector<Column> Columns;
            for (std::string str : columns)
                Columns.push_back(Column(str));

            rows.push_back(Row(Columns));
        }

        void setBorder(BorderStyle border) { this->border = border; }

        bool checkRegularity() {
            size_t referenceWidth = rows.front().columns.size();

            for (Row row : rows)
                if (row.columns.size() != referenceWidth)
                    return false;

            return true;
        }

        void setGlobalTextAlign(Alignment alignment) {
            this->alignment = alignment;

            for (Row& row : rows)
                row.setRowAlign(alignment);
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
                    int rest = col.width;
                    if (j < col.splittedContent.size()) {
                        result.append(col.splittedContent.at(j));
                        rest = col.width - col.splittedContent.at(j).size(); // to balance the line
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

                for (unsigned int k = 0; k < col.width; k++)
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

            unsigned short width = utilities::getTerminalWidth();
            unsigned int usableWidth = width * AUTOMATIC_WIDTH_PERCENT;

            size_t colsNum;
            size_t i = 0;
            do {
                if (i >= rows.size())
                    return;
                colsNum = rows.at(i).columns.size();
                i++;
            } while (colsNum == 0);
            i--; // negate last increment

            size_t tableSplits = colsNum + 1; // cols reserved by the table
            usableWidth -= tableSplits;
            if (usableWidth <= (colsNum)) { // at least one character in each column
                std::cout << "Not enough space";
                return;
            }

            // edit rows to match the width
            for (Row& row : rows)
                utilities::formatRow(usableWidth, horizontalPadding, row);

            // check if the table has consistent number of columns across all rows
            bool isRegular = checkRegularity();

            // adjusting border style
            style::BorderTemplates borderTemplates = style::getBorderTemplates(border);
            if (!templates.corner.empty()) borderTemplates.corner = templates.corner;
            if (!templates.horizontal.empty()) borderTemplates.horizontal = templates.horizontal;
            if (!templates.vertical.empty()) borderTemplates.vertical = templates.vertical;
            if (!isRegular) borderTemplates.corner = borderTemplates.horizontal; // for styling

            // ------printing the table-------
            Row rowReference = rows.at(i);
            // 0 to check in printRow
            unsigned int rowWidthReference = isRegular ? 0 : rowReference.getRowWidth();

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