/*
 * utf8conv is a utility header providing functions to convert between
 * wide-character strings (`std::wstring`) and standard C++ strings
 * (`std::string`) using UTF-8 encoding, without relying on the system locale
 * settings.
 *
 * It includes:
 *   - `utf8wsts`: converts `std::wstring` to `std::string`
 *   - `utf8stws`: converts `std::string` to `std::wstring`
 *
 * These functions are designed for consistent and locale-independent behavior,
 * making them suitable for environments where locale configuration is
 * unreliable.
 *
 * — Anas Hamdane, 2025-06-10
 */

#ifndef UTF8_CONV_HPP
#define UTF8_CONV_HPP

/* Detect wchar_t encoding (UTF-16/UTF-32) using platform macros */
#if defined(_WIN32) || defined(_WIN64)
#define USE_UTF16 true
#else
#define USE_UTF16 false
#endif

#include <cstdint>
#include <string>

namespace conv {
  /* Lookup table for UTF-8 sequence lengths */
  static const unsigned char utf8_sequence_length[256] = {
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
      2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  /* Decode UTF-8 to code point */
  inline bool
  utf8_decode(const char* s, uint32_t& codepoint, int& consumed) {
    const unsigned char* u = reinterpret_cast<const unsigned char*>(s);
    unsigned char c = u[0];
    consumed = utf8_sequence_length[c];
    if (consumed == 0) return false;

    switch (consumed) {
      case 1:
        codepoint = c;
        return true;
      case 2:
        if ((u[1] & 0xC0) != 0x80) return false;
        codepoint = ((c & 0x1F) << 6) | (u[1] & 0x3F);
        return codepoint >= 0x80;
      case 3:
        if ((u[1] & 0xC0) != 0x80 || (u[2] & 0xC0) != 0x80) return false;
        codepoint = ((c & 0x0F) << 12) | ((u[1] & 0x3F) << 6) | (u[2] & 0x3F);
        return codepoint >= 0x800 && (codepoint < 0xD800 || codepoint > 0xDFFF);
      case 4:
        if ((u[1] & 0xC0) != 0x80 || (u[2] & 0xC0) != 0x80 || (u[3] & 0xC0) != 0x80) return false;
        codepoint = ((c & 0x07) << 18) | ((u[1] & 0x3F) << 12) |
                    ((u[2] & 0x3F) << 6) | (u[3] & 0x3F);
        return codepoint >= 0x10000 && codepoint <= 0x10FFFF;
    }
    return false;
  }

  /* Decode UTF-16 surrogate pair to code point */
  inline bool utf16_decode(const wchar_t* s, size_t len, uint32_t& codepoint, int& consumed) {
    if (len == 0) return false;

    wchar_t c1 = s[0];

    // Basic Multilingual Plane (BMP)
    if (c1 < 0xD800 || c1 > 0xDFFF) {
      codepoint = static_cast<uint32_t>(c1);
      consumed = 1;
      return true;
    }

    // High surrogate
    if (c1 >= 0xD800 && c1 <= 0xDBFF) {
      if (len < 2) return false;
      wchar_t c2 = s[1];

      // Low surrogate
      if (c2 >= 0xDC00 && c2 <= 0xDFFF) {
        codepoint = 0x10000 + ((c1 & 0x3FF) << 10) + (c2 & 0x3FF);
        consumed = 2;
        return true;
      }
    }

    return false;
  }

  /* Decode UTF-32 to code point */
  inline bool utf32_decode(const wchar_t* s, size_t len, uint32_t& codepoint, int& consumed) {
    if (len == 0) return false;

    codepoint = static_cast<uint32_t>(s[0]);
    consumed = 1;

    // Validate code point range and exclude surrogates
    return codepoint <= 0x10FFFF && (codepoint < 0xD800 || codepoint > 0xDFFF);
  }

  /* Encode code point to UTF-8 */
  inline int utf8_encode(uint32_t codepoint, std::string& output) {
    if (codepoint <= 0x7F) {
      // 1-byte sequence: 0xxxxxxx
      output.push_back(static_cast<char>(codepoint));
      return 1;
    }
    if (codepoint <= 0x7FF) {
      // 2-byte sequence: 110xxxxx 10xxxxxx
      output.push_back(static_cast<char>(0xC0 | (codepoint >> 6)));
      output.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
      return 2;
    }
    if (codepoint <= 0xFFFF) {
      // 3-byte sequence: 1110xxxx 10xxxxxx 10xxxxxx
      output.push_back(static_cast<char>(0xE0 | (codepoint >> 12)));
      output.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
      output.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
      return 3;
    }
    if (codepoint <= 0x10FFFF) {
      // 4-byte sequence: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
      output.push_back(static_cast<char>(0xF0 | (codepoint >> 18)));
      output.push_back(static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F)));
      output.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
      output.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
      return 4;
    }
    return 0;
  }

  /* Encode code point to UTF-16 */
  inline int utf16_encode(uint32_t codepoint, std::wstring& output) {
    if (codepoint < 0xD800 || (codepoint > 0xDFFF && codepoint <= 0xFFFF)) {
      output.push_back(static_cast<wchar_t>(codepoint));
      return 1;
    }
    if (codepoint <= 0x10FFFF) {
      codepoint -= 0x10000;
      output.push_back(static_cast<wchar_t>(0xD800 | (codepoint >> 10)));
      output.push_back(static_cast<wchar_t>(0xDC00 | (codepoint & 0x3FF)));
      return 2;
    }
    return 0;
  }

  /* Encode code point to UTF-32 */
  inline int utf32_encode(uint32_t codepoint, std::wstring& output) {
    if (codepoint <= 0x10FFFF && (codepoint < 0xD800 || codepoint > 0xDFFF)) {
      output.push_back(static_cast<wchar_t>(codepoint));
      return 1;
    }
    return 0;
  }

  /* Convert UTF-8 string to wstring */
  inline std::wstring utf8stws(const std::string& input) {
    if (input.empty()) {
      return std::wstring();
    }

    std::wstring output;
    // Reserve space to minimize reallocations
    output.reserve(input.length());

    const char* p = input.c_str();
    const char* end = p + input.length();

    while (p < end) {
      uint32_t cp;
      int consumed;

      if (utf8_decode(p, cp, consumed)) {
        int written = USE_UTF16 ? utf16_encode(cp, output) : utf32_encode(cp, output);

        if (written > 0) {
          p += consumed;
          continue;
        }
      }

      // Add replacement character
      output.push_back(L'?');
      p++;
    }

    return output;
  }

  /* Convenience overload for C-style strings */
  inline std::wstring utf8stws(const char* input) {
    if (!input) {
      return std::wstring();
    }
    return utf8stws(std::string(input));
  }

  /* Convert wstring to UTF-8 string */
  inline std::string utf8wsts(const std::wstring& input) {
    if (input.empty())
      return std::string();

    std::string output;
    // Reserve space (worst case: 4 bytes per wide char)
    output.reserve(input.length() * 4);

    const wchar_t* p = input.c_str();
    size_t remaining = input.length();

    while (remaining > 0) {
      uint32_t cp;
      int consumed;

      bool decoded = USE_UTF16 ? utf16_decode(p, remaining, cp, consumed) : utf32_decode(p, remaining, cp, consumed);

      if (decoded) {
        int written = utf8_encode(cp, output);

        if (written > 0) {
          p += consumed;
          remaining -= consumed;
          continue;
        }
      }

      // Add replacement character
      output.push_back('?');
      p++;
      remaining--;
    }

    return output;
  }

  /* Convenience overload for C-style wide strings */
  inline std::string utf8wsts(const wchar_t* input) {
    if (!input)
      return std::string();
    
    return utf8wsts(std::wstring(input));
  }
} // namespace conv

#endif // !UTF8_CONV_HPP
