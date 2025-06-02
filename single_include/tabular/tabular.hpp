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
      -  [x] ANSI Support
      -  [x] Alignment support
      -  [x] terminal colors and 16 colors support
      -  [x] terminal colors rgb support
      -  [x] terminal font styles support
      -  [x] padding control
      -  [x] width control
      -  [x] range columns setters (functions)
      -  [ ] sub string styling support [VEEEEEEEEEEEEERY HAAAAAAAAAAARD]
*/

#include <algorithm>
#include <climits>
#include <cstdint>
#include <list>
#include <map>
#include <ostream>
#include <string>
#include <vector>

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
#include <unistd.h>
#else
#error Unsupported platform
#endif

#define DEFAULT_WIDTH_PERCENT .5
#define CONTENT_MANIPULATION_BACK_LIMIT .3 // back limit percent

// table ANSI printing
#define TABLE_MODE "\x1b(0"
#define RESET_TABLE_MODE "\x1B(B"

// Control sequence introducer (for font styles and colors)
#define CSI "\x1b["
#define RESET "\x1b[0m"

namespace tabular {
    enum class Alignment {
        left,
        center,
        right
    };

    enum class BorderStyle {
        standard,
        empty,
        ANSI
    };

    enum class Font {
        bold = 1,
        dim,
        italic,
        underline,
        blink, // rarely supported
        reverse = 7,
        concealed,
        crossed
    };

    enum class Color {
        black = 30,
        red,
        green,
        yellow,
        blue,
        magenta,
        cyan,
        white,
        normal = 39,
    };
    enum class BackgroundColor {
        black = 40,
        red,
        green,
        yellow,
        blue,
        magenta,
        cyan,
        white,
        normal = 49,
    };

    enum class Style {
        bold = 1,
        dim = 2,
        italic = 3,

        underline = 4,
        double_underline = 21, // not popular

        blink = 5,      // rarely supported
        fast_blink = 6, // rarely supported
        reverse = 7,
        concealed = 8,
        crossed = 9,
    };

    enum class ResetStyle {
        bold = 22,
        dim = 22,
        italic = 23,

        underline = 24,
        double_underline = 24,

        blink = 25,
        fast_blink = 25,
        reverse = 27,
        concealed = 28,
        crossed = 29
    };

    struct Border {
        std::string vertical;
        std::string horizontal;

        std::string corner;
        std::string bottom_right_corner;
        std::string top_right_corner;
        std::string top_left_corner;
        std::string bottom_left_corner;

        std::string middle_separator; // +

        std::string left_to_right;
        std::string right_to_left;
        std::string bottom_to_top;
        std::string top_to_bottom;
    };

    typedef struct Range {
        int from;
        int to;

        Range(int from, int to) : from(from), to(to) {}
    } Range;

    typedef struct RGB {
        uint8_t r, g, b;

        RGB(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}
    } RGB;

    class Column {
        // styles applied to the whole content OR a specific sub string
        std::string global_styles;
        std::vector<std::string> splitted_content;
        std::list<std::string> words;
        Alignment alignment;
        unsigned int width;
        unsigned int top_padding;
        unsigned int bottom_padding;
        unsigned int special_characters;

        class Config {
            Column& column;

        public:
            Config(Column& column) : column(column) {}

            Config& alignment(Alignment align) {
                column.alignment = align;

                return *this;
            }

            Config& width(int w) {
                if (w <= 0)
                    w = 0;

                column.width = static_cast<unsigned int>(w);

                return *this;
            }

            Config& padding(int padd) {
                if (padd <= 0)
                    padd = 0;

                column.top_padding = padd;
                column.bottom_padding = padd;

                return *this;
            }

            Config& top_padding(int padd) {
                if (padd <= 0)
                    column.top_padding = 0;
                else
                    column.top_padding = static_cast<unsigned int>(padd);

                return *this;
            }

            Config& bottom_padding(int padd) {
                if (padd <= 0)
                    column.bottom_padding = 0;
                else
                    column.bottom_padding = static_cast<unsigned int>(padd);

                return *this;
            }

            // add text style to the whole column
            Config& text_style(Style style) {
                // style statement
                std::string style_stt = CSI + std::to_string(static_cast<int>(style)) + "m";

                column.global_styles += style_stt;

                // later add special_characters of RESET in utils in the last element of splitted_content
                return *this;
            }

