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

/*
    TODO:
      -  [x] implement Dynamic table logic
      -  [x] default padding
      -  [x] ANSI Support
      -  [x] Alignment support
      -  [x] terminal colors and 16 colors support
      -  [x] terminal colors rgb support
      -  [x] terminal font styles support
      -  [x] padding control
      -  [x] width control
      -  [x] range columns setters (functions)
      -  [x] support for multi byte characters (automatic and manual)
      -  [x] full column background color support
      -  [x] border styling (colors, rgb)
*/

#include <algorithm>
#include <climits>
#include <cstdint>
#include <list>
#include <map>
#include <ostream>
#include <string>
#include <vector>

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
#define USE_UTF16 true
#include <windows.h>
#elif defined(OS_LINUX_BASED) || defined(OS_MACOS)
#define USE_UTF16 false
#include <sys/ioctl.h>
#include <unistd.h>
#else
#error Unsupported platform
#endif

#define CONTENT_MANIPULATION_BACK_LIMIT .3 // back limit percent

// table ANSI printing
#define TABLE_MODE "\x1b(0"
#define RESET_TABLE_MODE "\x1B(B"

// Control sequence introducer (for font styles and colors)
#define CSI "\x1b["
#define RESET "\x1b[0m"

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
    if (input.empty()) {
      return std::string();
    }

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
    if (!input) {
      return std::string();
    }
    return utf8wsts(std::wstring(input));
  }
} // namespace conv

namespace display_width {
  struct interval {
    int first;
    int last;
  };

  // clang-format off
    /* Wide characters (East Asian wide/fullwidth) */
    static const struct interval wide[] = {
        {0x1100, 0x115F}, {0x231A, 0x231B}, {0x2329, 0x232A}, 
        {0x23E9, 0x23EC}, {0x23F0, 0x23F0}, {0x23F3, 0x23F3}, 
        {0x25FD, 0x25FE}, {0x2614, 0x2615}, {0x2630, 0x2637}, 
        {0x2648, 0x2653}, {0x267F, 0x267F}, {0x268A, 0x268F}, 
        {0x2693, 0x2693}, {0x26A1, 0x26A1}, {0x26AA, 0x26AB}, 
        {0x26BD, 0x26BE}, {0x26C4, 0x26C5}, {0x26CE, 0x26CE}, 
        {0x26D4, 0x26D4}, {0x26EA, 0x26EA}, {0x26F2, 0x26F3}, 
        {0x26F5, 0x26F5}, {0x26FA, 0x26FA}, {0x26FD, 0x26FD}, 
        {0x2705, 0x2705}, {0x270A, 0x270B}, {0x2728, 0x2728}, 
        {0x274C, 0x274C}, {0x274E, 0x274E}, {0x2753, 0x2755}, 
        {0x2757, 0x2757}, {0x2795, 0x2797}, {0x27B0, 0x27B0}, 
        {0x27BF, 0x27BF}, {0x2B1B, 0x2B1C}, {0x2B50, 0x2B50}, 
        {0x2B55, 0x2B55}, {0x2E80, 0x2E99}, {0x2E9B, 0x2EF3}, 
        {0x2F00, 0x2FD5}, {0x2FF0, 0x3029}, {0x302E, 0x303E}, 
        {0x3041, 0x3096}, {0x309B, 0x30FF}, {0x3105, 0x312F}, 
        {0x3131, 0x318E}, {0x3190, 0x31E5}, {0x31EF, 0x321E}, 
        {0x3220, 0x3247}, {0x3250, 0xA48C}, {0xA490, 0xA4C6}, 
        {0xA960, 0xA97C}, {0xAC00, 0xD7A3}, {0xF900, 0xFA6D}, 
        {0xFA70, 0xFAD9}, {0xFE10, 0xFE19}, {0xFE30, 0xFE52}, 
        {0xFE54, 0xFE66}, {0xFE68, 0xFE6B}, {0xFF01, 0xFF60}, 
        {0xFFE0, 0xFFE6}, {0x16FE0, 0x16FE3}, {0x16FF0, 0x16FF1}, 
        {0x17000, 0x187F7}, {0x18800, 0x18CD5}, {0x18CFF, 0x18D08}, 
        {0x1AFF0, 0x1AFF3}, {0x1AFF5, 0x1AFFB}, {0x1AFFD, 0x1AFFE}, 
        {0x1B000, 0x1B122}, {0x1B132, 0x1B132}, {0x1B150, 0x1B152}, 
        {0x1B155, 0x1B155}, {0x1B164, 0x1B167}, {0x1B170, 0x1B2FB}, 
        {0x1D300, 0x1D356}, {0x1D360, 0x1D376}, {0x1F004, 0x1F004}, 
        {0x1F0CF, 0x1F0CF}, {0x1F18E, 0x1F18E}, {0x1F191, 0x1F19A}, 
        {0x1F200, 0x1F202}, {0x1F210, 0x1F23B}, {0x1F240, 0x1F248}, 
        {0x1F250, 0x1F251}, {0x1F260, 0x1F265}, {0x1F300, 0x1F320}, 
        {0x1F32D, 0x1F335}, {0x1F337, 0x1F37C}, {0x1F37E, 0x1F393}, 
        {0x1F3A0, 0x1F3CA}, {0x1F3CF, 0x1F3D3}, {0x1F3E0, 0x1F3F0}, 
        {0x1F3F4, 0x1F3F4}, {0x1F3F8, 0x1F43E}, {0x1F440, 0x1F440}, 
        {0x1F442, 0x1F4FC}, {0x1F4FF, 0x1F53D}, {0x1F54B, 0x1F54E}, 
        {0x1F550, 0x1F567}, {0x1F57A, 0x1F57A}, {0x1F595, 0x1F596}, 
        {0x1F5A4, 0x1F5A4}, {0x1F5FB, 0x1F64F}, {0x1F680, 0x1F6C5}, 
        {0x1F6CC, 0x1F6CC}, {0x1F6D0, 0x1F6D2}, {0x1F6D5, 0x1F6D7}, 
        {0x1F6DC, 0x1F6DF}, {0x1F6EB, 0x1F6EC}, {0x1F6F4, 0x1F6FC}, 
        {0x1F7E0, 0x1F7EB}, {0x1F7F0, 0x1F7F0}, {0x1F90C, 0x1F93A}, 
        {0x1F93C, 0x1F945}, {0x1F947, 0x1F9FF}, {0x1FA70, 0x1FA7C}, 
        {0x1FA80, 0x1FA89}, {0x1FA8F, 0x1FAC6}, {0x1FACE, 0x1FADC}, 
        {0x1FADF, 0x1FAE9}, {0x1FAF0, 0x1FAF8}, {0x20000, 0x2A6DF}, 
        {0x2A700, 0x2B739}, {0x2B740, 0x2B81D}, {0x2B820, 0x2CEA1}, 
        {0x2CEB0, 0x2EBE0}, {0x2EBF0, 0x2EE5D}, {0x2F800, 0x2FA1D}, 
        {0x30000, 0x3134A}, {0x31350, 0x323AF}
    };
    constexpr size_t wide_size = sizeof(wide) / sizeof(interval);
  // clang-format on

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

namespace tabular {
  enum class Alignment {
    left,
    center,
    right
  };

  enum class BorderStyle {
    standard,
    empty,
    ANSI
  };

  enum class Font {
    bold = 1,
    dim,
    italic,
    underline,
    blink, // rarely supported
    reverse = 7,
    concealed,
    crossed
  };

  enum class Color {
    black = 30,
    red,
    green,
    yellow,
    blue,
    magenta,
    cyan,
    white,
    normal = 39,
  };

  enum class Style {
    bold = 1,
    dim = 2,
    italic = 3,

    underline = 4,
    double_underline = 21, // not popular

    blink = 5,      // rarely supported
    fast_blink = 6, // rarely supported
    reverse = 7,
    concealed = 8,
    crossed = 9,
  };

