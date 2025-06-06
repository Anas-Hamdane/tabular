/*
 * Helper header file for my tabular project
 *
 * Author: Anas Hamdane
 * GitHub: https://github.com/Anas-Hamdane
 * tabular: https://github.com/Anas-Hamdane/tabular
 * wcwidth edit: https://github.com/Anas-Hamdane/wcwidth
 *
 */

#ifndef WCWIDTH_HPP
#define WCWIDTH_HPP

#include "unicode_width_tables.h"

static int bisearch(wchar_t ucs, const struct interval *table, int max) {
  int min = 0;
  int mid;

  if (ucs < table[0].first || ucs > table[max].last)
    return 0;
  while (max >= min) {
    mid = (min + max) / 2;
    if (ucs > table[mid].last)
      min = mid + 1;
    else if (ucs < table[mid].first)
      max = mid - 1;
    else
      return 1;
  }

  return 0;
}

#include <cwchar>

inline int ah_wcwidth(wchar_t ucs) {
  if (ucs == 0)
    return 0;

  // control characters
  if (ucs < 32 || (ucs >= 0x7f && ucs < 0xa0))
    return -1;

  // non-spacing/zero-width characters
  if (bisearch(ucs, combining, sizeof(combining) / sizeof(struct interval) - 1))
    return 0;

  // wide/fullwidth characters
  if (bisearch(ucs, wide, sizeof(wide) / sizeof(struct interval) - 1))
    return 2;

  return 1;
}

inline int ah_wcswidth(const wchar_t *pwcs, size_t n) {
  if (!pwcs)
    return 0;

  int w, width = 0;

  for (; *pwcs && n-- > 0; pwcs++)
    if ((w = ah_wcwidth(*pwcs)) < 0)
      return -1;
    else
      width += w;

  return width;
}

#endif // !WCWIDTH_HPP
