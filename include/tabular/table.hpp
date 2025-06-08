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
      -  [x] support for multi byte characters (automatic and manual)
      -  [x] full column background color support
      -  [ ] border styling (colors, rgb)
      -  [ ] sub string styling support [VEEEEEEEEEEEEERY HAAAAAAAAAAARD]
*/

#include <algorithm>
#include <ostream>
#include <vector>

// those include all headers
#include <tabular/utils.hpp>

namespace tabular {
    class Table {
        BorderStyle border_style;
        Border border_templates;
        unsigned int width;
        bool forced_width;
        bool force_ansi;

        class Config {
            Table& table;

        public:
            Config(Table& table) : table(table) {}

            /* -----------------ALIGNMENT--------------------- */
            Config& alignment(Alignment align, int column) {
                if (column < 0)
                    return *this;

                for (Row& row : table.rows)
                    if (column < row.columns.size())
                        row[column].config().alignment(align);

                return *this;
            }

            Config& alignment(Alignment align) {
                for (Row& row : table.rows)
                    row.config().alignment(align);

                return *this;
            }

            /* -----------------PADDING--------------------- */
            Config& padding(int pad, int column) {
                if (pad < 0 || column < 0)
                    return *this;

                for (Row& row : table.rows)
                    if (column < row.columns.size())
                        row[column].config().padding(pad);

                return *this;
            }

            Config& padding(int pad) {
                if (pad < 0)
                    return *this; // Invalid padding

                for (Row& row : table.rows)
                    row.config().padding(pad);

                return *this;
            }

            /* -----------------TOP_PADDING--------------------- */
            Config& top_padding(int pad, int column) {
                if (pad < 0 || column < 0)
                    return *this;

                for (Row& row : table.rows)
                    if (column < row.columns.size())
                        row[column].config().top_padding(pad);

                return *this;
            }

            Config& top_padding(int pad) {
                if (pad < 0)
                    return *this; // Invalid padding

                for (Row& row : table.rows)
                    row.config().top_padding(pad);

                return *this;
            }