  struct BorderGlyphs {
    std::string vertical;
    std::string horizontal;

    std::string bottom_right_corner;
    std::string top_right_corner;
    std::string top_left_corner;
    std::string bottom_left_corner;

    std::string intersection; // +

    std::string left_connector;
    std::string right_connector;
    std::string bottom_connector;
    std::string top_connector;
  };

  typedef struct RGB {
    uint8_t r, g, b;

    RGB(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}
  } RGB;

  class Column {
    // styles applied to the column content
    std::string text_styles;

    // column content coloring
    std::string content_color;
    std::string content_background_color;

    // column coloring
    std::string column_background_color;

    std::vector<std::string> splitted_content;
    std::list<std::string> words;
    Alignment alignment;
    unsigned int width;
    unsigned int top_padding;
    unsigned int bottom_padding;
    bool is_multi_byte;

    class Config {
      Column& column;

  public:
      Config(Column& column) : column(column) {}

      Config& alignment(Alignment align) {
        column.alignment = align;

        return *this;
      }

      Config& width(int w) {
        if (w <= 0)
          w = 0;

        column.width = static_cast<unsigned int>(w);

        return *this;
      }

      Config& padding(int padd) {
        if (padd <= 0)
          padd = 0;

        column.top_padding = padd;
        column.bottom_padding = padd;

        return *this;
      }

      Config& top_padding(int padd) {
        if (padd <= 0)
          column.top_padding = 0;
        else
          column.top_padding = static_cast<unsigned int>(padd);

        return *this;
      }

      Config& bottom_padding(int padd) {
        if (padd <= 0)
          column.bottom_padding = 0;
        else
          column.bottom_padding = static_cast<unsigned int>(padd);

        return *this;
      }

      // add text style to the whole column
      Config& text_style(Style style) {
        // style statement
        std::string style_stt = CSI + std::to_string(static_cast<int>(style)) + "m";

        column.text_styles += style_stt;

        // later add special_characters of RESET in utils in the last element of splitted_content
        return *this;
      }

      // add multiple text styles to the whole column
      Config& text_style(const std::vector<Style>& styles) {
        if (styles.empty()) return *this;

        // Apply all styles at the beginning
        for (const auto& style : styles)
          column.text_styles += CSI + std::to_string(static_cast<int>(style)) + "m";

        return *this;
      }

      // add Background Color to the whole column
      Config& content_background_color(Color color) {
        if (!column.content_background_color.empty())
          column.content_background_color.clear();

        column.content_background_color = CSI + std::to_string(static_cast<int>(color) + 10) + "m";

        return *this;
      }

      // add background RGB to the whole column
      Config& content_background_color(RGB rgb) {
        if (!column.content_background_color.empty())
          column.content_background_color.clear();

        column.content_background_color = CSI "48;2;" + std::to_string(rgb.r) + ";" +
                                          std::to_string(rgb.g) + ";" +
                                          std::to_string(rgb.b) + "m";

        return *this;
      }

      // add Color to the whole column
      Config& color(Color color) {
        if (!column.content_color.empty())
          column.content_color.clear();

        column.content_color = CSI + std::to_string(static_cast<int>(color)) + "m";

        return *this;
      }

      // add RGB to the whole column
      Config& color(RGB rgb) {
        if (!column.content_color.empty())
          column.content_color.clear();

        column.content_color = CSI "38;2;" + std::to_string(rgb.r) + ";" +
                               std::to_string(rgb.g) + ";" +
                               std::to_string(rgb.b) + "m";

        return *this;
      }

      // column background coloring
      Config& column_background_color(Color color) {
        if (!column.column_background_color.empty())
          column.column_background_color.clear();

        column.column_background_color = CSI + std::to_string(static_cast<int>(color) + 10) + "m";

        return *this;
      }

      Config& column_background_color(RGB rgb) {
        if (!column.column_background_color.empty())
          column.column_background_color.clear();

        column.column_background_color = CSI "48;2;" + std::to_string(rgb.r) + ";" +
                                         std::to_string(rgb.g) + ";" +
                                         std::to_string(rgb.b) + "m";

        return *this;
      }
    };

    class Getters {
      Column& column;

  public:
      Getters(Column& column) : column(column) {}

      Alignment alignment() { return column.alignment; }

      unsigned int width() { return column.width; }

      unsigned int top_padding() { return column.top_padding; }

      unsigned int bottom_padding() { return column.bottom_padding; }

      std::vector<std::string> splitted_content() { return column.splitted_content; }

      std::list<std::string> words() { return column.words; }

      std::string text_styles() { return column.text_styles; }

      std::string content_color() { return column.content_color; }

      std::string content_background_color() { return column.content_background_color; }

      std::string column_background_color() { return column.column_background_color; }

      bool is_multi_byte() { return column.is_multi_byte; }
    };

    class Setters {
      Column& column;

  public:
      Setters(Column& column) : column(column) {}

      Setters& splitted_content(std::vector<std::string> splittedContent) {
        column.splitted_content = splittedContent;
        return *this;
      }

      Setters& words(std::list<std::string> words) {
        column.words = words;
        return *this;
      }

      Setters& width(int width) {
        if (width <= 0)
          width = 0;

        column.width = static_cast<unsigned int>(width);

        return *this;
      }

      // multi byte characters support
      // (locale-independent but only utf8 encoding is supported)
      Setters& multi_byte_characters(bool is_multi_byte) {
        column.is_multi_byte = is_multi_byte;

        return *this;
      }
    };

public:
    std::string content;

    // automatic check for multi byte characters
    Column(std::string content) {
      this->content = content;
      alignment = Alignment::left;
      width = 0;
      top_padding = 0;
      bottom_padding = 0;

      text_styles = "";
      content_color = "";
      content_background_color = "";

      // auto detection(with ascii check) has been removed
      // for better performance
      is_multi_byte = false;
    }

    Config config() { return Config(*this); }

    Getters get() { return Getters(*this); }

    Setters set() { return Setters(*this); }
  };

  class Row {
    unsigned int width;

    class Config {
      Row& row;

  public:
      Config(Row& row) : row(row) {}

      Config& alignment(Alignment alignment) {
        for (Column& column : row.columns)
          column.config().alignment(alignment);

        return *this;
      }

      Config& padding(int padding) {
        for (Column& column : row.columns)
          column.config().padding(padding);

        return *this;
      }

      Config& top_padding(int padding) {
        for (Column& column : row.columns)
          column.config().top_padding(padding);

        return *this;
      }

      Config& bottom_padding(int padding) {
        for (Column& column : row.columns)
          column.config().bottom_padding(padding);

        return *this;
      }

      // add text style to the columns in this row
      Config& text_style(Style style) {
        for (Column& column : row.columns)
          column.config().text_style(style);

        return *this;
      }

      // add multiple text styles to the columns in this row
      Config& text_style(const std::vector<Style>& styles) {
        for (Column& column : row.columns)
          column.config().text_style(styles);

        return *this;
      }

      Config& color(Color color) {
        for (Column& column : row.columns)
          column.config().color(color);

        return *this;
      }

      Config& color(RGB rgb) {
        for (Column& column : row.columns)
          column.config().color(rgb);

        return *this;
      }

      Config& content_background_color(Color back_color) {
        for (Column& column : row.columns)
          column.config().content_background_color(back_color);

        return *this;
      }

      Config& content_background_color(RGB background_rgb) {
        for (Column& column : row.columns)
          column.config().content_background_color(background_rgb);

        return *this;
      }

      Config& column_background_color(Color back_color) {
        for (Column& column : row.columns)
          column.config().column_background_color(back_color);

        return *this;
      }

      Config& column_background_color(RGB background_rgb) {
        for (Column& column : row.columns)
          column.config().column_background_color(background_rgb);

        return *this;
      }
    };

    class Setters {
      Row& row;