            // add multiple text styles to the whole column
            Config& text_style(const std::vector<Style>& styles) {
                if (styles.empty()) return *this;

                // Apply all styles at the beginning
                std::string styles_stt;
                for (const auto& style : styles) {
                    styles_stt += CSI + std::to_string(static_cast<int>(style)) + "m";
                }

                column.global_styles += styles_stt;

                return *this;
            }

            // add Background Color to the whole column
            Config& background_color(BackgroundColor color) {

                // Background Color statement
                std::string back_color_stt = CSI + std::to_string(static_cast<int>(color)) + "m";

                column.global_styles += back_color_stt;

                return *this;
            }

            // add Color to the whole column
            Config& color(Color color) {

                // Color statement
                std::string color_stt = CSI + std::to_string(static_cast<int>(color)) + "m";

                column.global_styles += color_stt;

                return *this;
            }

            // add RGB to the whole column
            Config& rgb(RGB rgb) {

                // RGB statement
                std::string rgb_stt = CSI "38;2;" + std::to_string(rgb.r) + ";" +
                                      std::to_string(rgb.g) + ";" +
                                      std::to_string(rgb.b) + "m";

                column.global_styles += rgb_stt;
                return *this;
            }

            // add background RGB to the whole column
            Config& background_rgb(RGB rgb) {

                // RGB statement
                std::string back_rgb_stt = CSI "48;2;" + std::to_string(rgb.r) + ";" +
                                           std::to_string(rgb.g) + ";" +
                                           std::to_string(rgb.b) + "m";

                column.global_styles += back_rgb_stt;

                return *this;
            }
        };

        class Getters {
            Column& column;

        public:
            Getters(Column& column) : column(column) {}

            Alignment alignment() { return column.alignment; }

            unsigned int width() { return column.width; }

            unsigned int top_padding() { return column.top_padding; }

            unsigned int bottom_padding() { return column.bottom_padding; }

            unsigned int special_characters() { return column.special_characters; }

            std::vector<std::string> splitted_content() { return column.splitted_content; }

            std::list<std::string> words() { return column.words; }

            std::string global_styles() { return column.global_styles; }
        };

        class Setters {
            Column& column;

        public:
            Setters(Column& column) : column(column) {}

            Setters& splitted_content(std::vector<std::string> splittedContent) {
                column.splitted_content = splittedContent;
                return *this;
            }

            Setters& words(std::list<std::string> words) {
                column.words = words;
                return *this;
            }

            Setters& width(int width) {
                if (width <= 0)
                    width = 0;

                column.width = static_cast<unsigned int>(width);

                return *this;
            }
        };

    public:
        std::string content;

        Column(std::string content)
            : content(content), alignment(Alignment::left), width(0), top_padding(0), bottom_padding(0), special_characters(0) {};

        Config config() { return Config(*this); }

        Getters get() { return Getters(*this); }

        Setters set() { return Setters(*this); }
    };

    class Row {
        Alignment alignment;

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

            Config& color(Color c) {
                for (Column& col : row.columns)
                    col.config().color(c);

                return *this;
            }

            Config& background_color(BackgroundColor back_color) {
                for (Column& col : row.columns)
                    col.config().background_color(back_color);

                return *this;
            }
        };

    public:
        std::vector<Column> columns;

        Row(std::vector<Column> columns)
            : columns(columns), alignment(Alignment::left) {}

        Config config() { return Config(*this); }

        int get_columns_number() { return columns.size(); }

        unsigned int get_full_row_width() {
            unsigned int width = columns.size() + 1;

            for (Column column : columns)
                width += column.get().width();

            return width;
        }

