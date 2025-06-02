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

#ifndef TABULAR_BORDER_HPP
#define TABULAR_BORDER_HPP

#include <string>

namespace tabular {
    struct Border {
        std::string vertical;
        std::string horizontal;

        std::string corner;
        std::string bottom_right_corner;
        std::string top_right_corner;
        std::string top_left_corner;
        std::string bottom_left_corner;

        std::string middle_separator; // +

        std::string left_to_right;
        std::string right_to_left;
        std::string bottom_to_top;
        std::string top_to_bottom;
    };
} // namespace tabular

#endif // TABULAR_BORDER_HPP