  public:
      Setters(Row& row) : row(row) {}

      Setters& multi_byte_characters(bool is_multi_byte) {
        for (Column& column : row.columns)
          column.set().multi_byte_characters(is_multi_byte);

        return *this;
      }
    };

public:
    std::vector<Column>
        columns;

    Row(std::vector<Column> columns)
        : columns(columns) {}

    Config config() { return Config(*this); }

    Setters set() { return Setters(*this); }

    Column& operator[](int index) {
      return this->columns.at(index);
    }
  };

  namespace string_utils {
    // multi byte string width
    inline size_t mbswidth(std::string str) {
      std::wstring wstr = conv::utf8stws(str.c_str());
      size_t width = display_width::wcswidth(wstr.c_str(), wstr.length());

      return width;
    }

    inline size_t display_width(std::string str, bool is_multi_byte) {
      return is_multi_byte ? mbswidth(str) : str.length();
    }

    inline bool is_continuation_byte(unsigned char byte) {
      return (byte & 0xC0) == 0x80; // 10xxxxxx
    }

    inline std::size_t safe_split_pos(const std::string& s, std::size_t pos) {
      if (pos > s.size()) pos = s.size();
      while (pos > 0 && is_continuation_byte(static_cast<unsigned char>(s[pos]))) {
        --pos;
      }
      return pos;
    }

    // split by words AND save both '\n' and ' ' as words too
    // works at least for UTF-8 encoding strings
    inline std::list<std::string> split_text(const std::string& str) {
      std::list<std::string> result;

      size_t start = 0;
      const size_t len = str.size();

      for (size_t i = 0; i < len; ++i) {
        char ch = str[i];

        if (ch == ' ' || ch == '\n') {
          if (i > start)
            result.emplace_back(str.begin() + start, str.begin() + i);

          result.emplace_back(1, ch); // Push " " or "\n"
          start = i + 1;
        }
      }

      if (start < len)
        result.emplace_back(str.begin() + start, str.end());

      return result;
    }
  } // namespace string_utils

  namespace utils {
    // to align PPDirectives
    // clang-format off
        inline unsigned short get_terminal_width() {
            // first case: defined env var of COLUMNS
            const char* columns_env = std::getenv("COLUMNS");
            if (columns_env != nullptr) {
                try {
                    int width_int = std::stoi(columns_env);
                    if (width_int > 0 && width_int <= USHRT_MAX)
                        return static_cast<unsigned short>(width_int);
                } catch(...) {}
            }

            unsigned short width = 0;

            #if defined(OS_LINUX_BASED) || defined(OS_MACOS)
                struct winsize ws;
                if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1)
                    width = 0;
                else 
                    width = ws.ws_col;
            #elif defined(OS_WINDOWS)
                CONSOLE_SCREEN_BUFFER_INFO csbi;
                if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
                    width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
            #else
                width = 0;
            #endif

            return width;
        }

        inline bool check_terminal() {
            #if defined(OS_WINDOWS)
                // if it is not a valid terminal, enabling VTP will fail so we don't have to check :)
                static bool initialized = []() -> bool {
                    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
                    if (hOut == INVALID_HANDLE_VALUE)
                        return false;

                    DWORD dwMode = 0;
                    if (!GetConsoleMode(hOut, &dwMode))
                        return false;

                    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
                    if (!SetConsoleMode(hOut, dwMode))
                        return false;

                    return true;
                }();
                // SetConsoleOutputCP(CP_UTF8);
                return initialized;
            #else
                if (!isatty(STDIN_FILENO)) return false; // not a valid terminal
                return true;
            #endif
        }
    // clang-format on

    // finalize line and push it
    inline void commit_line(std::vector<std::string>& result, std::string& sub, size_t* sub_size, int usable_width, Column column) {
      std::string line;
      Alignment align = column.get().alignment();

      // auto horizontal padding of 1
      if (line.empty())
        line.append(" ");

      int start;
      if (align == Alignment::center)
        start = (usable_width - (*sub_size)) / 2;
      else if (align == Alignment::right)
        start = (usable_width - (*sub_size));
      else
        start = 0;

      line.append(start, ' ');

      std::string styles;

      // styling
      styles += column.get().text_styles();
      styles += column.get().content_color();
      styles += column.get().content_background_color();

      if (!styles.empty())
        line.append(styles);

      line.append(sub);

      if (!styles.empty())
        line.append(RESET);

      /* no need */
      // auto horizontal padding
      // line.append(" ");

      result.push_back(line);

      sub.clear();
      *sub_size = 0;
    }

    inline std::vector<std::string> format_column_lines(Column& column) {
      const std::string& content = column.content;
      const bool is_multi_byte = column.get().is_multi_byte();

      if (content.empty())
        return std::vector<std::string>();

      unsigned int top_padding = column.get().top_padding();
      unsigned int bottom_padding = column.get().bottom_padding();

      // split the content into words to easily manipulate it
      auto words = string_utils::split_text(content);

      // the return result
      std::vector<std::string> result;

      // TOP padding
      result.insert(result.end(), top_padding, std::string());

      // e.g: MAX sub size POSSIBLE, - 2 for two sides spaces
      const int usable_width = (column.get().width() - 2);

      // don't go back more than 30%, when the last word is too long
      const int limit = (usable_width * CONTENT_MANIPULATION_BACK_LIMIT);

      std::string sub;
      size_t sub_size = 0;
      for (auto it = words.begin(); it != words.end(); ++it) {
        std::string& word = *it;

        // add existing content if we reach new line
        if (word == "\n") {
          commit_line(result, sub, &sub_size, usable_width, column);
          continue;
        }

        // we need split
        size_t word_size = string_utils::display_width(word, is_multi_byte);
        if ((sub_size + word_size) > usable_width) {
          int remaining_space = usable_width - sub_size;

          // force split
          if (remaining_space > limit) {
            std::string first_part;
            std::string remainder;

            size_t first_part_width = 0;
            if (is_multi_byte) {
              size_t total_display_width = string_utils::safe_split_pos(word, remaining_space - 1);

              // wide word string to walk into `wchar_t`s
              std::wstring wword = conv::utf8stws(word);

              int cut_index = 0;
              for (size_t i = 0; i < wword.length(); i++) {
                size_t w = display_width::wcwidth(wword[i]);
                if (w < 0) w = 0; // just in case u know

                if (first_part_width + w > total_display_width)
                  break;

                first_part_width += w;
                cut_index = i + 1;
              }

              first_part = conv::utf8wsts(wword.substr(0, cut_index)) + '-';
              remainder = conv::utf8wsts(wword.substr(cut_index));
            } else {
              first_part = word.substr(0, remaining_space - 1) + '-';
              remainder = word.substr(remaining_space - 1);

              first_part_width = string_utils::display_width(first_part, is_multi_byte);
            }

            sub += first_part;
            sub_size += first_part_width;

            commit_line(result, sub, &sub_size, usable_width, column);

            words.insert(std::next(it), remainder);
          }
          // commit line and append the word on the next line
          else {
            if (!sub.empty() && sub.back() == ' ') {
              sub.pop_back(); // pop the last space if exist
              sub_size--;
            }

            commit_line(result, sub, &sub_size, usable_width, column);
            --it; // to append the word on the next iteration
          }
        } else {
          sub += word; // add a normall less than line word
          sub_size += word_size;
        }
      }

      // any remaining words
      if (!sub.empty())
        commit_line(result, sub, &sub_size, usable_width, column);

      // BOTTOM padding
      result.insert(result.end(), bottom_padding, std::string());

      return result;
    }

    inline void format_row(unsigned int width, Row& row) {
      for (Column& column : row.columns)
        column.set().splitted_content(
            format_column_lines(column));
    }

    // return the size of the tallest splittedContent vector
    inline size_t find_max_splitted_content_size(const Row& row) {
      size_t result = 0;
      for (Column column : row.columns) {
        size_t splitted_content_size = column.get().splitted_content().size();
        if (splitted_content_size > result)
          result = splitted_content_size;
      }

      return result;
    }

  } // namespace utils