        Column& operator[](int index) {
            if (index >= this->columns.size() || index < 0)
                throw std::out_of_range("Index out of bounds");

            return this->columns[index];
        }
    };

    namespace maps {
        static Border get_border_template(BorderStyle borderStyle) {
            static std::map<BorderStyle, Border> templates{
                {BorderStyle::empty, {" ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "}},
                {BorderStyle::standard, {"|", "-", "+", "+", "+", "+", "+", "+", "+", "+", "+", "+"}},
                {BorderStyle::ANSI, {
                                        TABLE_MODE "x" RESET_TABLE_MODE, // vertical
                                        TABLE_MODE "q" RESET_TABLE_MODE, // horizontal
                                        TABLE_MODE "n" RESET_TABLE_MODE, // corner
                                        TABLE_MODE "j" RESET_TABLE_MODE, // bottom_right_corner
                                        TABLE_MODE "k" RESET_TABLE_MODE, // top_right_corner
                                        TABLE_MODE "l" RESET_TABLE_MODE, // top_left_corner
                                        TABLE_MODE "m" RESET_TABLE_MODE, // bottom_left_corner
                                        TABLE_MODE "n" RESET_TABLE_MODE, // middle_separator
                                        TABLE_MODE "t" RESET_TABLE_MODE, // middle_left_to_right
                                        TABLE_MODE "u" RESET_TABLE_MODE, // middle_right_to_left
                                        TABLE_MODE "v" RESET_TABLE_MODE, // middle_bottom_to_top
                                        TABLE_MODE "w" RESET_TABLE_MODE  // middle_top_to_bottom
                                    }}};
            return templates[borderStyle];
        }

        static ResetStyle get_style_reset(Style style) {
            static std::map<Style, ResetStyle> reset{
                {Style::bold, ResetStyle::bold},
                {Style::dim, ResetStyle::dim},
                {Style::italic, ResetStyle::italic},
                {Style::underline, ResetStyle::underline},
                {Style::double_underline, ResetStyle::double_underline},
                {Style::blink, ResetStyle::blink},
                {Style::fast_blink, ResetStyle::fast_blink},
                {Style::reverse, ResetStyle::reverse},
                {Style::concealed, ResetStyle::concealed},
                {Style::crossed, ResetStyle::crossed}};

            return reset[style];
        }
    }; // namespace maps

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

    } // namespace utils

    class Table {
        BorderStyle border_style;
        Border border_templates;
        unsigned int width; // for width we check if it is bigger than the terminal width so no problem
        bool forced_width;
        bool force_ansi;

        class Config {
            Table& table;

            bool validate_params(int column_index, const Range& range) const {
                if (column_index < 0 || range.from < 0 || range.to < 0)
                    return false;

                return true;
            }

            Range normalize_range(Range range) {
                Range result = range;
                size_t rowsSize = table.rows.size();

                if (result.to >= rowsSize)
                    result.to = rowsSize - 1;

                return result;
            }

        public:
            Config(Table& table) : table(table) {}

            /* -----------------ALIGNMENT--------------------- */

            Config& alignment(Alignment align, int column_index, Range range) {
                if (!validate_params(column_index, range))
                    return *this;

                range = normalize_range(range);

                for (int i = range.from; i <= range.to; i++) {
                    Row& row = table.rows[i];
                    row.columns[column_index].config().alignment(align);
                }

                return *this; // done (true)
            }

            Config& alignment(Alignment align, int column_index) {
                return alignment(align, column_index,
                                 Range(0, table.rows.size() - 1));
            }

            Config& alignment(Alignment align) {
                for (Row& row : table.rows)
                    row.config().alignment(align);

                return *this;
            }

            /* -----------------PADDING--------------------- */

            Config& padding(int pad, int column_index, Range range) {
                if (pad < 0)
                    return *this; // Invalid padding

                if (!validate_params(column_index, range))
                    return *this;

                range = normalize_range(range);

                for (int i = range.from; i <= range.to; i++) {
                    Row& row = table.rows[i];
                    row.columns[column_index].config().padding(pad);
                }

                return *this;
            }

            Config& padding(int pad, int column_index) {
                return padding(pad, column_index, Range(0, table.rows.size() - 1));
            }

            Config& padding(int pad) {
                if (pad < 0)
                    return *this; // Invalid padding

                for (Row& row : table.rows)
                    row.config().padding(pad);

                return *this;
            }

            /* -----------------TOP_PADDING--------------------- */

            Config& top_padding(int pad, int column_index, Range range) {
                if (pad < 0)
                    return *this; // Invalid padding

                if (!validate_params(column_index, range))
                    return *this;

                range = normalize_range(range);

                for (int i = range.from; i <= range.to; i++) {
                    Row& row = table.rows[i];
                    row.columns[column_index].config().top_padding(pad);
                }

                return *this;
            }

            Config& top_padding(int pad, int column_index) {
                return top_padding(pad, column_index, Range(0, table.rows.size() - 1));
            }

            Config& top_padding(int pad) {
                if (pad < 0)
                    return *this; // Invalid padding

                for (Row& row : table.rows)
                    row.config().top_padding(pad);

                return *this;
            }

            /* -----------------BOTTOM_PADDING--------------------- */

            Config& bottom_padding(int pad, int column_index, Range range) {
                if (pad < 0)
                    return *this; // Invalid padding

                if (!validate_params(column_index, range))
                    return *this;

                range = normalize_range(range);

                for (int i = range.from; i <= range.to; i++) {
                    Row& row = table.rows[i];
                    row.columns[column_index].config().bottom_padding(pad);
                }

                return *this;
            }

            Config& bottom_padding(int pad, int column_index) {
                return bottom_padding(pad, column_index, Range(0, table.rows.size() - 1));
            }

            Config& bottom_padding(int pad) {
                if (pad < 0)
                    return *this; // Invalid padding

                for (Row& row : table.rows)
                    row.config().bottom_padding(pad);

                return *this;
            }

            /* -----------------WIDTH--------------------- */

            Config& table_width(int width) {
                if (width <= 0)
                    return *this; // Invalid width

                table.width = width;

                return *this;
            }

            Config& columns_width(int width, int column_index, Range range) {
                if (width < 0)
                    return *this; // Invalid width

                if (!validate_params(column_index, range))
                    return *this;

                range = normalize_range(range);

                // Check regularity
                if (!table.is_regular(range))
                    return *this;

                for (int i = range.from; i <= range.to; i++) {
                    Row& row = table.rows[i];
                    row.columns[column_index].config().width(width);
                }

                return *this;
            }

            Config& columns_width(int width, int column_index) {
                return columns_width(width, column_index, Range(0, table.rows.size() - 1));
            }

            /* -------------------Colors------------------------- */

            Config& color(Color c, int column_index, Range range) {
                if (!validate_params(column_index, range))
                    return *this;

                range = normalize_range(range);

                for (int i = range.from; i <= range.to; i++) {
                    Row& row = table.rows[i];
                    row[column_index].config().color(c);
                }

                return *this;
            }

            Config& color(Color c, int column_index) {
                return color(c, column_index, Range(0, table.rows.size() - 1));
            }

            Config& color(Color c) {
                for (Row& row : table.rows)
                    row.config().color(c);

                return *this;
            }

            /* ---------------Background Colors--------------------- */

            Config& background_color(BackgroundColor back_color, int column_index, Range range) {
                if (!validate_params(column_index, range))
                    return *this;

                range = normalize_range(range);

                for (int i = range.from; i <= range.to; i++) {
                    Row& row = table.rows[i];
                    row[column_index].config().background_color(back_color);
                }

                return *this;
            }

            Config& background_color(BackgroundColor back_color, int column_index) {
                return background_color(back_color, column_index, Range(0, table.rows.size() - 1));
            }

            Config& background_color(BackgroundColor back_color) {
                for (Row& row : table.rows)
                    row.config().background_color(back_color);

                return *this;
            }

            /* -----------------BORDER PARTS--------------------- */

            Config& corner(char corner) {
                table.border_templates.corner = corner;
                return *this;
            }

            Config& horizontal(char horizontal) {
                table.border_templates.horizontal = horizontal;
                return *this;
            }

            Config& vertical(char vertical) {
                table.border_templates.vertical = vertical;
                return *this;
            }

            Config& bottom_right_corner(char corner) {
                table.border_templates.bottom_right_corner = corner;
                return *this;
            }

            Config& top_right_corner(char corner) {
                table.border_templates.top_right_corner = corner;
                return *this;
            }

            Config& top_left_corner(char corner) {
                table.border_templates.top_left_corner = corner;
                return *this;
            }

            Config& bottom_left_corner(char corner) {
                table.border_templates.bottom_left_corner = corner;
                return *this;
            }

            Config& middle_separator(char separator) {
                table.border_templates.middle_separator = separator;
                return *this;
            }

            Config& middle_left_to_right(char connector) {
                table.border_templates.left_to_right = connector;
                return *this;
            }

            Config& middle_right_to_left(char connector) {
                table.border_templates.right_to_left = connector;
                return *this;
            }

            Config& middle_bottom_to_top(char connector) {
                table.border_templates.bottom_to_top = connector;
                return *this;
            }

            Config& middle_top_to_bottom(char connector) {
                table.border_templates.top_to_bottom = connector;
                return *this;
            }

            Config& border(BorderStyle style) {
                table.border_style = style;
                return *this;
            }
        };

        class Setters {
            Table& table;

        public:
            Setters(Table& table) : table(table) {}

            Setters& width(int width) {
                if (width > 0)
                    table.width = static_cast<unsigned int>(width);
                else
                    table.width = 0;

                return *this;
            }

            // * for testing set a fixed, forced width
            Setters& forced_width(int width, bool forced) {
                if (width < 0)
                    table.width = 0;
                else
                    table.width = static_cast<unsigned int>(width);

                table.forced_width = forced;

                return *this;
            }

            // * for testing force using ansi (used when redirecting output to files)
            Setters& forced_ansi(bool forced) {
                table.border_style = BorderStyle::ANSI;

                table.force_ansi = forced;

                return *this;
            }
        };

        std::vector<int> find_stops(Row row) {
            std::vector<int> result;

            int track = 1;
            for (Column column : row.columns) {
                track += column.get().width();

                result.push_back(int(track));
                track++;
            }

            return result;
        }

        void print_row(std::ostream& stream, Row& row, int width_reference) {
            if (width_reference != 0) {
                // tableSplits = (row.columns.size() + 1)
                unsigned int usable_width = width_reference - (row.columns.size() + 1);
                utils::format_row(usable_width, row);
            }

            std::string vertical = border_templates.vertical;

            size_t max_splitted_content_size = utils::find_max_splitted_content_size(row); // tallest vector of splitted strings
            for (unsigned int i = 0; i < max_splitted_content_size; i++) {
                stream << '\n'
                       << vertical;

                for (Column column : row.columns) {
                    int rest = column.get().width();
                    int splitted_content_size = column.get().splitted_content().size();
                    std::string current_line;

                    if (i < splitted_content_size) {
                        current_line = column.get().splitted_content().at(i);
                        stream << current_line;

                        int special_characters = column.get().special_characters();
                        std::string global_styles = column.get().global_styles();

                        // for each splitted_content element has a one/more global_styles it has one RESET at the end
                        if (!global_styles.empty()) {
                            std::string reset = RESET;
                            special_characters += reset.size();
                            special_characters += global_styles.size();
                        }
                        rest -= current_line.size() - special_characters; // to balance the line
                    }

                    for (int k = 0; k < rest; k++)
                        stream << ' ';

                    stream << vertical;
                }
            }
        }

        void print_border(std::ostream& stream, std::vector<Row>::iterator& it, bool is_first, bool is_last, bool regular) {

            Row reference = *it;
            std::vector<int> next_row_corners;

            if (is_first)
                next_row_corners = find_stops(*it);
            else if (!is_last)
                next_row_corners = find_stops(*(it + 1));

            if (!is_first)
                stream << '\n';

            std::string horizontal = border_templates.horizontal;

            // (vertical separators)/corners
            std::string left_corner;
            std::string right_corner;

            // horizontal separator
            std::string middle_separator;
            std::string top_to_bottom;
            std::string bottom_to_top;

            if (is_first) {
                left_corner = border_templates.top_left_corner;
                right_corner = border_templates.top_right_corner;

                middle_separator = border_templates.top_to_bottom;
                top_to_bottom = border_templates.top_to_bottom;
                bottom_to_top = border_templates.top_to_bottom;
            } else if (is_last) {
                left_corner = border_templates.bottom_left_corner;
                right_corner = border_templates.bottom_right_corner;

                middle_separator = border_templates.bottom_to_top;
                top_to_bottom = border_templates.bottom_to_top;
                bottom_to_top = border_templates.bottom_to_top;
            } else {
                left_corner = border_templates.left_to_right;
                right_corner = border_templates.right_to_left;

                middle_separator = border_templates.middle_separator;
                top_to_bottom = border_templates.top_to_bottom;
                bottom_to_top = border_templates.bottom_to_top;
            }

            stream << left_corner;

            size_t cols_num = reference.columns.size();

            int tracker = 0;
            for (size_t j = 0; j < cols_num; j++) {
                Column column = reference.columns[j];
                unsigned col_width = column.get().width();

                // tracker++;

                for (unsigned int k = 0; k < col_width; k++) {
                    tracker++;

                    if (std::find(next_row_corners.begin(), next_row_corners.end(), tracker) != next_row_corners.end())
                        stream << top_to_bottom;
                    else
                        stream << horizontal;
                }

                tracker++;

                if (j + 1 != cols_num) {

                    // column separator at the bottom and at the top
                    if (std::find(next_row_corners.begin(), next_row_corners.end(), tracker) != next_row_corners.end())
                        stream << middle_separator;
                    else
                        stream << bottom_to_top; // just at the top
                }
            }

            stream << right_corner;
        }

        unsigned int find_max_rows_width() {
            unsigned int result = 0;
            for (Row row : rows) {
                unsigned int row_width = row.get_full_row_width();
                if (row_width > result) result = row_width;
            }

            return result;
        }

    public:
        std::vector<Row> rows;

        Table() : border_style(BorderStyle::standard), width(0), forced_width(false) {}

        // configure the table
        Config config() { return Config(*this); }

        Setters set() { return Setters(*this); }

        unsigned int get_width() { return width; }

        void add_row(std::vector<std::string> contents) {
            std::vector<Column> columns;
            for (std::string content : contents)
                columns.push_back(Column(content));

            rows.push_back(Row(columns));
        }

        /* Regularity means it has the same number of columns in each row */
        bool is_regular(Range range) {
            size_t reference = rows[range.from].columns.size();

            for (int i = range.from + 1; i <= range.to; i++)
                if (rows[i].columns.size() != reference)
                    return false;

            return true;
        }

        bool is_regular() {
            return is_regular(Range(0, rows.size() - 1));
        }

        // * return 2 for empty rows and 3 for terminal space problems
        int print(std::ostream& stream) {
            if (rows.size() == 0)
                return 2;

            // chose width to use
            unsigned int usable_width;
            if (forced_width)
                usable_width = width;
            else {
                unsigned short terminal_width = utils::get_terminal_width();
                usable_width = terminal_width * DEFAULT_WIDTH_PERCENT;
                if (this->width <= 0 || this->width > terminal_width)
                    width = usable_width;
                else
                    usable_width = width;
            }

            size_t cols_num;
            size_t row_usable_width;
            // edit rows to match the width
            for (Row& row : rows) {
                cols_num = row.columns.size();
                row_usable_width = usable_width - (cols_num + 1); // ... - tableSplits

                if (row_usable_width <= cols_num)
                    return 3;

                utils::format_row(row_usable_width, row);
            }

            // check if the table has consistent number of columns across all rows
            bool regular = is_regular();

            if (border_style == BorderStyle::ANSI)
                if (!utils::check_terminal() && !force_ansi)
                    border_style = BorderStyle::standard;

            Border templates = border_templates;
            border_templates = maps::get_border_template(border_style);

            if (!templates.corner.empty()) border_templates.corner = templates.corner;
            if (!templates.horizontal.empty()) border_templates.horizontal = templates.horizontal;
            if (!templates.vertical.empty()) border_templates.vertical = templates.vertical;
            if (!templates.top_left_corner.empty()) border_templates.top_left_corner = templates.top_left_corner;
            if (!templates.top_right_corner.empty()) border_templates.top_right_corner = templates.top_right_corner;
            if (!templates.bottom_left_corner.empty()) border_templates.bottom_left_corner = templates.bottom_left_corner;
            if (!templates.bottom_right_corner.empty()) border_templates.bottom_right_corner = templates.bottom_right_corner;
            if (!templates.middle_separator.empty()) border_templates.middle_separator = templates.middle_separator;
            if (!templates.left_to_right.empty()) border_templates.left_to_right = templates.left_to_right;
            if (!templates.right_to_left.empty()) border_templates.right_to_left = templates.right_to_left;
            if (!templates.top_to_bottom.empty()) border_templates.top_to_bottom = templates.top_to_bottom;
            if (!templates.bottom_to_top.empty()) border_templates.bottom_to_top = templates.bottom_to_top;

            /* ------ printing the table ------- */

            // 0 to check in print_row
            unsigned int row_width_reference = regular ? 0 : find_max_rows_width();

            if (!regular)
                for (auto it = rows.begin(); it != rows.end(); ++it)
                    utils::format_row(row_width_reference - (it->columns.size() + 1), *it);

            bool is_first = true, is_last = false;
            if (rows.size() == 1)
                is_last = true;

            auto it = rows.begin();
            print_border(stream, it, is_first, is_last, regular);

            is_first = false;
            for (it = rows.begin(); it != rows.end(); ++it) {
                Row& row = *it;

                print_row(stream, row, row_width_reference);

                if ((it + 1) == rows.end())
                    is_last = true;

                print_border(stream, it, is_first, is_last, regular);
            }

            return 0;
        }

        Row& operator[](int index) {
            if (index >= this->rows.size() || index < 0)
                throw std::out_of_range("Index out of bounds");

            return this->rows[index];
        }
    };

    inline std::ostream& operator<<(std::ostream& stream, const Table& table) {
        const_cast<Table&>(table).print(stream);
        return stream;
    }
} // namespace tabular
