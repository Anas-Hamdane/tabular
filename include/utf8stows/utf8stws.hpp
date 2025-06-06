/*
 * utf8stws (utf8 string to wide string) is an utility function designed to
 * convert a standard C string (char*) to a wide-character string (wchar_t*)
 * without relying on the system locale settings.
 *
 * This function aims to provide consistent and locale-independent behavior,
 * making it suitable for environments where locale configuration is unreliable.
 *
 * Author: Anas Hamdane
 * Date: 2025-06-05
 * GitHub: https://github.com/Anas-Hamdane
 */

#ifndef UTF8_STWS_HPP

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cwchar>

/* Detect wchar_t encoding (UTF-16/UTF-32) using platform macros */
#if defined(_WIN32) || defined(_WIN64)
#define USE_UTF16 1
#else
#define USE_UTF16 0
#endif

/* Decode UTF-8 to code point */
inline int utf8_decode(const char *s, uint32_t *codepoint, int *consumed) {
  unsigned char c = s[0];
  if (c < 0x80) {
    *codepoint = c;
    *consumed = 1;
  } else if ((c >> 5) == 0x6) {
    *codepoint = ((c & 0x1F) << 6) | (s[1] & 0x3F);
    *consumed = 2;
  } else if ((c >> 4) == 0xE) {
    *codepoint = ((c & 0x0F) << 12) | ((s[1] & 0x3F) << 6) | (s[2] & 0x3F);
    *consumed = 3;
  } else if ((c >> 3) == 0x1E) {
    *codepoint = ((c & 0x07) << 18) | ((s[1] & 0x3F) << 12) |
                 ((s[2] & 0x3F) << 6) | (s[3] & 0x3F);
    *consumed = 4;
  } else {
    return 0;
  }
  return 1;
}

/* Encode code point to UTF-16 */
inline int utf16_encode(uint32_t codepoint, wchar_t *output) {
  if (codepoint <= 0xFFFF && (codepoint < 0xD800 || codepoint > 0xDFFF)) {
    output[0] = (wchar_t)codepoint;
    return 1;
  } else if (codepoint <= 0x10FFFF) {
    codepoint -= 0x10000;
    output[0] = (wchar_t)(0xD800 + (codepoint >> 10));
    output[1] = (wchar_t)(0xDC00 + (codepoint & 0x3FF));
    return 2;
  }
  return 0;
}

/* Encode code point to UTF-32 */
inline int utf32_encode(uint32_t codepoint, wchar_t *output) {
  if (codepoint > 0x10FFFF || (codepoint >= 0xD800 && codepoint <= 0xDFFF))
    return 0;
  output[0] = (wchar_t)codepoint;
  return 1;
}

/* Convert UTF-8 to wchar_t string */
inline wchar_t *utf8stws(const char *input) {
  size_t len = strlen(input);
  wchar_t *output =
      (wchar_t *)malloc((len * 2 + 1) * sizeof(wchar_t)); // Enough space
  size_t out_index = 0;
  size_t i = 0;

  while (i < len) {
    uint32_t cp;
    int consumed;

    if (!utf8_decode(&input[i], &cp, &consumed))
      break;

    int written = 0;
    if (USE_UTF16)
      written = utf16_encode(cp, &output[out_index]);
    else
      written = utf32_encode(cp, &output[out_index]);

    out_index += written;
    i += consumed;
  }

  output[out_index] = L'\0'; // null-terminate
  return output;
}

#endif // !UTF8_STWS_HPP
