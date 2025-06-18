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

#if defined(_WIN32) || defined(_WIN64)
#define WINDOWS 1
#define UTF16 1
#include <windows.h>
#undef RGB

#elif defined(__unix__) || defined(__unix) || defined(__APPLE__) || defined(__linux__)
#define UNIX_BASED 1
#define UTF16 0
#include <sys/ioctl.h>
#include <unistd.h>

#else
#error unsupported platform
#endif

namespace tabular {
  namespace ansi {
    // table ANSI printing
    constexpr const char* TABLE_MODE = "\x1b(0";
    constexpr const char* RESET_TABLE_MODE = "\x1B(B";

    // Control sequence introducer (for font styles and colors)
    constexpr const char* CSI = "\x1b[";
    constexpr const char* RESET = "\x1b[0m";

    constexpr const char* FG_COLOR_RESET = "\x1b[39m";
    constexpr const char* BG_COLOR_RESET = "\x1b[49m";
  } // namespace ansi
} // namespace tabular

#endif // TABULAR_GLOBAL_HPP
