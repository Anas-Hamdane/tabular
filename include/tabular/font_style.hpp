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

#ifndef TABULAR_FONT_STYLE_HPP
#define TABULAR_FONT_STYLE_HPP

namespace tabular {
    enum class FontStyle {
        bold,
        dark,
        italic,
        underline,
        blink,
        reverse,
        concealed, // doesn't work for windows
        crossed
    };
}

#endif // TABULAR_FONT_STYLE_HPP