  class Border {
private:
    BorderGlyphs glyphs;

    BorderStyle style;

    BorderGlyphs colors;
    BorderGlyphs background_colors;

    bool is_multi_byte;

    class Getters {
      Border& border;

  public:
      Getters(Border& border) : border(border) {}

      BorderStyle style() { return border.style; }

      BorderGlyphs glyphs() {
        static const std::map<BorderStyle, BorderGlyphs> styleTemplates{
            {BorderStyle::empty, {" ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "}},
            {BorderStyle::standard, {"|", "-", "+", "+", "+", "+", "+", "+", "+", "+", "+"}},
            {BorderStyle::ANSI, {
                                    TABLE_MODE "x" RESET_TABLE_MODE, // vertical
                                    TABLE_MODE "q" RESET_TABLE_MODE, // horizontal
                                    TABLE_MODE "j" RESET_TABLE_MODE, // bottom_right_corner
                                    TABLE_MODE "k" RESET_TABLE_MODE, // top_right_corner
                                    TABLE_MODE "l" RESET_TABLE_MODE, // top_left_corner
                                    TABLE_MODE "m" RESET_TABLE_MODE, // bottom_left_corner
                                    TABLE_MODE "n" RESET_TABLE_MODE, // intersection
                                    TABLE_MODE "t" RESET_TABLE_MODE, // left_connector
                                    TABLE_MODE "u" RESET_TABLE_MODE, // right_connector
                                    TABLE_MODE "v" RESET_TABLE_MODE, // bottom_connector
                                    TABLE_MODE "w" RESET_TABLE_MODE  // top_connector
                                }}};

        const BorderGlyphs& def = styleTemplates.at(border.style);
        BorderGlyphs result;

        auto pick = [](const std::string& val, const std::string& fallback) -> std::string {
          return val.empty() ? fallback : val;
        };

        auto wrap = [](const std::string& fg, const std::string& bg, const std::string& val) -> std::string {
          if (fg.empty() && bg.empty()) return val;
          return fg + bg + val + RESET;
        };

        result.vertical = wrap(border.colors.vertical,
                               border.background_colors.vertical,
                               pick(border.glyphs.vertical, def.vertical));

        result.horizontal = wrap(border.colors.horizontal,
                                 border.background_colors.horizontal,
                                 pick(border.glyphs.horizontal, def.horizontal));

        result.bottom_right_corner = wrap(border.colors.bottom_right_corner,
                                          border.background_colors.bottom_right_corner,
                                          pick(border.glyphs.bottom_right_corner, def.bottom_right_corner));

        result.top_right_corner = wrap(border.colors.top_right_corner,
                                       border.background_colors.top_right_corner,
                                       pick(border.glyphs.top_right_corner, def.top_right_corner));

        result.top_left_corner = wrap(border.colors.top_left_corner,
                                      border.background_colors.top_left_corner,
                                      pick(border.glyphs.top_left_corner, def.top_left_corner));

        result.bottom_left_corner = wrap(border.colors.bottom_left_corner,
                                         border.background_colors.bottom_left_corner,
                                         pick(border.glyphs.bottom_left_corner, def.bottom_left_corner));

        result.intersection = wrap(border.colors.intersection,
                                   border.background_colors.intersection,
                                   pick(border.glyphs.intersection, def.intersection));

        result.left_connector = wrap(border.colors.left_connector,
                                     border.background_colors.left_connector,
                                     pick(border.glyphs.left_connector, def.left_connector));

        result.right_connector = wrap(border.colors.right_connector,
                                      border.background_colors.right_connector,
                                      pick(border.glyphs.right_connector, def.right_connector));

        result.bottom_connector = wrap(border.colors.bottom_connector,
                                       border.background_colors.bottom_connector,
                                       pick(border.glyphs.bottom_connector, def.bottom_connector));

        result.top_connector = wrap(border.colors.top_connector,
                                    border.background_colors.top_connector,
                                    pick(border.glyphs.top_connector, def.top_connector));

        return result;
      }

      std::string horizontal() const {
        return border.glyphs.horizontal;
      }

      std::string vertical() const {
        return border.glyphs.vertical;
      }

      std::string top_left_corner() const {
        return border.glyphs.top_left_corner;
      }

      std::string top_right_corner() const {
        return border.glyphs.top_right_corner;
      }

      std::string bottom_left_corner() const {
        return border.glyphs.bottom_left_corner;
      }

      std::string bottom_right_corner() const {
        return border.glyphs.bottom_right_corner;
      }

      std::string intersection() const {
        return border.glyphs.intersection;
      }

      std::string left_connector() const {
        return border.glyphs.left_connector;
      }

      std::string right_connector() const {
        return border.glyphs.right_connector;
      }

      std::string top_connector() const {
        return border.glyphs.top_connector;
      }

      std::string bottom_connector() const {
        return border.glyphs.bottom_connector;
      }
    };

    class Setters {
      Border& border;

  public:
      Setters(Border& border) : border(border) {}

      /*
       * Note: It is the user's responsibility to ensure that each border glyph is a single
       * terminal column wide. If a string with a visual width > 1 is provided, the table
       * layout may break.
       *
       * Using str.length() is unreliable for multi-byte characters, and calling utils::mbswidth()
       * on every assignment would introduce unnecessary performance overhead. Supporting accurate
       * width checks would also require UTF-8 decoding and wide character handling, which is
       * beyond the scope of this setter.
       *
       * Since this library is aimed at developers, it assumes users understand the implications
       * of using multi-byte or wide characters in border definitions.
       *
       * — Anas Hamdane, 2025-06-09
       */

      Setters& style(BorderStyle style) {
        border.style = style;
        return *this;
      }

      Setters& horizontal(std::string horizontal) {
        border.glyphs.horizontal = horizontal;
        return *this;
      }

      Setters& vertical(std::string vertical) {
        border.glyphs.vertical = vertical;
        return *this;
      }

      Setters& corner(std::string corner) {
        border.glyphs.bottom_right_corner = corner;
        border.glyphs.top_right_corner = corner;

        border.glyphs.bottom_left_corner = corner;
        border.glyphs.top_left_corner = corner;
        return *this;
      }

      Setters& top_left_corner(std::string corner) {
        border.glyphs.top_left_corner = corner;
        return *this;
      }

      Setters& top_right_corner(std::string corner) {
        border.glyphs.top_right_corner = corner;
        return *this;
      }

      Setters& bottom_left_corner(std::string corner) {
        border.glyphs.bottom_left_corner = corner;
        return *this;
      }

      Setters& bottom_right_corner(std::string corner) {
        border.glyphs.bottom_right_corner = corner;
        return *this;
      }

      Setters intersection(std::string intersection) {
        border.glyphs.intersection = intersection;
        return *this;
      }

      Setters& left_connector(std::string connector) {
        border.glyphs.left_connector = connector;
        return *this;
      }

      Setters& right_connector(std::string connector) {
        border.glyphs.right_connector = connector;
        return *this;
      }

      Setters& top_connector(std::string connector) {
        border.glyphs.top_connector = connector;
        return *this;
      }

      Setters& bottom_connector(std::string connector) {
        border.glyphs.bottom_connector = connector;
        return *this;
      }
    };

    class Coloring {
      Border& border;

  public:
      Coloring(Border& border) : border(border) {}

      /* ------------------ full ------------------------ */
      Coloring& full(Color color) {
        horizontal(color);
        vertical(color);

        return *this;
      }

      Coloring& full(RGB rgb) {
        horizontal(rgb);
        vertical(rgb);

        return *this;
      }

      /* ------------------ horizontal ------------------------ */
      Coloring& horizontal(Color color) {
        if (!border.colors.horizontal.empty())
          border.colors.horizontal.clear();

        border.colors.horizontal = CSI + std::to_string(static_cast<int>(color)) + "m";

        top_connector(color);
        bottom_connector(color);
        intersection(color);

        corners(color);

        return *this;
      }

