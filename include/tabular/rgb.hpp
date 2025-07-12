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

#ifndef TABULAR_RGB_HPP
#define TABULAR_RGB_HPP

#include <cstdint>

namespace tabular {
  typedef struct RGB {
    uint8_t r, g, b;

    RGB(const uint8_t r, const uint8_t g, const uint8_t b) : r(r), g(g), b(b) {}
  } RGB;
} // namespace tabular

#endif // TABULAR_RGB_HPP
