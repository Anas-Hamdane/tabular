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

#ifndef TABULAR_STYLES_HPP
#define TABULAR_STYLES_HPP

namespace tabular {
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
} // namespace tabular

#endif // TABULAR_STYLES_HPP
