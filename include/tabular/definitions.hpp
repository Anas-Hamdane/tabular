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

#ifndef TABULAR_GL_VARS_HPP
#define TABULAR_GL_VARS_HPP

#if defined(_WIN32) || defined(_WIN64)
#define OS_WINDOWS
#elif defined(__APPLE__)
#define OS_MACOS
#elif defined(__unix__) || defined(__unix)
#define OS_LINUX_BASED
#else
#error unsupported platform
#endif

#if defined(OS_WINDOWS)
#include <windows.h>
#elif defined(OS_LINUX_BASED) || defined(OS_MACOS)
#include <sys/ioctl.h>
#include <unistd.h>
#else
#error Unsupported platform
#endif

#define DEFAULT_WIDTH_PERCENT .5
#define CONTENT_MANIPULATION_BACK_LIMIT .3 // back limit percent

// table ANSI printing
#define TABLE_MODE "\x1b(0"
#define RESET_TABLE_MODE "\x1B(B"

// Control sequence introducer (for font styles and colors)
#define CSI "\x1b["

#define RESET "\x1b[0m"
#define FG_COLOR_RESET "\x1b[39m"
#define BG_COLOR_RESET "\x1b[49m"

#endif // TABULAR_GL_VARS_HPP
