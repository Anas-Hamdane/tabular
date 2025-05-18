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
      -  [ ] terminal colors and highlights support
      -  [x] terminal font styles support
      -  [x] padding control
      -  [x] width control
      -  [x] range columns setters (functions)

    ! NOTE: add more border control by editing the Border struct to be like the ANSI, switch to chars for better performance, and maybe move configure to namespace style, merge tests in one test with all the cases and better handling.
*/

#include <algorithm>
#include <ostream>
#include <vector>

// headers
#include <tabular/column.hpp>
#include <tabular/row.hpp>

#include <tabular/style.hpp>
#include <tabular/utils.hpp>

#include <tabular/definitions.hpp>
#include <tabular/global_vars.hpp>

namespace tabular {
    class Table {
        BorderStyle border_style;
        style::Border border_templates;
        unsigned int width; // for width we check if it is bigger than the terminal width so no problem
        bool forced_width;
        bool force_ansi;

        class Config {
            Table& table;

            /*
                ! ERROR CODES:
                    * 1: true done
                    * 2: error code for "cols_index" problems
                    * 3: error code for "range.from" problems
                    * 4: error code for "range.to" problems
                    * 5: error code for "padding"'s problems
                    * 6: error code for "width" problems
                    * 7: error code for regularity problems
            */

            int validate_params(int column_index, const Range& range) const {
                if (column_index < 0)
                    return 2; // Column index error

                if (range.from < 0)
                    return 3; // Range start error

                if (range.to < 0)
                    return 4; // Range end error

                return 1;
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

            int alignment(Alignment align, int column_index, Range range) {
                int validation_code = validate_params(column_index, range);
                if (validation_code != 1)
                    return validation_code;

                range = normalize_range(range);

                for (int i = range.from; i <= range.to; i++) {
                    Row& row = table.rows[i];
                    row.columns[column_index].config().alignment(align);
                }

                return 1; // done (true)
            }

            int alignment(Alignment align, int column_index) {
                return alignment(align, column_index,
                                 Range(0, table.rows.size() - 1));
            }

            int alignment(Alignment align) {
                for (Row& row : table.rows)
                    row.config().alignment(align);

                return 1;
            }

            /* -----------------PADDING--------------------- */

            int padding(int pad, int column_index, Range range) {
                if (pad < 0)
                    return 5; // Invalid padding

                int validation = validate_params(column_index, range);
                if (validation != 1)
                    return validation;

                range = normalize_range(range);

                for (int i = range.from; i <= range.to; i++) {
                    Row& row = table.rows[i];
                    row.columns[column_index].config().padding(pad);
                }

                return 1;
            }

            int padding(int pad, int column_index) {
                return padding(pad, column_index, Range(0, table.rows.size() - 1));
            }

            int padding(int pad) {
                if (pad < 0)
                    return 5; // Invalid padding

                for (Row& row : table.rows)
                    row.config().padding(pad);

                return 1;
            }

            /* -----------------TOP_PADDING--------------------- */

            int top_padding(int pad, int column_index, Range range) {
                if (pad < 0)
                    return 5; // Invalid padding

                int validation = validate_params(column_index, range);
                if (validation != 1)
                    return validation;

                range = normalize_range(range);

                for (int i = range.from; i <= range.to; i++) {
                    Row& row = table.rows[i];
                    row.columns[column_index].config().top_padding(pad);
                }

                return 1;
            }

            int top_padding(int pad, int column_index) {
                return top_padding(pad, column_index, Range(0, table.rows.size() - 1));
            }

            int top_padding(int pad) {
                if (pad < 0)
                    return 5; // Invalid padding

                for (Row& row : table.rows)
                    row.config().top_padding(pad);

                return 1;
            }

            /* -----------------BOTTOM_PADDING--------------------- */

            int bottom_padding(int pad, int column_index, Range range) {
                if (pad < 0)
                    return 5; // Invalid padding

                int validation = validate_params(column_index, range);
                if (validation != 1)
                    return validation;

                range = normalize_range(range);

                for (int i = range.from; i <= range.to; i++) {
                    Row& row = table.rows[i];
                    row.columns[column_index].config().bottom_padding(pad);
                }

                return 1;
            }

            int bottom_padding(int pad, int column_index) {
                return bottom_padding(pad, column_index, Range(0, table.rows.size() - 1));
            }

            int bottom_padding(int pad) {
                if (pad < 0)
                    return 5; // Invalid padding

                for (Row& row : table.rows)
                    row.config().bottom_padding(pad);

                return 1;
            }

            /* -----------------WIDTH--------------------- */

            int table_width(int width) {
                if (width <= 0)
                    return 6; // Invalid width

                table.width = width;

                return 1;
            }

            int columns_width(int width, int column_index, Range range) {
                if (width < 0)
                    return 6; // Invalid width

                int validation = validate_params(column_index, range);
                if (validation != 1)
                    return validation;

                range = normalize_range(range);

                // Check regularity
                if (!table.is_regular(range))
                    return 7;

                for (int i = range.from; i <= range.to; i++) {
                    Row& row = table.rows[i];
                    row.columns[column_index].config().width(width);
                }

                return 1;
            }

            int columns_width(int width, int column_index) {
                return columns_width(width, column_index, Range(0, table.rows.size() - 1));
            }
        };

