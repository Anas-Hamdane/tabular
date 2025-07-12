/*
      __        ___.         .__
    _/  |______ \_ |__  __ __|  | _____ _______
    \   __\__  \ | __ \|  |  \  | \__  \\_  __ \
     |  |  / __ \| \_\ \  |  /  |__/ __ \|  | \/
     |__| (____  /___  /____/|____(____  /__|
               \/    \/                \/

    *  Author: Anas Hamdane
    *  Github: https://github.com/Anas-Hamdane
    *  LICENSE: https://github.com/Anas-Hamdane/tabular/blob/main/LICENSE
*/

#ifndef TABULAR_ATTRIBUTES_HPP
#define TABULAR_ATTRIBUTES_HPP

namespace tabular {
  enum class Attribute {
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
} // namespace tabular

#endif // TABULAR_ATTRIBUTES_HPP