      Coloring& horizontal(RGB rgb) {
        if (!border.colors.horizontal.empty())
          border.colors.horizontal.clear();

        border.colors.horizontal = CSI "38;2;" + std::to_string(rgb.r) + ";" +
                                   std::to_string(rgb.g) + ";" +
                                   std::to_string(rgb.b) + "m";

        top_connector(rgb);
        bottom_connector(rgb);
        intersection(rgb);

        corners(rgb);

        return *this;
      }

      Coloring& bottom_connector(Color color) {
        if (!border.colors.bottom_connector.empty())
          border.colors.bottom_connector.clear();

        border.colors.bottom_connector = CSI + std::to_string(static_cast<int>(color)) + "m";

        return *this;
      }

      Coloring& bottom_connector(RGB rgb) {
        if (!border.colors.bottom_connector.empty())
          border.colors.bottom_connector.clear();

        border.colors.bottom_connector = CSI "38;2;" + std::to_string(rgb.r) + ";" +
                                         std::to_string(rgb.g) + ";" +
                                         std::to_string(rgb.b) + "m";

        return *this;
      }

      Coloring& top_connector(Color color) {
        if (!border.colors.top_connector.empty())
          border.colors.top_connector.clear();

        border.colors.top_connector = CSI + std::to_string(static_cast<int>(color)) + "m";

        return *this;
      }

      Coloring& top_connector(RGB rgb) {
        if (!border.colors.top_connector.empty())
          border.colors.top_connector.clear();

        border.colors.top_connector = CSI "38;2;" + std::to_string(rgb.r) + ";" +
                                      std::to_string(rgb.g) + ";" +
                                      std::to_string(rgb.b) + "m";

        return *this;
      }

      /* ------------------ vertical ------------------------ */
      Coloring& vertical(Color color) {
        if (!border.colors.vertical.empty())
          border.colors.vertical.clear();

        border.colors.vertical = CSI + std::to_string(static_cast<int>(color)) + "m";

        left_connector(color);
        right_connector(color);

        corners(color);

        return *this;
      }

      Coloring& vertical(RGB rgb) {
        if (!border.colors.vertical.empty())
          border.colors.vertical.clear();

        border.colors.vertical = CSI "38;2;" + std::to_string(rgb.r) + ";" +
                                 std::to_string(rgb.g) + ";" +
                                 std::to_string(rgb.b) + "m";

        left_connector(rgb);
        right_connector(rgb);

        corners(rgb);

        return *this;
      }

      Coloring& left_connector(Color color) {
        if (!border.colors.left_connector.empty())
          border.colors.left_connector.clear();

        border.colors.left_connector = CSI + std::to_string(static_cast<int>(color)) + "m";

        return *this;
      }

      Coloring& left_connector(RGB rgb) {
        if (!border.colors.left_connector.empty())
          border.colors.left_connector.clear();

        border.colors.left_connector = CSI "38;2;" + std::to_string(rgb.r) + ";" +
                                       std::to_string(rgb.g) + ";" +
                                       std::to_string(rgb.b) + "m";

        return *this;
      }

      Coloring& right_connector(Color color) {
        if (!border.colors.right_connector.empty())
          border.colors.right_connector.clear();

        border.colors.right_connector = CSI + std::to_string(static_cast<int>(color)) + "m";

        return *this;
      }

      Coloring& right_connector(RGB rgb) {
        if (!border.colors.right_connector.empty())
          border.colors.right_connector.clear();

        border.colors.right_connector = CSI "38;2;" + std::to_string(rgb.r) + ";" +
                                        std::to_string(rgb.g) + ";" +
                                        std::to_string(rgb.b) + "m";

        return *this;
      }

      /* ------------------ corners ------------------------ */
      Coloring& corners(Color color) {
        top_left_corner(color);
        top_right_corner(color);

        bottom_left_corner(color);
        bottom_right_corner(color);

        return *this;
      }

      Coloring& corners(RGB rgb) {
        top_left_corner(rgb);
        top_right_corner(rgb);

        bottom_left_corner(rgb);
        bottom_right_corner(rgb);

        return *this;
      }

      Coloring& top_left_corner(Color color) {
        if (!border.colors.top_left_corner.empty())
          border.colors.top_left_corner.clear();

        border.colors.top_left_corner = CSI + std::to_string(static_cast<int>(color)) + "m";

        return *this;
      }

      Coloring& top_left_corner(RGB rgb) {
        if (!border.colors.top_left_corner.empty())
          border.colors.top_left_corner.clear();

        border.colors.top_left_corner = CSI "38;2;" + std::to_string(rgb.r) + ";" +
                                        std::to_string(rgb.g) + ";" +
                                        std::to_string(rgb.b) + "m";

        return *this;
      }

      Coloring& top_right_corner(Color color) {
        if (!border.colors.top_right_corner.empty())
          border.colors.top_right_corner.clear();

        border.colors.top_right_corner = CSI + std::to_string(static_cast<int>(color)) + "m";

        return *this;
      }

      Coloring& top_right_corner(RGB rgb) {
        if (!border.colors.top_right_corner.empty())
          border.colors.top_right_corner.clear();

        border.colors.top_right_corner = CSI "38;2;" + std::to_string(rgb.r) + ";" +
                                         std::to_string(rgb.g) + ";" +
                                         std::to_string(rgb.b) + "m";

        return *this;
      }

      Coloring& bottom_left_corner(Color color) {
        if (!border.colors.bottom_left_corner.empty())
          border.colors.bottom_left_corner.clear();

        border.colors.bottom_left_corner = CSI + std::to_string(static_cast<int>(color)) + "m";

        return *this;
      }

      Coloring& bottom_left_corner(RGB rgb) {
        if (!border.colors.bottom_left_corner.empty())
          border.colors.bottom_left_corner.clear();

        border.colors.bottom_left_corner = CSI "38;2;" + std::to_string(rgb.r) + ";" +
                                           std::to_string(rgb.g) + ";" +
                                           std::to_string(rgb.b) + "m";

        return *this;
      }

      Coloring& bottom_right_corner(Color color) {
        if (!border.colors.bottom_right_corner.empty())
          border.colors.bottom_right_corner.clear();

        border.colors.bottom_right_corner = CSI + std::to_string(static_cast<int>(color)) + "m";

        return *this;
      }

      Coloring& bottom_right_corner(RGB rgb) {
        if (!border.colors.bottom_right_corner.empty())
          border.colors.bottom_right_corner.clear();

        border.colors.bottom_right_corner = CSI "38;2;" + std::to_string(rgb.r) + ";" +
                                            std::to_string(rgb.g) + ";" +
                                            std::to_string(rgb.b) + "m";

        return *this;
      }

      /* ------------------ intersection point ------------------------ */
      Coloring& intersection(Color color) {
        if (!border.colors.intersection.empty())
          border.colors.intersection.clear();

        border.colors.intersection = CSI + std::to_string(static_cast<int>(color)) + "m";

        return *this;
      }

      Coloring& intersection(RGB rgb) {
        if (!border.colors.intersection.empty())
          border.colors.intersection.clear();

        border.colors.intersection = CSI "38;2;" + std::to_string(rgb.r) + ";" +
                                     std::to_string(rgb.g) + ";" +
                                     std::to_string(rgb.b) + "m";

        return *this;
      }
    };

    class BackgroundColoring {
      Border& border;

  public:
      BackgroundColoring(Border& border) : border(border) {}

      /* ------------------ full ------------------------ */
      BackgroundColoring& full(Color background_color) {
        horizontal(background_color);
        vertical(background_color);

        return *this;
      }

      BackgroundColoring& full(RGB rgb) {
        horizontal(rgb);
        vertical(rgb);

        return *this;
      }