            /* -----------------BOTTOM_PADDING--------------------- */
            Config& bottom_padding(int pad, int column) {
                if (pad < 0 || column < 0)
                    return *this;

                for (Row& row : table.rows)
                    if (column < row.columns.size())
                        row[column].config().bottom_padding(pad);

                return *this;
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

            Config& columns_width(int width, int column) {
                if (width < 0 || column < 0)
                    return *this;

                for (Row& row : table.rows)
                    if (column < row.columns.size())
                        row[column].config().width(width);

                return *this;
            }

            /* -------------------COLORS------------------------- */
            Config& color(Color col, int column) {
                if (column < 0)
                    return *this;

                for (Row& row : table.rows)
                    if (column < row.columns.size())
                        row[column].config().color(col);

                return *this;
            }

            Config& color(Color color) {
                for (Row& row : table.rows)
                    row.config().color(color);

                return *this;
            }

            /* ---------------BACKGROUND COLORS--------------------- */
            Config& content_background_color(BackgroundColor back_color, int column) {
                if (column < 0)
                    return *this;

                for (Row& row : table.rows)
                    if (column < row.columns.size())
                        row[column].config().content_background_color(back_color);

                return *this;
            }

            Config& content_background_color(BackgroundColor back_color) {
                for (Row& row : table.rows)
                    row.config().column_background_color(back_color);

                return *this;
            }

            /* -------------------RGB------------------------- */
            Config& rgb(RGB rgb, int column) {
                if (column < 0)
                    return *this;

                for (Row& row : table.rows)
                    if (column < row.columns.size())
                        row[column].config().rgb(rgb);

                return *this;
            }

            Config& rgb(RGB rgb) {
                for (Row& row : table.rows)
                    row.config().rgb(rgb);

                return *this;
            }

            /* ---------------BACKGROUND RGB--------------------- */
            Config& content_background_rgb(RGB back_rgb, int column) {
                if (column < 0)
                    return *this;

                for (Row& row : table.rows)
                    if (column < row.columns.size())
                        row[column].config().content_background_rgb(back_rgb);

                return *this;
            }

            Config& content_background_rgb(RGB back_rgb) {
                for (Row& row : table.rows)
                    row.config().column_background_rgb(back_rgb);

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

        void print_row(std::ostream& stream, Row& row) {
            std::string vertical = border_templates.vertical;

            size_t max_splitted_content_size = utils::find_max_splitted_content_size(row); // tallest vector of splitted strings
            for (unsigned int i = 0; i < max_splitted_content_size; i++) {
                // side border
                stream << '\n'
                       << vertical;

                // printing the n element of splitted_content for each column
                for (Column column : row.columns) {
                    std::string reset = RESET;

                    int rest = column.get().width();
                    int splitted_content_size = column.get().splitted_content().size();
                    std::string current_line;

                    int special_characters = 0;

                    // column background
                    std::string column_background_color = column.get().column_background_color();
                    stream << column_background_color;

                    if (i < splitted_content_size) {
                        current_line = column.get().splitted_content().at(i);
                        stream << current_line;

                        std::string styles = column.get().text_styles() + column.get().content_color() + column.get().content_background_color();

                        // for each splitted_content element has a one or more global_styles, it has one RESET at the end
                        if (!styles.empty()) {
                            special_characters += reset.size();
                            special_characters += styles.size();
                        }

                        size_t curr_line_size;

                        if (column.get().is_multi_byte())
                            curr_line_size = utils::mbswidth(current_line);
                        else
                            curr_line_size = current_line.size();

                        // special_characters will not be displayed so they are not counted
                        rest -= curr_line_size - special_characters; // to balance the line
                    }

                        stream << column_background_color;

                    for (int k = 0; k < rest; k++)
                        stream << ' ';

                    if (!column_background_color.empty())
                        stream << reset;

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

        inline void adjust_widths(unsigned int table_width) {
            if (table_width <= 0)
                return;

            for (Row& row : this->rows) {
                size_t columns_num = row.columns.size();
                if (columns_num == 0)
                    continue;

                unsigned int table_usable_width = table_width - (columns_num + 1);

                for (Column& column : row.columns) {
                    if (column.get().width() != 0) {
                        table_usable_width -= column.get().width();
                        columns_num--;
                    }
                }

                int indiv_column_width = table_usable_width / columns_num;
                int rest = table_usable_width % columns_num;

                for (Column& column : row.columns) {
                    unsigned int width = column.get().width();

                    if (width == 0) {
                        width = indiv_column_width + (rest > 0 ? 1 : 0);
                        if (rest > 0) rest--;
                        column.set().width(width);
                    }

                    /* no need because it is the work of format_row() :) */
                    // column.set().splitted_content(utils::format_column_lines(column, width));
                }
            }
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
        bool is_regular() {
            size_t reference = rows[0].columns.size();
            size_t rows_num = rows.size();

            for (int i = 0; i < rows_num; i++)
                if (rows[i].columns.size() != reference)
                    return false;

            return true;
        }

        // * return 2 for empty rows and 3 for terminal space problems
        int print(std::ostream& stream) {
            if (rows.empty())
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

            // check if the table has consistent number of columns across all rows
            bool regular = is_regular();

            // adjust the width of all columns once
            adjust_widths(usable_width);

            for (Row& row : rows) {
                size_t cols_num = row.columns.size();
                size_t row_usable_width = usable_width - (cols_num + 1); // ... - tableSplits

                if (row_usable_width <= cols_num)
                    return 3;

                utils::format_row(row_usable_width, row);
            }

            if (border_style == BorderStyle::ANSI && !utils::check_terminal() && !force_ansi)
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
            bool is_first = true, is_last = false;
            if (rows.size() == 1)
                is_last = true;

            auto it = rows.begin();
            print_border(stream, it, is_first, is_last, regular);

            is_first = false;
            for (it = rows.begin(); it != rows.end(); ++it) {
                Row& row = *it;

                print_row(stream, row);

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
