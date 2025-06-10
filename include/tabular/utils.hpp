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
#include <utf8conv/utf8conv.hpp>
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

        // multi byte string width
        inline size_t mbswidth(std::string str) {
            std::wstring wstr = conv::utf8stws(str.c_str());
            size_t width = display_width::wcswidth(wstr.c_str(), wstr.length());

            return width;
        }

        inline size_t display_width(std::string str, bool is_multi_byte) {
            return is_multi_byte ? mbswidth(str) : str.length();
        }

        // split by words AND save both '\n' and ' ' as words too
        // works at least for UTF-8 encoding strings
        inline std::list<std::string> split_text(const std::string& str) {
            std::list<std::string> result;

            size_t start = 0;
            const size_t len = str.size();

            for (size_t i = 0; i < len; ++i) {
                char ch = str[i];

                if (ch == ' ' || ch == '\n') {
                    if (i > start)
                        result.emplace_back(str.begin() + start, str.begin() + i);

                    result.emplace_back(1, ch); // Push " " or "\n"
                    start = i + 1;
                }
            }

            if (start < len)
                result.emplace_back(str.begin() + start, str.end());

            return result;
        }

        // finalize line and push it
        inline void commit_line(std::vector<std::string>& result, std::string& sub, size_t* sub_size, int usable_width, Column column) {
            std::string line;
            Alignment align = column.get().alignment();

            // auto horizontal padding of 1
            if (line.empty())
                line.append(" ");

            int start;
            if (align == Alignment::center)
                start = (usable_width - (*sub_size)) / 2;
            else if (align == Alignment::right)
                start = (usable_width - (*sub_size));
            else
                start = 0;

            line.append(start, ' ');

            std::string styles;

            // styling
            styles += column.get().text_styles();
            styles += column.get().content_color();
            styles += column.get().content_background_color();

            if (!styles.empty())
                line.append(styles);

            line.append(sub);

            if (!styles.empty())
                line.append(RESET);

            /* no need */
            // auto horizontal padding
            // line.append(" ");

            result.push_back(line);

            sub.clear();
            *sub_size = 0;
        }

        inline std::vector<std::string> format_column_lines(Column& column) {
            const std::string& content = column.content;
            const bool is_multi_byte = column.get().is_multi_byte();

            if (content.empty())
                return std::vector<std::string>();

            unsigned int top_padding = column.get().top_padding();
            unsigned int bottom_padding = column.get().bottom_padding();

            // split the content into words to easily manipulate it
            auto words = split_text(content);

            // the return result
            std::vector<std::string> result;

            // TOP padding
            result.insert(result.end(), top_padding, std::string());

            // e.g: MAX sub size POSSIBLE, - 2 for two sides spaces
            const int usable_width = (column.get().width() - 2);

            // don't go back more than 30%, when the last word is too long
            const int limit = (usable_width * CONTENT_MANIPULATION_BACK_LIMIT);

            std::string sub;
            size_t sub_size = 0;
            for (auto it = words.begin(); it != words.end(); ++it) {
                std::string& word = *it;

                // add existing content if we reach new line
                if (word == "\n") {
                    commit_line(result, sub, &sub_size, usable_width, column);
                    continue;
                }

                // we need split
                size_t word_size = display_width(word, is_multi_byte);
                if ((sub_size + word_size) > usable_width) {
                    int remaining_space = usable_width - sub_size;

                    // force split
                    if (remaining_space > limit) {
                        std::string first_part;
                        std::string remainder;
                        if (is_multi_byte) {
                            std::wstring wword = conv::utf8stws(word);
                            std::wstring wpart = wword.substr(0, remaining_space - 1) + L'-';
                            std::wstring wremainder = wword.substr(remaining_space - 1);

                            first_part = conv::utf8wsts(wpart);
                            remainder = conv::utf8wsts(wremainder);
                        } else {
                            first_part = word.substr(0, remaining_space - 1) + '-';
                            remainder = word.substr(remaining_space - 1);
                        }

                        sub += first_part;
                        sub_size += display_width(first_part, is_multi_byte);

                        commit_line(result, sub, &sub_size, usable_width, column);

                        words.insert(std::next(it), remainder);
                    }
                    // commit line and append the word on the next line
                    else {
                        if (!sub.empty() && sub.back() == ' ') {
                            sub.pop_back(); // pop the last space if exist
                            sub_size--;
                        }

                        commit_line(result, sub, &sub_size, usable_width, column);
                        --it; // to append the word on the next iteration
                    }
                } else {
                    sub += word; // add a normall less than line word
                    sub_size += word_size;
                }
            }

            // any remaining words
            if (!sub.empty())
                commit_line(result, sub, &sub_size, usable_width, column);

            // BOTTOM padding
            result.insert(result.end(), bottom_padding, std::string());

            return result;
        }

        inline void format_row(unsigned int width, Row& row) {
            for (Column& column : row.columns)
                column.set().splitted_content(
                    format_column_lines(column));
        }

        // return the size of the tallest splittedContent vector
        inline size_t find_max_splitted_content_size(const Row& row) {
            size_t result = 0;
            for (Column column : row.columns) {
                size_t splitted_content_size = column.get().splitted_content().size();
                if (splitted_content_size > result)
                    result = splitted_content_size;
            }

            return result;
        }

    } // namespace utils
} // namespace tabular

#endif // TABULAR_UTILITIES_HPP
