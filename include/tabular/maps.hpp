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

#ifndef TABULAR_MAPS_HPP
#define TABULAR_MAPS_HPP

#include <map>

#include <tabular/border.hpp>
#include <tabular/border_style.hpp>
#include <tabular/definitions.hpp>
#include <tabular/styles.hpp>

namespace tabular {
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
} // namespace tabular

#endif // TABULAR_MAPS_HPP
