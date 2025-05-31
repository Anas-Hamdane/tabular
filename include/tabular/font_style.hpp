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
}

#endif // TABULAR_FONT_STYLE_HPP
