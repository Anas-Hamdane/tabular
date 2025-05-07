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

#define DEFAULT_WIDTH_PERCENT .5
#define CONTENT_MANIPULATION_BACK_LIMIT .3 // back limit percent for prepColContent()
#define ESC "\x1b"
#define CSI "\x1b["

#include <iostream>
#include <list>
#include <map>
#include <sstream>
#include <vector>

typedef std::vector<std::string> StringVector;
typedef std::list<std::string> StringList;

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
        std::vector<FontStyle> fontStyles;
        StringVector splittedContent;
        StringList words;
        Alignment alignment;
        unsigned int width;

    public:
        std::string content;

        Column(std::string content)
            : content(content), alignment(Alignment::left), width(0) {};

        void setColumnAlign(Alignment alignment) { this->alignment = alignment; }

        Alignment getColumnAlign() { return alignment; }

        void setWidth(unsigned int width) { this->width = width; }

        unsigned int getWidth() { return this->width; }

        void setSplittedContent(StringVector splittedContent) { this->splittedContent = splittedContent; }

        StringVector getSplittedContent() { return splittedContent; }

        void setWords(StringList words) { this->words = words; }

        StringList getWords() { return words; }
    };

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
        unsigned int getFullRowWidth() {
            unsigned int width = columns.size() + 1; // table splits

            for (Column col : columns)
                width += col.getWidth();

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

        inline void addSpaces(std::string& in, int spacesNum) {
            for (int i = 0; i < spacesNum; i++)
                in.append(" ");
        }

        // split by words AND save '\n' as a word too
        inline StringList split_text(std::string str) {
            StringList result;

            size_t str_size = str.size();
            size_t prev_start = 0;
            for (size_t i = 0; i < str_size; i++) {
                if (str.at(i) == '\n') {
                    if (i > prev_start)
                        result.push_back(str.substr(prev_start, i - prev_start));

                    result.push_back("\n");
                    prev_start = i + 1;
                } else if (str.at(i) == ' ') {
                    if (i > prev_start)
                        result.push_back(str.substr(prev_start, i - prev_start));

                    prev_start = i + 1;
                }
            }

            // Handles the last word if it exists
            if (prev_start < str_size)
                result.push_back(str.substr(prev_start));

            return result;
        }

        inline void setContentAlign(std::string& line, std::string sub, int maxWidth, int padding, Alignment align) {
            if (line.empty())
                addSpaces(line, padding);

            int usableWidth = (maxWidth - (padding * 2));

            int start;
            if (align == Alignment::center)
                start = (usableWidth - sub.size()) / 2;
            else if (align == Alignment::right)
                start = (usableWidth - sub.size());
            else
                start = 0;

            addSpaces(line, start);
            line.append(sub);
            addSpaces(line, padding);
        }

        inline void appendAndClear(StringVector& result, std::string& sub, int maxWidth, int padding, Alignment colAlign) {
            std::string line;

            setContentAlign(line, sub, maxWidth, padding, colAlign);
            result.push_back(line);

            sub.clear();
        }

        inline StringVector prepareColContent(Column col, int padding, int maxWidth) {
            std::string str = col.content;
            Alignment colAlign = col.getColumnAlign();

            if (col.getWidth() != 0) maxWidth = col.getWidth();

            if (str.empty() || maxWidth == 0)
                return StringVector();

            // split the content into words to easily manipulate it
            StringList words = split_text(str);

            // the return result
            StringVector result;

            const int usableWidth = (maxWidth - (padding * 2));                // e.g: MAX sub size POSSIBLE
            const int limit = (usableWidth * CONTENT_MANIPULATION_BACK_LIMIT); // don't go back more than 30% when the last word is too long

            std::string sub;
            for (auto it = words.begin(); it != words.end(); ++it) {
                std::string& word = *it;

                // add existing content if we reach new line
                if (word == "\n") {
                    appendAndClear(result, sub, maxWidth, padding, colAlign);
                    continue;
                }

                if (!sub.empty())
                    sub += ' ';

                // we need split
                if ((sub.size() + word.size()) > usableWidth) {
                    int diff = usableWidth - sub.size();
                    if (diff > limit) {
                        std::string part = word.substr(0, diff - 1);
                        part += '-';

                        sub += part;
                        appendAndClear(result, sub, maxWidth, padding, colAlign);

                        std::string remaining = word.substr(diff - 1);
                        words.insert(std::next(it), remaining);
                    } else {
                        sub.pop_back(); // pop the space added previously
                        appendAndClear(result, sub, maxWidth, padding, colAlign);
                        --it;
                    }
                } else
                    sub += word; // add a normall less than line word
            }

            // any remaining words
            if (!sub.empty())
                appendAndClear(result, sub, maxWidth, padding, colAlign);

            return result;
        }

        inline void formatRow(unsigned int width, int horPadding, Row& row) {
            if (row.columns.size() == 0)
                return;

            int colsNum = row.columns.size();
            if (width <= 0 || colsNum <= 0)
                return;

            for (Column col : row.columns) {
                if (col.getWidth() != 0) {
                    width -= col.getWidth();
                    colsNum--;
                }
            }

            int individualColWidth = 0;
            int rest = 0;
            if (colsNum > 0) {
                individualColWidth = width / colsNum;
                rest = width % colsNum;
            }

            if (horPadding >= individualColWidth)
                horPadding = 1; // reset horPadding by default value

            for (Column& col : row.columns) {
                if (col.getWidth() != 0)
                    col.setSplittedContent(prepareColContent(col, horPadding, col.getWidth()));

                else if (rest > 0) {
                    col.setSplittedContent(prepareColContent(col, horPadding, individualColWidth + 1));

                    col.setWidth(individualColWidth + 1);
                    rest--;
                } else {
                    col.setSplittedContent(prepareColContent(col, horPadding, individualColWidth));

                    col.setWidth(individualColWidth);
                }
            }
        }

        //   return the size of the tallest splittedContent vector
        inline size_t findMaxSplittedContentSize(Row row) {
            size_t result = 0;
            for (Column col : row.columns) {
                size_t splittedContentSize = col.getSplittedContent().size();
                if (splittedContentSize > result)
                    result = splittedContentSize;
            }

            return result;
        }

    } // namespace utilities

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