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

#include <tabular/row.hpp>
#include <tabular/glVars.hpp>

#ifndef TABULAR_UTILITIES_HPP
#define TABULAR_UTILITIES_HPP

namespace tabular {
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

        // note: addSpaces(line, 1); for space side
        inline void setContentAlign(std::string& line, std::string sub, int usableWidth, Alignment align) {
            if (line.empty())
                addSpaces(line, 1);

            int start;
            if (align == Alignment::center)
                start = (usableWidth - sub.size()) / 2;
            else if (align == Alignment::right)
                start = (usableWidth - sub.size());
            else
                start = 0;

            addSpaces(line, start);
            line.append(sub);
            addSpaces(line, 1);
        }

        inline void appendAndClear(StringVector& result, std::string& sub, int usableWidth, Alignment colAlign) {
            std::string line;

            setContentAlign(line, sub, usableWidth, colAlign);
            result.push_back(line);

            sub.clear();
        }

        inline StringVector prepareColContent(Column col, int maxWidth) {
            std::string str = col.content;
            Alignment colAlign = col.getColumnAlign();
            unsigned int topPadding = col.getTopPadding();
            unsigned int bottomPadding = col.getBottomPadding();

            // which width to use
            if (col.getWidth() != 0) maxWidth = col.getWidth();

            if (str.empty() || maxWidth == 0)
                return StringVector();

            // split the content into words to easily manipulate it
            StringList words = split_text(str);

            // the return result
            StringVector result;

            // TOP padding
            for (unsigned int i = 0; i < topPadding; i++)
                result.push_back(std::string());

            const int usableWidth = (maxWidth - 2);                            // e.g: MAX sub size POSSIBLE, - 2 for two sides spaces
            const int limit = (usableWidth * CONTENT_MANIPULATION_BACK_LIMIT); // don't go back more than 30% when the last word is too long

            std::string sub;
            for (auto it = words.begin(); it != words.end(); ++it) {
                std::string& word = *it;

                // add existing content if we reach new line
                if (word == "\n") {
                    appendAndClear(result, sub, usableWidth, colAlign);
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
                        appendAndClear(result, sub, usableWidth, colAlign);

                        std::string remaining = word.substr(diff - 1);
                        words.insert(std::next(it), remaining);
                    } else {
                        sub.pop_back(); // pop the space added previously
                        appendAndClear(result, sub, usableWidth, colAlign);
                        --it;
                    }
                } else
                    sub += word; // add a normall less than line word
            }

            // any remaining words
            if (!sub.empty())
                appendAndClear(result, sub, usableWidth, colAlign);

            // BOTTOM padding
            for (unsigned int i = 0; i < bottomPadding; i++)
                result.push_back(std::string());

            return result;
        }

        inline void formatRow(unsigned int width, Row& row) {
            if (row.columns.size() == 0)
                return;

            int colsNum = row.columns.size();
            if (width <= 0 || colsNum <= 0)
                return;

            // for other columns width calculation we should decrease the specific ones
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

            for (Column& col : row.columns) {
                if (col.getWidth() != 0)
                    col.setSplittedContent(prepareColContent(col, col.getWidth()));

                else if (rest > 0) {
                    col.setSplittedContent(prepareColContent(col, individualColWidth + 1));

                    col.setWidth(individualColWidth + 1);
                    rest--;
                } else {
                    col.setSplittedContent(prepareColContent(col, individualColWidth));

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
} // namespace tabular

#endif // TABULAR_UTILITIES_HPP