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
#include <tabular/definitions.hpp>

#ifndef TABULAR_UTILITIES_HPP
#define TABULAR_UTILITIES_HPP

namespace tabular {
    namespace utils {

        // to align PPDirectives
        // clang-format off
        inline unsigned short get_terminal_width() {
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

        inline void add_spaces(std::string& in, int spaces_num) {
            for (int i = 0; i < spaces_num; i++)
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

        // note: add_spaces(line, 1); for space side
        inline void set_content_align(std::string& line, std::string sub, int usable_width, Alignment align) {
            if (line.empty())
                add_spaces(line, 1);

            int start;
            if (align == Alignment::center)
                start = (usable_width - sub.size()) / 2;
            else if (align == Alignment::right)
                start = (usable_width - sub.size());
            else
                start = 0;

            add_spaces(line, start);
            line.append(sub);
            add_spaces(line, 1);
        }

        inline void append_and_clear(StringVector& result, std::string& sub, int usable_width, Alignment col_align) {
            std::string line;

            set_content_align(line, sub, usable_width, col_align);
            result.push_back(line);

            sub.clear();
        }

        inline StringVector prepare_col_content(Column col, int max_width) {
            std::string str = col.content;
            Alignment col_align = col.get_column_align();
            unsigned int top_padding = col.get_top_padding();
            unsigned int bottom_padding = col.get_bottom_padding();

            // which width to use
            if (col.get_width() != 0) max_width = col.get_width();

            if (str.empty() || max_width == 0)
                return StringVector();

            // split the content into words to easily manipulate it
            StringList words = split_text(str);

            // the return result
            StringVector result;

            // TOP padding
            for (unsigned int i = 0; i < top_padding; i++)
                result.push_back(std::string());

            const int usable_width = (max_width - 2);                            // e.g: MAX sub size POSSIBLE, - 2 for two sides spaces
            const int limit = (usable_width * CONTENT_MANIPULATION_BACK_LIMIT); // don't go back more than 30% when the last word is too long

            std::string sub;
            for (auto it = words.begin(); it != words.end(); ++it) {
                std::string& word = *it;

                // add existing content if we reach new line
                if (word == "\n") {
                    append_and_clear(result, sub, usable_width, col_align);
                    continue;
                }

                if (!sub.empty())
                    sub += ' ';

                // we need split
                if ((sub.size() + word.size()) > usable_width) {
                    int diff = usable_width - sub.size();
                    if (diff > limit) {
                        std::string part = word.substr(0, diff - 1);
                        part += '-';

                        sub += part;
                        append_and_clear(result, sub, usable_width, col_align);

                        std::string remaining = word.substr(diff - 1);
                        words.insert(std::next(it), remaining);
                    } else {
                        sub.pop_back(); // pop the space added previously
                        append_and_clear(result, sub, usable_width, col_align);
                        --it;
                    }
                } else
                    sub += word; // add a normall less than line word
            }

            // any remaining words
            if (!sub.empty())
                append_and_clear(result, sub, usable_width, col_align);

            // BOTTOM padding
            for (unsigned int i = 0; i < bottom_padding; i++)
                result.push_back(std::string());

            return result;
        }

        inline void format_row(unsigned int width, Row& row) {
            if (row.columns.size() == 0)
                return;

            int cols_num = row.columns.size();
            if (width <= 0 || cols_num <= 0)
                return;

            // for other columns width calculation we should decrease the specific ones
            for (Column col : row.columns) {
                if (col.get_width() != 0) {
                    width -= col.get_width();
                    cols_num--;
                }
            }

            int individual_col_width = 0;
            int rest = 0;
            if (cols_num > 0) {
                individual_col_width = width / cols_num;
                rest = width % cols_num;
            }

            for (Column& col : row.columns) {
                if (col.get_width() != 0)
                    col.set_splitted_content(prepare_col_content(col, col.get_width()));

                else if (rest > 0) {
                    col.set_splitted_content(prepare_col_content(col, individual_col_width + 1));

                    col.set_width(individual_col_width + 1);
                    rest--;
                } else {
                    col.set_splitted_content(prepare_col_content(col, individual_col_width));

                    col.set_width(individual_col_width);
                }
            }
        }

        //   return the size of the tallest splittedContent vector
        inline size_t find_max_splitted_content_size(Row row) {
            size_t result = 0;
            for (Column col : row.columns) {
                size_t splitted_content_size = col.get_splitted_content().size();
                if (splitted_content_size > result)
                    result = splitted_content_size;
            }

            return result;
        }

    } // namespace utilities
} // namespace tabular

#endif // TABULAR_UTILITIES_HPP