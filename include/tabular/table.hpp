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
      -  [x] padding control
      -  [x] width control
      -  [x] range columns setters (functions)
*/

#include <iostream>
#include <algorithm>
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

        struct Setters {
            Table& table;
            Setters(Table& table) : table(table) {}

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

            /* -----------------ALIGNMENT--------------------- */

            // Set All Columns Alignments in the table
            void set_global_align(Alignment align) {
                for (Row& row : table.rows)
                    row.set_row_align(align);
            }

            // set the alignment of all cols in index "cols_index" from row "from" to "to"
            // note: "from" and "to" are indices
            int set_cols_align(Alignment align, int cols_index, Range range) {
                if (cols_index < 0)
                    return 2;

                if (range.from < 0)
                    return 3;

                if (range.to < 0)
                    return 4;

                size_t rows_size = table.rows.size();
                if (range.to >= rows_size)
                    range.to = rows_size - 1; // last element

                for (int i = range.from; i <= range.to; i++) {
                    Row& row = table.rows[i];
                    row.columns[cols_index].set_column_align(align);
                }

                return 1; // done (true)
            }

            // set all the columns with "cols_index" to the "align"
            int set_all_cols_align(Alignment align, int cols_index) {
                return set_cols_align(align, cols_index,
                                      Range(0, table.rows.size() - 1));
            }

            /* -----------------PADDING--------------------- */

            // check for negative values
            int set_global_padding(int padding) {
                if (padding < 0)
                    return 5;

                for (Row& row : table.rows)
                    row.set_row_padding(padding);

                return 1; // done (true)
            }

            // set the padding of all cols in index "cols_index" from row in index "from" to "to"
            // note: "from" and "to" are indices
            int set_cols_padding(int padding, int cols_index, Range range) {
                if (cols_index < 0)
                    return 2;

                if (range.from < 0)
                    return 3;

                if (range.to < 0)
                    return 4;

                if (padding < 0)
                    return 5;

                size_t rows_size = table.rows.size();
                if (range.to >= rows_size)
                    range.to = rows_size - 1; // last element

                for (int i = range.from; i <= range.to; i++) {
                    Row& row = table.rows[i];
                    row.columns[cols_index].set_column_padding(padding);
                }

                return 1; // done (true)
            }

            // set all the columns with "cols_index" to "padding"
            int set_all_cols_padding(int padding, int colsIndex) {
                return set_cols_padding(padding, colsIndex,
                                        Range(0, table.rows.size() - 1));
            }

            /* -----------------TOP_PADDING--------------------- */

            // check for negative values
            int set_global_top_padding(int top_padding) {
                if (top_padding < 0)
                    return 5;

                for (Row& row : table.rows)
                    row.set_row_top_padding(top_padding);

                return 1; // done (true)
            }

            // set the top padding of all cols in index "cols_index" from row in index "from" to "to"
            // note: "from" and "to" are indices
            int set_cols_top_padding(int top_padding, int cols_index, Range range) {
                if (cols_index < 0)
                    return 2;

                if (range.from < 0)
                    return 3;

                if (range.to < 0)
                    return 4;

                if (top_padding < 0)
                    return 5;

                size_t rows_size = table.rows.size();
                if (range.to >= rows_size)
                    range.to = rows_size - 1; // last element

                for (int i = range.from; i <= range.to; i++) {
                    Row& row = table.rows[i];
                    row.columns[cols_index].set_column_top_padding(top_padding);
                }

                return 1; // done (true)
            }

            // set all the columns with "cols_index" to "top_padding"
            int set_all_cols_top_padding(int top_padding, int colsIndex) {
                return set_cols_top_padding(top_padding, colsIndex,
                                            Range(0, table.rows.size() - 1));
            }

            /* -----------------BOTTOM_PADDING--------------------- */

            // check for negative values
            int set_global_bottom_padding(int bottom_padding) {
                if (bottom_padding < 0)
                    return 5;

                for (Row& row : table.rows)
                    row.set_row_bottom_padding(bottom_padding);

                return 1; // done (true)
            }

            // set the bottom padding of all cols in index "cols_index" from row in index "from" to "to"
            // note: "from" and "to" are indices
            int set_cols_bottom_padding(int bottom_padding, int cols_index, Range range) {
                if (cols_index < 0)
                    return 2;

                if (range.from < 0)
                    return 3;

                if (range.to < 0)
                    return 4;

                if (bottom_padding < 0)
                    return 5;

                size_t rows_size = table.rows.size();
                if (range.to >= rows_size)
                    range.to = rows_size - 1; // last element

                for (int i = range.from; i <= range.to; i++) {
                    Row& row = table.rows[i];
                    row.columns[cols_index].set_column_bottom_padding(bottom_padding);
                }

                return 1; // done (true)
            }

            // set all the columns with "cols_index" to "bottom_padding"
            int set_all_cols_bottom_padding(int bottom_padding, int colsIndex) {
                return set_cols_bottom_padding(bottom_padding, colsIndex,
                                               Range(0, table.rows.size() - 1));
            }

            /* -----------------WIDTH--------------------- */

            int set_table_width(int width) {
                if (width <= 0)
                    return 6;

                table.width = width;

                return 1;
            }

            // set the width of all cols in index "cols_index" from row in index "from" to "to"
            // note: "from" and "to" are indices
            int set_cols_width(int width, int cols_index, Range range) {
                if (cols_index < 0)
                    return 2;

                if (range.from < 0)
                    return 3;

                if (range.to < 0)
                    return 4;

                if (width < 0)
                    return 6;

                size_t rows_size = table.rows.size();
                if (range.to >= rows_size)
                    range.to = rows_size - 1; // last element

                if (!table.is_regular(range))
                    return 7;

                for (int i = range.from; i <= range.to; i++) {
                    Row& row = table.rows[i];
                    row.columns[cols_index].set_width(width);
                }

                return 1; // done (true)
            }

            int set_all_cols_width(int width, int cols_index) {
                return set_cols_width(width, cols_index, Range(0, table.rows.size() - 1));
            }
        };

        struct Format {
            Table& table;
            Format(Table& table) : table(table) {}

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

            void border(BorderStyle style) { table.border_style = style; }
        };

        std::vector<int> find_stops(Row row) {
            std::vector<int> result;

            int track = 1;
            for (Column col : row.columns) {
                track += col.get_width();

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

            if (border_style == BorderStyle::ANSI)
                vertical = TABLE_MODE + std::string(1, static_cast<char>(ANSI_TABLE_PARTS::TABLE_VERTICAL)) + RESET_TABLE_MODE;

            size_t max_splitted_content_size = utils::find_max_splitted_content_size(row); // tallest vector of splitted strings
            for (unsigned int i = 0; i < max_splitted_content_size; i++) {
                stream << '\n'
                       << vertical;

                for (Column col : row.columns) {
                    int rest = col.get_width();
                    int splitted_content_size = col.get_splitted_content().size();
                    std::string current_line;

                    if (i < splitted_content_size) {
                        current_line = col.get_splitted_content().at(i);
                        stream << current_line;
                        rest -= current_line.size(); // to balance the line
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

            if (is_first) next_row_corners = find_stops(*it);
            else if (!is_last) next_row_corners = find_stops(*(it + 1));

            // (vertical separators)/corners
            std::string left_corner = border_templates.corner;
            std::string right_corner = border_templates.corner;

            std::string horizontal = border_templates.horizontal;

            // horizontal separator
            std::string hor_separator = border_templates.corner;
            std::string top_to_bottom = border_templates.corner;

            bool is_ANSI = (border_style == BorderStyle::ANSI);

            if (!is_first)
                stream << '\n';

            if (is_ANSI)
                horizontal = TABLE_MODE + std::string(1, static_cast<char>(ANSI_TABLE_PARTS::TABLE_HORIZONTAL)) + RESET_TABLE_MODE;

            if (is_first && is_ANSI) {
                left_corner = TABLE_MODE + std::string(1, static_cast<char>(ANSI_TABLE_PARTS::TABLE_TOP_LEFT_CORNER)) + RESET_TABLE_MODE;
                right_corner = TABLE_MODE + std::string(1, static_cast<char>(ANSI_TABLE_PARTS::TABLE_TOP_RIGHT_CORNER)) + RESET_TABLE_MODE;

                hor_separator = TABLE_MODE + std::string(1, static_cast<char>(ANSI_TABLE_PARTS::TABLE_MIDDLE_TOP_TO_BOTTOM)) + RESET_TABLE_MODE;
            }

            else if (!(is_first || is_last) && is_ANSI) {
                left_corner = TABLE_MODE + std::string(1, static_cast<char>(ANSI_TABLE_PARTS::TABLE_MIDDLE_LEFT_TO_RIGHT)) + RESET_TABLE_MODE;
                right_corner = TABLE_MODE + std::string(1, static_cast<char>(ANSI_TABLE_PARTS::TABLE_MIDDLE_RIGHT_TO_LEFT)) + RESET_TABLE_MODE;

                hor_separator = TABLE_MODE + std::string(1, static_cast<char>(ANSI_TABLE_PARTS::TABLE_MIDDLE_SEPARATOR)) + RESET_TABLE_MODE;
                
                top_to_bottom = TABLE_MODE + std::string(1, static_cast<char>(ANSI_TABLE_PARTS::TABLE_MIDDLE_TOP_TO_BOTTOM)) + RESET_TABLE_MODE;
            }

            else if (is_last && is_ANSI) {
                left_corner = TABLE_MODE + std::string(1, static_cast<char>(ANSI_TABLE_PARTS::TABLE_BOTTOM_LEFT_CORNER)) + RESET_TABLE_MODE;
                right_corner = TABLE_MODE + std::string(1, static_cast<char>(ANSI_TABLE_PARTS::TABLE_BOTTOM_RIGHT_CORNER)) + RESET_TABLE_MODE;

                hor_separator = TABLE_MODE + std::string(1, static_cast<char>(ANSI_TABLE_PARTS::TABLE_MIDDLE_BOTTOM_TO_TOP)) + RESET_TABLE_MODE;
            }

            stream << left_corner;

            size_t cols_num = reference.columns.size();

            int tracker = 0;
            for (size_t j = 0; j < cols_num; j++) {
                Column col = reference.columns[j];
                unsigned col_width = col.get_width();

                // tracker++;

                for (unsigned int k = 0; k < col_width; k++) {
                    tracker++;
                    
                    if (std::find(next_row_corners.begin(), next_row_corners.end(), tracker) != next_row_corners.end())
                        stream << top_to_bottom;
                    else
                        stream << horizontal;
                }

                tracker++;

                if (j + 1 != cols_num)
                    stream << hor_separator;
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
        std::vector<Row>
            rows;

        Table() : border_style(BorderStyle::standard), width(0), forced_width(false) {}

        // configure the table
        Setters configure() { return Setters(*this); }

        Format format() { return Format(*this); }

        void set_width(int width) {
            if (width > 0)
                this->width = static_cast<unsigned int>(width);
            else
                this->width = 0;
        }

        int get_width() { return this->width; }

        // * for testing set a fixed, forced width
        void set_forced_width(int width) {
            if (width < 0)
                this->width = 0;
            else
                this->width = static_cast<unsigned int>(width);

            forced_width = true;
        }

        // * for testing force using ansi (used when redirecting output to files)
        void set_forced_ansi() {
            border_style = BorderStyle::ANSI;

            force_ansi = true;
        }

        void remove_forced_width() { forced_width = false; }

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

            if (border_style == BorderStyle::ANSI) {
                // if (!regular) // no support for irregular tables
                //     border_style = BorderStyle::standard;
                if (!utils::check_terminal() && !force_ansi) // for testing/redirecting to files (not a terminal)
                    border_style = BorderStyle::standard;
            }

            if (border_style != BorderStyle::ANSI) {
                // adjusting border style
                style::Border templates = style::get_border_template(border_style);

                if (border_templates.corner.empty()) border_templates.corner = templates.corner;
                if (border_templates.horizontal.empty()) border_templates.horizontal = templates.horizontal;
                if (border_templates.vertical.empty()) border_templates.vertical = templates.vertical;

                // if (!regular) border_templates.corner = border_templates.horizontal; // for styling
            }

            /* ------ printing the table ------- */

            // 0 to check in print_row
            unsigned int row_width_reference = regular ? 0 : find_max_rows_width();

            if (!regular)
                for (auto it = rows.begin(); it != rows.end(); ++it)
                    utils::format_row(row_width_reference - (it->columns.size() + 1), *it);  
            
            bool is_first = true, is_last;
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
    };

    inline std::ostream& operator<<(std::ostream& stream, const Table& table) {
        const_cast<Table&>(table).print(stream);
        return stream;
    }
} // namespace tabular