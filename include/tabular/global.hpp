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

#ifndef TABULAR_GLOBAL_HPP
#define TABULAR_GLOBAL_HPP

#define TABULAR_VERSION_MAJOR 1
#define TABULAR_VERSION_MINOR 1
#define TABULAR_VERSION_PATCH 0
#define TABULAR_VERSION "1.1.0"

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h> // for table rendering
#include <io.h>
#undef RGB

#endif

namespace tabular {
  namespace ansi {
    // table ANSI printing
    constexpr auto TABLE_MODE = "\x1b(0";
    constexpr auto RESET_TABLE_MODE = "\x1B(B";

    // Control sequence introducer (for font styles and colors)
    constexpr auto CSI = "\x1b[";
    constexpr auto RESET = "\x1b[0m";

    // end of an ansi sequence
    constexpr char suffix = 'm';

    constexpr auto FG_COLOR_RESET = "\x1b[39m";
    constexpr auto BG_COLOR_RESET = "\x1b[49m";
  } // namespace ansi
} // namespace tabular

#endif // TABULAR_GLOBAL_HPP
