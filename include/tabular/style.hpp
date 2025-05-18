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

#ifndef TABULAR_STYLE_HPP
#define TABULAR_STYLE_HPP

#include <map>
#include <string>

#include <tabular/definitions.hpp>
#include <tabular/global_vars.hpp>

namespace tabular {
    namespace style {
        struct Border {
            std::string vertical;
            std::string horizontal;

            std::string corner;
            std::string bottom_right_corner;
            std::string top_right_corner;
            std::string top_left_corner;
            std::string bottom_left_corner;

            std::string middle_separator; // +

            std::string middle_left_to_right;
            std::string middle_right_to_left;
            std::string middle_bottom_to_top;
            std::string middle_top_to_bottom;
        };

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

        static std::string get_font_style(FontStyle style) {
            static std::map<FontStyle, std::string> styles{
                // {FontStyle::reset, "0m"},
                {FontStyle::bold, "1m"},
                {FontStyle::dark, "2m"},
                {FontStyle::italic, "3m"},
                {FontStyle::underline, "4m"},
                {FontStyle::blink, "5m"},
                {FontStyle::reverse, "7m"},
                {FontStyle::concealed, "8m"},
                {FontStyle::crossed, "9m"}};
            return styles[style];
        }
    }; // namespace style
} // namespace tabular

#endif // TABULAR_STYLE_HPP