        class Format {
            Table& table;

            class Fonts {
                Table& table;

                int validate_params(int column_index, const Range& range) const {
                    if (column_index < 0)
                        return 2; // Column index error

                    if (range.from < 0)
                        return 3; // Range start error

                    if (range.to < 0)
                        return 4; // Range end error

                    return 1;
                }

                Range normalize_range(Range range) {
                    Range result = range;
                    size_t rowsSize = table.rows.size();

                    if (result.to >= rowsSize)
                        result.to = rowsSize - 1;

                    return result;
                }

            public:
                Fonts(Table& table) : table(table) {}

                /* -----------------ADD FONT STYLES--------------------- */

                int add(FontStylesVector styles, int column_index, Range range) {
                    int validation = validate_params(column_index, range);
                    if (validation != 1)
                        return validation;

                    range = normalize_range(range);

                    for (int i = range.from; i <= range.to; i++) {
                        Row& row = table.rows[i];
                        row.columns[column_index].config().fonts().add(styles);
                    }

                    return 1; // done (true)
                }

                int add(FontStylesVector styles, int column_index) {
                    return add(styles, column_index,
                               Range(0, table.rows.size() - 1));
                }

                void add(FontStylesVector styles) {
                    for (Row& row : table.rows)
                        row.config().fonts().add(styles);
                }

                /* -----------------REMOVE FONT STYLES--------------------- */

                int remove(const FontStylesVector& styles, int column_index, Range range) {
                    int validation = validate_params(column_index, range);
                    if (validation != 1)
                        return validation;

                    range = normalize_range(range);

                    for (int i = range.from; i <= range.to; i++) {
                        Row& row = table.rows[i];
                        row.columns[column_index].config().fonts().remove(styles);
                    }

                    return 1;
                }

                int remove(const FontStylesVector& styles, int column_index) {
                    return remove(styles, column_index, Range(0, table.rows.size() - 1));
                }

                int remove(const FontStylesVector& styles) {
                    for (Row& row : table.rows)
                        row.config().fonts().remove(styles);

                    return 1;
                }
            };

        public:
            Format(Table& table) : table(table) {
            }

            Fonts fonts() { return Fonts(table); }

            Format& corner(char corner) {
                table.border_templates.corner = corner;
                return *this;
            }

            Format& horizontal(char horizontal) {
                table.border_templates.horizontal = horizontal;
                return *this;
            }

            Format& vertical(char vertical) {
                table.border_templates.vertical = vertical;
                return *this;
            }

            Format& bottom_right_corner(char corner) {
                table.border_templates.bottom_right_corner = corner;
                return *this;
            }

            Format& top_right_corner(char corner) {
                table.border_templates.top_right_corner = corner;
                return *this;
            }

            Format& top_left_corner(char corner) {
                table.border_templates.top_left_corner = corner;
                return *this;
            }

            Format& bottom_left_corner(char corner) {
                table.border_templates.bottom_left_corner = corner;
                return *this;
            }

            Format& middle_separator(char separator) {
                table.border_templates.middle_separator = separator;
                return *this;
            }

            Format& middle_left_to_right(char connector) {
                table.border_templates.left_to_right = connector;
                return *this;
            }

            Format& middle_right_to_left(char connector) {
                table.border_templates.right_to_left = connector;
                return *this;
            }

            Format& middle_bottom_to_top(char connector) {
                table.border_templates.bottom_to_top = connector;
                return *this;
            }

            Format& middle_top_to_bottom(char connector) {
                table.border_templates.top_to_bottom = connector;
                return *this;
            }

            void border(BorderStyle style) { table.border_style = style; }
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

                        int special_characters = 0;
                        FontStylesVector font_styles = column.get().font_styles();
                        if (!font_styles.empty())
                            special_characters = 4 + (font_styles.size() * 4); // 4 fixed for RESET extra characters and (font_styles.size() * 4) = (font_styles.size() * 2) + (font_styles.size() * 2), means 2 characters for each font_style + 2 characters fir CSI for each font_style

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

        Format format() { return Format(*this); }

        Setters set() { return Setters(*this); }

        unsigned int get_width() { return width; }

        void add_row(std::vector<std::string> contents) {
            std::vector<Column> columns;
            for (std::string content : contents)
                columns.push_back(Column(content));

            rows.push_back(Row(columns));
        }

        /* Regularity means it has the same number of columns in each rows */
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

            style::Border templates = border_templates;
            border_templates = style::get_border_template(border_style);

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