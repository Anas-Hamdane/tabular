/*
 * Helper header file for my tabular project
 *
 * Author: Anas Hamdane
 * GitHub: https://github.com/Anas-Hamdane
 * tabular: https://github.com/Anas-Hamdane/tabular
 * wcwidth edit: https://github.com/Anas-Hamdane/wcwidth
 *
 * NOTE: this version has been edited to match the tabular project
 */

#ifndef WCWIDTH_HPP
#define WCWIDTH_HPP

#include "unicode_width_tables.h"

#include <stdio.h>

namespace display_width {
  static bool bisearch(wchar_t ucs, const struct interval* table, int max) {
    int min = 0;
    int mid;

    if (ucs < table[0].first || ucs > table[max].last)
      return false;

    while (max >= min) {
      mid = (min + max) / 2;
      if (ucs > table[mid].last)
        min = mid + 1;
      else if (ucs < table[mid].first)
        max = mid - 1;
      else
        return true;
    }

    return false;
  }

  inline int wcwidth(wchar_t ucs) {
    if (ucs == 0)
      return 0;

    // edit: to match the width calculation for tabular
    if (ucs == 0x1B)
      return 1;

    // edited to match tabular width calculation
    // control characters
    // if (ucs < 32 || (ucs >= 0x7f && ucs < 0xa0))
    // return -1;

    /* no need to check for combining characters since the user will give visible characters*/
    // non-spacing/zero-width characters
    // if (bisearch(ucs, combining, combining_size - 1))
    //   return 0;

    // wide/fullwidth characters
    if (bisearch(ucs, wide, wide_size - 1))
      return 2;

    return 1;
  }

  // edited to ignore control characters
  inline size_t wcswidth(const wchar_t* pwcs, size_t n) {
    if (!pwcs)
      return 0;

    int w;
    size_t width = 0;

    for (; *pwcs && n-- > 0; pwcs++)
      if ((w = wcwidth(*pwcs)) > 0)
        width += static_cast<size_t>(w);

    return width;
  }
} // namespace display_width

#endif // !WCWIDTH_HPP
