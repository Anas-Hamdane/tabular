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

#ifndef TABULAR_RANGE_HPP
#define TABULAR_RANGE_HPP

namespace tabular {
    typedef struct Range {
        int from;
        int to;

        Range(int from, int to) : from(from), to(to) {}
    } Range;
} // namespace tabular

#endif // TABULAR_RANGE_HPP