      /* ------------------ horizontal ------------------------ */
      BackgroundColoring& horizontal(Color background_color) {
        if (!border.background_colors.horizontal.empty())
          border.background_colors.horizontal.clear();

        border.background_colors.horizontal = CSI + std::to_string(static_cast<int>(background_color) + 10) + "m";

        top_connector(background_color);
        bottom_connector(background_color);

        corners(background_color);
        return *this;
      }

      BackgroundColoring& horizontal(RGB rgb) {
        if (!border.background_colors.horizontal.empty())
          border.background_colors.horizontal.clear();

        border.background_colors.horizontal = CSI "48;2;" + std::to_string(rgb.r) + ";" +
                                              std::to_string(rgb.g) + ";" +
                                              std::to_string(rgb.b) + "m";

        top_connector(rgb);
        bottom_connector(rgb);

        corners(rgb);

        return *this;
      }

      BackgroundColoring& bottom_connector(Color background_color) {
        if (!border.background_colors.bottom_connector.empty())
          border.background_colors.bottom_connector.clear();

        border.background_colors.bottom_connector = CSI + std::to_string(static_cast<int>(background_color) + 10) + "m";

        return *this;
      }

      BackgroundColoring& bottom_connector(RGB rgb) {
        if (!border.background_colors.bottom_connector.empty())
          border.background_colors.bottom_connector.clear();

        border.background_colors.bottom_connector = CSI "48;2;" + std::to_string(rgb.r) + ";" +
                                                    std::to_string(rgb.g) + ";" +
                                                    std::to_string(rgb.b) + "m";

        return *this;
      }

      BackgroundColoring& top_connector(Color background_color) {
        if (!border.background_colors.top_connector.empty())
          border.background_colors.top_connector.clear();

        border.background_colors.top_connector = CSI + std::to_string(static_cast<int>(background_color) + 10) + "m";

        return *this;
      }

      BackgroundColoring& top_connector(RGB rgb) {
        if (!border.background_colors.top_connector.empty())
          border.background_colors.top_connector.clear();

        border.background_colors.top_connector = CSI "48;2;" + std::to_string(rgb.r) + ";" +
                                                 std::to_string(rgb.g) + ";" +
                                                 std::to_string(rgb.b) + "m";

        return *this;
      }

      /* ------------------ vertical ------------------------ */
      BackgroundColoring& vertical(Color background_color) {
        if (!border.background_colors.vertical.empty())
          border.background_colors.vertical.clear();

        border.background_colors.vertical = CSI + std::to_string(static_cast<int>(background_color) + 10) + "m";

        left_connector(background_color);
        left_connector(background_color);

        corners(background_color);

        return *this;
      }

      BackgroundColoring& vertical(RGB rgb) {
        if (!border.background_colors.vertical.empty())
          border.background_colors.vertical.clear();

        border.background_colors.vertical = CSI "48;2;" + std::to_string(rgb.r) + ";" +
                                            std::to_string(rgb.g) + ";" +
                                            std::to_string(rgb.b) + "m";

        left_connector(rgb);
        left_connector(rgb);

        corners(rgb);

        return *this;
      }

      BackgroundColoring& left_connector(Color background_color) {
        if (!border.background_colors.left_connector.empty())
          border.background_colors.left_connector.clear();

        border.background_colors.left_connector = CSI + std::to_string(static_cast<int>(background_color) + 10) + "m";

        return *this;
      }

      BackgroundColoring& left_connector(RGB rgb) {
        if (!border.background_colors.left_connector.empty())
          border.background_colors.left_connector.clear();

        border.background_colors.left_connector = CSI "48;2;" + std::to_string(rgb.r) + ";" +
                                                  std::to_string(rgb.g) + ";" +
                                                  std::to_string(rgb.b) + "m";

        return *this;
      }

      BackgroundColoring& right_connector(Color background_color) {
        if (!border.background_colors.right_connector.empty())
          border.background_colors.right_connector.clear();

        border.background_colors.right_connector = CSI + std::to_string(static_cast<int>(background_color) + 10) + "m";

        return *this;
      }

      BackgroundColoring& right_connector(RGB rgb) {
        if (!border.background_colors.right_connector.empty())
          border.background_colors.right_connector.clear();

        border.background_colors.right_connector = CSI "48;2;" + std::to_string(rgb.r) + ";" +
                                                   std::to_string(rgb.g) + ";" +
                                                   std::to_string(rgb.b) + "m";

        return *this;
      }

      /* ------------------ corners ------------------------ */
      BackgroundColoring& corners(Color background_color) {
        top_left_corner(background_color);
        top_right_corner(background_color);

        bottom_left_corner(background_color);
        bottom_right_corner(background_color);

        return *this;
      }

      BackgroundColoring& corners(RGB rgb) {
        top_left_corner(rgb);
        top_right_corner(rgb);

        bottom_left_corner(rgb);
        bottom_right_corner(rgb);

        return *this;
      }

      BackgroundColoring& top_left_corner(Color background_color) {
        if (!border.background_colors.top_left_corner.empty())
          border.background_colors.top_left_corner.clear();

        border.background_colors.top_left_corner = CSI + std::to_string(static_cast<int>(background_color) + 10) + "m";

        return *this;
      }

      BackgroundColoring& top_left_corner(RGB rgb) {
        if (!border.background_colors.top_left_corner.empty())
          border.background_colors.top_left_corner.clear();

        border.background_colors.top_left_corner = CSI "48;2;" + std::to_string(rgb.r) + ";" +
                                                   std::to_string(rgb.g) + ";" +
                                                   std::to_string(rgb.b) + "m";

        return *this;
      }

      BackgroundColoring& top_right_corner(Color background_color) {
        if (!border.background_colors.top_right_corner.empty())
          border.background_colors.top_right_corner.clear();

        border.background_colors.top_right_corner = CSI + std::to_string(static_cast<int>(background_color) + 10) + "m";

        return *this;
      }

      BackgroundColoring& top_right_corner(RGB rgb) {
        if (!border.background_colors.top_right_corner.empty())
          border.background_colors.top_right_corner.clear();

        border.background_colors.top_right_corner = CSI "48;2;" + std::to_string(rgb.r) + ";" +
                                                    std::to_string(rgb.g) + ";" +
                                                    std::to_string(rgb.b) + "m";

        return *this;
      }

      BackgroundColoring& bottom_left_corner(Color background_color) {
        if (!border.background_colors.bottom_left_corner.empty())
          border.background_colors.bottom_left_corner.clear();

        border.background_colors.bottom_left_corner = CSI + std::to_string(static_cast<int>(background_color) + 10) + "m";

        return *this;
      }

      BackgroundColoring& bottom_left_corner(RGB rgb) {
        if (!border.background_colors.bottom_left_corner.empty())
          border.background_colors.bottom_left_corner.clear();

        border.background_colors.bottom_left_corner = CSI "48;2;" + std::to_string(rgb.r) + ";" +
                                                      std::to_string(rgb.g) + ";" +
                                                      std::to_string(rgb.b) + "m";

        return *this;
      }

      BackgroundColoring& bottom_right_corner(Color background_color) {
        if (!border.background_colors.bottom_right_corner.empty())
          border.background_colors.bottom_right_corner.clear();

        border.background_colors.bottom_right_corner = CSI + std::to_string(static_cast<int>(background_color) + 10) + "m";

        return *this;
      }

      BackgroundColoring& bottom_right_corner(RGB rgb) {
        if (!border.background_colors.bottom_right_corner.empty())
          border.background_colors.bottom_right_corner.clear();

        border.background_colors.bottom_right_corner = CSI "48;2;" + std::to_string(rgb.r) + ";" +
                                                       std::to_string(rgb.g) + ";" +
                                                       std::to_string(rgb.b) + "m";

        return *this;
      }

