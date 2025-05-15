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

#ifndef TABULAR_ENUMS_HPP
#define TABULAR_ENUMS_HPP

namespace tabular {
    enum class Alignment {
        left,
        center,
        right
    };
    enum class FontStyle {
        bold,
        dark,
        italic,
        underline,
        blink,
        reverse,
        concealed,
        crossed
    };
    enum class BorderStyle {
        standard,
        empty,
        ANSI
    };

    typedef struct Range {
        int from;
        int to;

        Range(int from, int to) : from(from), to(to) {}
    } Range;
    typedef enum {
        // straight lines
        TABLE_HORIZONTAL = 'q',
        TABLE_VERTICAL = 'x',

        // corners
        TABLE_BOTTOM_RIGHT_CORNER = 'j',
        TABLE_TOP_RIGHT_CORNER = 'k',
        TABLE_TOP_LEFT_CORNER = 'l',
        TABLE_BOTTOM_LEFT_CORNER = 'm',
        TABLE_MIDDLE_SEPARATOR = 'n',
        
        // middle separators
        TABLE_MIDDLE_LEFT_TO_RIGHT = 't',
        TABLE_MIDDLE_RIGHT_TO_LEFT = 'u',
        
        TABLE_MIDDLE_BOTTOM_TO_TOP = 'v',
        TABLE_MIDDLE_TOP_TO_BOTTOM = 'w'
    } ANSI_TABLE_PARTS;
} // namespace tabular

#endif // TABULAR_ENUMS_HPP