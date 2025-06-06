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

#include <climits>
#include <tabular/row.hpp>
#include <utf8stows/utf8stws.hpp>
#include <wcwidth/wcwidth.hpp>

#ifndef TABULAR_UTILITIES_HPP
#define TABULAR_UTILITIES_HPP

namespace tabular {
    namespace utils {

        // to align PPDirectives
        // clang-format off
        inline unsigned short get_terminal_width() {
            // first case: defined env var of COLUMNS
            const char* columns_env = std::getenv("COLUMNS");
            if (columns_env != nullptr) {
                try {
                    int width_int = std::stoi(columns_env);
                    if (width_int > 0 && width_int <= USHRT_MAX)
                        return static_cast<unsigned short>(width_int);
                } catch(...) {}
            }

            unsigned short width = 0;

            #if defined(OS_LINUX_BASED) || defined(OS_MACOS)
                struct winsize ws;
                if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1)
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

        inline bool check_terminal() {
            #if defined(OS_WINDOWS)
                // if it is not a valid terminal, enabling VTP will fail so we don't have to check :)
                static bool initialized = []() -> bool {
                    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
                    if (hOut == INVALID_HANDLE_VALUE)
                        return false;

                    DWORD dwMode = 0;
                    if (!GetConsoleMode(hOut, &dwMode))
                        return false;

                    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
                    if (!SetConsoleMode(hOut, dwMode))
                        return false;

                    return true;
                }();
                // SetConsoleOutputCP(CP_UTF8);
                return initialized;
            #else
                if (!isatty(STDIN_FILENO)) return false; // not a valid terminal
                return true;
            #endif
        }
        // clang-format on

        inline void add_spaces(std::string& in, int spaces_num) {
            for (int i = 0; i < spaces_num; i++)
                in.append(" ");
        }

        // split by words AND save '\n' as a word too
        inline std::list<std::string> split_text(std::string str) {
            std::list<std::string> result;

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

                    result.push_back(" ");
                    prev_start = i + 1;
                }
            }

            // Handles the last word if it exists
            if (prev_start < str_size)
                result.push_back(str.substr(prev_start));

            return result;
        }

        // note: add_spaces(line, 1); for space side
        inline void append_and_clear(std::vector<std::string>& result, std::string& sub, int usable_width, Column column) {
            std::string line;
            Alignment align = column.get().alignment();

            // auto horizontal padding of 1
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

            std::string global_styles = column.get().global_styles();
            if (!global_styles.empty())
                line.append(global_styles);

            line.append(sub);

            if (!global_styles.empty())
                line.append(RESET);

            // auto horizontal padding
            add_spaces(line, 1);

            result.push_back(line);

            sub.clear();
        }

        inline std::vector<std::string> prepare_col_content(Column& column, int max_width) {
            std::string content = column.content;
            unsigned int top_padding = column.get().top_padding();
            unsigned int bottom_padding = column.get().bottom_padding();

            // which width to use
            if (column.get().width() != 0) max_width = column.get().width();

            if (content.empty() || max_width == 0)
                return std::vector<std::string>();

            // split the content into words to easily manipulate it
            auto words = split_text(content);

            // the return result
            std::vector<std::string> result;

            // TOP padding
            for (unsigned int i = 0; i < top_padding; i++)
                result.push_back(std::string());

            // e.g: MAX sub size POSSIBLE, - 2 for two sides spaces
            const int usable_width = (max_width - 2);

            // don't go back more than 30% when the last word is too long
            const int limit = (usable_width * CONTENT_MANIPULATION_BACK_LIMIT);

            std::string sub;
            for (auto it = words.begin(); it != words.end(); ++it) {
                std::string& word = *it;

                // add existing content if we reach new line
                if (word == "\n") {
                    append_and_clear(result, sub, usable_width, column);
                    continue;
                }

                // we need split
                if ((sub.size() + word.size()) > usable_width) {
                    int diff = usable_width - sub.size();
                    if (diff > limit) {
                        std::string part = word.substr(0, diff - 1);
                        part += '-';

                        sub += part;
                        append_and_clear(result, sub, usable_width, column);

                        std::string remaining = word.substr(diff - 1);
                        words.insert(std::next(it), remaining);
                    } else {
                        sub.pop_back(); // pop the space added previously which is a word itself
                        append_and_clear(result, sub, usable_width, column);
                        --it;
                    }
                } else
                    sub += word; // add a normall less than line word
            }

            // any remaining words
            if (!sub.empty())
                append_and_clear(result, sub, usable_width, column);

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
            for (Column& column : row.columns) {
                if (column.get().width() != 0) {
                    width -= column.get().width();
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
                if (col.get().width() != 0)
                    col.set().splitted_content(prepare_col_content(col, col.get().width()));

                else if (rest > 0) {
                    col.set().splitted_content(prepare_col_content(col, individual_col_width + 1));

                    col.set().width(individual_col_width + 1);
                    rest--;
                } else {
                    col.set().splitted_content(prepare_col_content(col, individual_col_width));

                    col.set().width(individual_col_width);
                }
            }
        }

        //   return the size of the tallest splittedContent vector
        inline size_t find_max_splitted_content_size(Row row) {
            size_t result = 0;
            for (Column col : row.columns) {
                size_t splitted_content_size = col.get().splitted_content().size();
                if (splitted_content_size > result)
                    result = splitted_content_size;
            }

            return result;
        }

        inline size_t mbswidth(std::string str) {
            wchar_t* wstr = utf8stws(str.c_str());
            return ah_wcswidth(wstr, wcslen(wstr));
        }
    } // namespace utils
} // namespace tabular

#endif // TABULAR_UTILITIES_HPP