      /* ------------------ intersection point ------------------------ */
      BackgroundColoring& intersection(Color background_color) {
        if (!border.background_colors.intersection.empty())
          border.background_colors.intersection.clear();

        border.background_colors.intersection = CSI + std::to_string(static_cast<int>(background_color) + 10) + "m";

        return *this;
      }

      BackgroundColoring& intersection(RGB rgb) {
        if (!border.background_colors.intersection.empty())
          border.background_colors.intersection.clear();

        border.background_colors.intersection = CSI "48;2;" + std::to_string(rgb.r) + ";" +
                                                std::to_string(rgb.g) + ";" +
                                                std::to_string(rgb.b) + "m";

        return *this;
      }
    };

public:
    Border() : style(BorderStyle::standard) {}

    Getters get() { return Getters(*this); }

    Setters set() { return Setters(*this); }

    Coloring color() { return Coloring(*this); }

    BackgroundColoring background_color() { return BackgroundColoring(*this); }
  };

  class Table {
    Border table_border;
    unsigned int width;
    bool forced_width;
    bool force_ansi;

    float width_percent;

    std::vector<Row> rows;

    class Config {
      Table& table;

  public:
      Config(Table& table) : table(table) {}

      /* -----------------ALIGNMENT--------------------- */
      Config& alignment(Alignment align) {
        for (Row& row : table.rows)
          row.config().alignment(align);

        return *this;
      }

      /* -----------------PADDING--------------------- */
      Config& padding(int pad) {
        if (pad < 0)
          return *this; // Invalid padding

        for (Row& row : table.rows)
          row.config().padding(pad);

        return *this;
      }

      /* -----------------TOP_PADDING--------------------- */
      Config& top_padding(int pad) {
        if (pad < 0)
          return *this; // Invalid padding

        for (Row& row : table.rows)
          row.config().top_padding(pad);

        return *this;
      }

      /* -----------------BOTTOM_PADDING--------------------- */
      Config& bottom_padding(int pad) {
        if (pad < 0)
          return *this; // Invalid padding

        for (Row& row : table.rows)
          row.config().bottom_padding(pad);

        return *this;
      }

      /* -----------------WIDTH--------------------- */
      Config& table_width(int width) {
        if (width <= 0)
          return *this; // Invalid width

        table.width = width;

        return *this;
      }

      /* -------------------COLORS------------------------- */
      Config& color(Color color) {
        for (Row& row : table.rows)
          row.config().color(color);

        return *this;
      }

      Config& color(RGB rgb) {
        for (Row& row : table.rows)
          row.config().color(rgb);

        return *this;
      }

      /* ---------------BACKGROUND COLORS--------------------- */
      Config& content_background_color(Color back_color) {
        for (Row& row : table.rows)
          row.config().content_background_color(back_color);

        return *this;
      }

      Config& content_background_color(RGB back_rgb) {
        for (Row& row : table.rows)
          row.config().content_background_color(back_rgb);

        return *this;
      }

      Config& column_background_color(Color back_color) {
        for (Row& row : table.rows)
          row.config().column_background_color(back_color);

        return *this;
      }

      Config& column_background_color(RGB back_rgb) {
        for (Row& row : table.rows)
          row.config().column_background_color(back_rgb);

        return *this;
      }
    };

    class Setters {
      Table& table;

  public:
      Setters(Table& table) : table(table) {}

      Setters& width(int width) {
        if (width > 0)
          table.width = static_cast<unsigned int>(width);
        else
          table.width = 0;

        return *this;
      }

      // * for testing set a fixed, forced width
      Setters& forced_width(int width, bool forced) {
        if (width < 0)
          table.width = 0;
        else
          table.width = static_cast<unsigned int>(width);

        table.forced_width = forced;

        return *this;
      }

      // * for testing force using ansi (used when redirecting output to files)
      Setters& forced_ansi(bool forced) {
        table.table_border.set().style(BorderStyle::ANSI);

        table.force_ansi = forced;

        return *this;
      }

      Setters& multi_byte_characters(bool is_multi_byte) {
        for (Row& row : table.rows)
          row.set().multi_byte_characters(is_multi_byte);

        return *this;
        ;
      }

      Setters& width_percent(float percent) {
        if (percent <= 1 && percent > 0)
          table.width_percent = percent;
          
        return *this;
      }
    };

    class Getters {
      Table& table;

      public:
      Getters(Table& table) : table(table) {}

      std::vector<Row> rows() { return table.rows; }

      unsigned int width() { return table.width; }
    };

    class Columns {
      std::vector<Row>& rows;
      int index;

      class Config {
        std::vector<Row>& rows;
        int index;

    public:
        Config(std::vector<Row>& rows, int index) : rows(rows), index(index) {}

        Config& alignment(Alignment align) {
          for (Row& row : rows)
            if (index >= 0 && index < static_cast<int>(row.columns.size()))
              row.columns[index].config().alignment(align);

          return *this;
        }

        Config& width(int w) {
          unsigned int safe_width = (w <= 0) ? 0 : static_cast<unsigned int>(w);

          for (Row& row : rows)
            if (index >= 0 && index < static_cast<int>(row.columns.size()))
              row.columns[index].config().width(safe_width);

          return *this;
        }

        Config& padding(int padd) {
          unsigned int safe_padding = (padd <= 0) ? 0 : static_cast<unsigned int>(padd);

          for (Row& row : rows) {
            if (index >= 0 && index < static_cast<int>(row.columns.size())) {
              row.columns[index].config().top_padding(safe_padding);
              row.columns[index].config().bottom_padding(safe_padding);
            }
          }

          return *this;
        }

        Config& top_padding(int padd) {
          unsigned int safe_padding = (padd <= 0) ? 0 : static_cast<unsigned int>(padd);

          for (Row& row : rows)
            if (index >= 0 && index < static_cast<int>(row.columns.size()))
              row.columns[index].config().top_padding(safe_padding);

          return *this;
        }

        Config& bottom_padding(int padd) {
          unsigned int safe_padding = (padd <= 0) ? 0 : static_cast<unsigned int>(padd);

          for (Row& row : rows)
            if (index >= 0 && index < static_cast<int>(row.columns.size()))
              row.columns[index].config().bottom_padding(safe_padding);

          return *this;
        }

        Config& text_style(Style style) {
          for (Row& row : rows)
            if (index >= 0 && index < static_cast<int>(row.columns.size()))
              row.columns[index].config().text_style(style);

          return *this;
        }

        Config& text_style(const std::vector<Style>& styles) {
          for (Row& row : rows)
            if (index >= 0 && index < static_cast<int>(row.columns.size()))
              row.columns[index].config().text_style(styles);

          return *this;
        }

        Config& color(Color color) {
          for (Row& row : rows)
            if (index >= 0 && index < static_cast<int>(row.columns.size()))
              row.columns[index].config().color(color);

          return *this;
        }

        Config& color(RGB rgb) {
          for (Row& row : rows)
            if (index >= 0 && index < static_cast<int>(row.columns.size()))
              row.columns[index].config().color(rgb);

          return *this;
        }

        Config& content_background_color(Color color) {
          for (Row& row : rows)
            if (index >= 0 && index < static_cast<int>(row.columns.size()))
              row.columns[index].config().content_background_color(color);

          return *this;
        }

        Config& content_background_color(RGB rgb) {
          for (Row& row : rows)
            if (index >= 0 && index < static_cast<int>(row.columns.size()))
              row.columns[index].config().content_background_color(rgb);

          return *this;
        }

        Config& column_background_color(Color color) {
          for (Row& row : rows)
            if (index >= 0 && index < static_cast<int>(row.columns.size()))
              row.columns[index].config().column_background_color(color);

          return *this;
        }

        Config& column_background_color(RGB rgb) {
          for (Row& row : rows)
            if (index >= 0 && index < static_cast<int>(row.columns.size()))
              row.columns[index].config().column_background_color(rgb);

          return *this;
        }
      };

