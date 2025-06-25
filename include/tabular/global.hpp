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

#ifndef TABULAR_GLOBAL_HPP
#define TABULAR_GLOBAL_HPP

#define TABULAR_VERSION_MAJOR 1
#define TABULAR_VERSION_MINOR 0
#define TABULAR_VERSION_PATCH 1
#define TABULAR_VERSION "1.0.1"

#if defined(_WIN32) || defined(_WIN64)
#define WINDOWS
#include <windows.h>
#include <io.h> // for _isatty
#undef RGB

#elif defined(__unix__) || defined(__unix) || defined(__APPLE__) || defined(__linux__)
#define UNIX_LIKE
#include <sys/ioctl.h>
#include <unistd.h>

#else
#error unsupported platform
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