  public:
      Columns(std::vector<Row>& rows, int index) : rows(rows), index(index) {
      }

      Config config() { return Config(rows, index); }
    };

    std::vector<int> find_stops(Row row) {
      std::vector<int> result;

      int track = 1;
      for (Column column : row.columns) {
        track += column.get().width();

        result.push_back(int(track));
        track++;
      }

      return result;
    }

    void print_row(std::ostream& stream, Row& row, BorderGlyphs glyphs) {
      std::string vertical = glyphs.vertical;

      size_t max_splitted_content_size = utils::find_max_splitted_content_size(row); // tallest vector of splitted strings
      for (unsigned int i = 0; i < max_splitted_content_size; i++) {
        // side border
        stream << '\n'
               << vertical;

        // printing the n element of splitted_content for each column
        for (Column column : row.columns) {
          std::string reset = RESET;

          int rest = column.get().width();
          int splitted_content_size = column.get().splitted_content().size();
          std::string current_line;

          // column background
          std::string column_background_color = column.get().column_background_color();
          stream << column_background_color;

          if (i < splitted_content_size) {
            current_line = column.get().splitted_content().at(i);
            stream << current_line;

            std::string styles = column.get().text_styles() + column.get().content_color() + column.get().content_background_color();
            int special_characters = 0;

            // for each splitted_content element has a one or more global_styles, it has one RESET at the end
            if (!(styles.empty() || current_line.empty())) {
              special_characters += reset.size();
              special_characters += styles.size();
            }

            int curr_line_size = string_utils::display_width(current_line, column.get().is_multi_byte());

            // special_characters will not be displayed so they are not counted
            rest -= (curr_line_size - special_characters); // to balance the line
          }

          stream << column_background_color;

          for (int k = 0; k < rest; k++)
            stream << ' ';

          if (!column_background_color.empty())
            stream << reset;

          stream << vertical;
        }
      }
    }

    void print_border(std::ostream& stream, std::vector<Row>::iterator& it, BorderGlyphs border_glyphs, bool is_first, bool is_last, bool regular) {

      Row reference = *it;
      std::vector<int> next_row_corners;

      if (is_first)
        next_row_corners = find_stops(*it);
      else if (!is_last)
        next_row_corners = find_stops(*(it + 1));

      if (!is_first)
        stream << '\n';

      std::string horizontal = border_glyphs.horizontal;

      // (vertical separators)/corners
      std::string left_corner;
      std::string right_corner;

      // horizontal separator
      std::string middle_separator;
      std::string top_to_bottom;
      std::string bottom_to_top;

      if (is_first) {
        left_corner = border_glyphs.top_left_corner;
        right_corner = border_glyphs.top_right_corner;

        middle_separator = border_glyphs.top_connector;
        top_to_bottom = border_glyphs.top_connector;
        bottom_to_top = border_glyphs.top_connector;
      } else if (is_last) {
        left_corner = border_glyphs.bottom_left_corner;
        right_corner = border_glyphs.bottom_right_corner;

        middle_separator = border_glyphs.bottom_connector;
        top_to_bottom = border_glyphs.bottom_connector;
        bottom_to_top = border_glyphs.bottom_connector;
      } else {
        left_corner = border_glyphs.left_connector;
        right_corner = border_glyphs.right_connector;

        middle_separator = border_glyphs.intersection;
        top_to_bottom = border_glyphs.top_connector;
        bottom_to_top = border_glyphs.bottom_connector;
      }

      stream << left_corner;

      size_t cols_num = reference.columns.size();

      int tracker = 0;
      for (size_t j = 0; j < cols_num; j++) {
        Column column = reference.columns[j];
        unsigned col_width = column.get().width();

        for (unsigned int k = 0; k < col_width; k++) {
          tracker++;

          if (std::find(next_row_corners.begin(), next_row_corners.end(), tracker) != next_row_corners.end())
            stream << top_to_bottom;
          else
            stream << horizontal;
        }

        tracker++;

        if (j + 1 != cols_num) {

          // column separator at the bottom and at the top
          if (std::find(next_row_corners.begin(), next_row_corners.end(), tracker) != next_row_corners.end())
            stream << middle_separator;
          else
            stream << bottom_to_top; // just at the top
        }
      }

      stream << right_corner;
    }

    /* Regularity means it has the same number of columns in each row */
    bool is_regular() {
      size_t reference = rows[0].columns.size();
      size_t rows_num = rows.size();

      for (int i = 0; i < rows_num; i++)
        if (rows[i].columns.size() != reference)
          return false;

      return true;
    }

    inline void adjust_widths(unsigned int table_width) {
      if (table_width <= 0)
        return;

      for (Row& row : this->rows) {
        size_t columns_num = row.columns.size();
        if (columns_num == 0)
          continue;

        unsigned int table_usable_width = table_width - (columns_num + 1);

        for (Column& column : row.columns) {
          if (column.get().width() != 0) {
            table_usable_width -= column.get().width();
            columns_num--;
          }
        }

        int indiv_column_width = table_usable_width / columns_num;
        int rest = table_usable_width % columns_num;

        for (Column& column : row.columns) {
          unsigned int width = column.get().width();

          if (width == 0) {
            width = indiv_column_width + (rest > 0 ? 1 : 0);
            if (rest > 0) rest--;
            column.set().width(width);
          }

          /* no need because it is the work of format_row() :) */
          // column.set().splitted_content(utils::format_column_lines(column, width));
        }
      }
    }

public:
    Table() : width(0), forced_width(false), width_percent(0.5) {}

    // configure the table
    Config config() { return Config(*this); }

    Border& border() { return table_border; }

    Setters set() { return Setters(*this); }

    Getters get() { return Getters(*this); }

    Columns column(int index) {
      return Columns(rows, index);
    }

    Row& row(int index) {
      return rows.at(index); // Throws std::out_of_range if index is invalid
    }

    Table& add_row(std::vector<std::string> contents) {
      std::vector<Column> columns;
      for (std::string content : contents)
        columns.push_back(Column(content));

      rows.push_back(Row(columns));

      return *this;
    }

    // * return 2 for empty rows and 3 for terminal space problems
    int print(std::ostream& stream) {
      if (rows.empty())
        return 2;

      // chose width to use
      unsigned int usable_width;
      if (forced_width)
        usable_width = width;
      else {
        unsigned short terminal_width = utils::get_terminal_width();
        usable_width = terminal_width * width_percent;
        if (this->width <= 0 || this->width > terminal_width)
          width = usable_width;
        else
          usable_width = width;
      }

      // check if the table has consistent number of columns across all rows
      bool regular = is_regular();

      // adjust the width of all columns once
      adjust_widths(usable_width);

      for (Row& row : rows) {
        size_t cols_num = row.columns.size();
        size_t row_usable_width = usable_width - (cols_num + 1); // ... - tableSplits

        if (row_usable_width <= cols_num)
          return 3;

        utils::format_row(row_usable_width, row);
      }

      if (table_border.get().style() == BorderStyle::ANSI && !utils::check_terminal() && !force_ansi)
        table_border.set().style(BorderStyle::standard);

      BorderGlyphs glyphs = table_border.get().glyphs();

      /* ------ printing the table ------- */
      bool is_first = true, is_last = false;
      if (rows.size() == 1)
        is_last = true;

      auto it = rows.begin();
      print_border(stream, it, glyphs, is_first, is_last, regular);

      is_first = false;
      for (it = rows.begin(); it != rows.end(); ++it) {
        Row& row = *it;

        print_row(stream, row, glyphs);

        if ((it + 1) == rows.end())
          is_last = true;

        print_border(stream, it, glyphs, is_first, is_last, regular);
      }

      return 0;
    }

    Row& operator[](int index) {
      return this->rows.at(index);
    }
  };

  inline std::ostream& operator<<(std::ostream& stream, const Table& table) {
    const_cast<Table&>(table).print(stream);
    return stream;
  }
} // namespace tabular
