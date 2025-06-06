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
      -  [ ] sub string styling support [VEEEEEEEEEEEEERY HAAAAAAAAAAARD]
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

/* ----------------------The following code copied from utf8stows.hpp -----------------------*/
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

#include <string.h>

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

/* ----------------------The following code copied from wcwidth.hpp -----------------------*/
/* COPIED FROM data/unicode_width_table.h */
struct interval {
  int first;
  int last;
};

static const struct interval combining[] = {
    {0x0000, 0x001F},   {0x007F, 0x009F},   {0x0300, 0x036F},
    {0x0483, 0x0489},   {0x0591, 0x05BD},   {0x05BF, 0x05BF},
    {0x05C1, 0x05C2},   {0x05C4, 0x05C5},   {0x05C7, 0x05C7},
    {0x0600, 0x0605},   {0x0610, 0x061A},   {0x061C, 0x061C},
    {0x064B, 0x065F},   {0x0670, 0x0670},   {0x06D6, 0x06DD},
    {0x06DF, 0x06E4},   {0x06E7, 0x06E8},   {0x06EA, 0x06ED},
    {0x070F, 0x070F},   {0x0711, 0x0711},   {0x0730, 0x074A},
    {0x07A6, 0x07B0},   {0x07EB, 0x07F3},   {0x07FD, 0x07FD},
    {0x0816, 0x0819},   {0x081B, 0x0823},   {0x0825, 0x0827},
    {0x0829, 0x082D},   {0x0859, 0x085B},   {0x0890, 0x0891},
    {0x0897, 0x089F},   {0x08CA, 0x0903},   {0x093A, 0x093C},
    {0x093E, 0x094F},   {0x0951, 0x0957},   {0x0962, 0x0963},
    {0x0981, 0x0983},   {0x09BC, 0x09BC},   {0x09BE, 0x09C4},
    {0x09C7, 0x09C8},   {0x09CB, 0x09CD},   {0x09D7, 0x09D7},
    {0x09E2, 0x09E3},   {0x09FE, 0x09FE},   {0x0A01, 0x0A03},
    {0x0A3C, 0x0A3C},   {0x0A3E, 0x0A42},   {0x0A47, 0x0A48},
    {0x0A4B, 0x0A4D},   {0x0A51, 0x0A51},   {0x0A70, 0x0A71},
    {0x0A75, 0x0A75},   {0x0A81, 0x0A83},   {0x0ABC, 0x0ABC},
    {0x0ABE, 0x0AC5},   {0x0AC7, 0x0AC9},   {0x0ACB, 0x0ACD},
    {0x0AE2, 0x0AE3},   {0x0AFA, 0x0AFF},   {0x0B01, 0x0B03},
    {0x0B3C, 0x0B3C},   {0x0B3E, 0x0B44},   {0x0B47, 0x0B48},
    {0x0B4B, 0x0B4D},   {0x0B55, 0x0B57},   {0x0B62, 0x0B63},
    {0x0B82, 0x0B82},   {0x0BBE, 0x0BC2},   {0x0BC6, 0x0BC8},
    {0x0BCA, 0x0BCD},   {0x0BD7, 0x0BD7},   {0x0C00, 0x0C04},
    {0x0C3C, 0x0C3C},   {0x0C3E, 0x0C44},   {0x0C46, 0x0C48},
    {0x0C4A, 0x0C4D},   {0x0C55, 0x0C56},   {0x0C62, 0x0C63},
    {0x0C81, 0x0C83},   {0x0CBC, 0x0CBC},   {0x0CBE, 0x0CC4},
    {0x0CC6, 0x0CC8},   {0x0CCA, 0x0CCD},   {0x0CD5, 0x0CD6},
    {0x0CE2, 0x0CE3},   {0x0CF3, 0x0CF3},   {0x0D00, 0x0D03},
    {0x0D3B, 0x0D3C},   {0x0D3E, 0x0D44},   {0x0D46, 0x0D48},
    {0x0D4A, 0x0D4D},   {0x0D57, 0x0D57},   {0x0D62, 0x0D63},
    {0x0D81, 0x0D83},   {0x0DCA, 0x0DCA},   {0x0DCF, 0x0DD4},
    {0x0DD6, 0x0DD6},   {0x0DD8, 0x0DDF},   {0x0DF2, 0x0DF3},
    {0x0E31, 0x0E31},   {0x0E34, 0x0E3A},   {0x0E47, 0x0E4E},
    {0x0EB1, 0x0EB1},   {0x0EB4, 0x0EBC},   {0x0EC8, 0x0ECE},
    {0x0F18, 0x0F19},   {0x0F35, 0x0F35},   {0x0F37, 0x0F37},
    {0x0F39, 0x0F39},   {0x0F3E, 0x0F3F},   {0x0F71, 0x0F84},
    {0x0F86, 0x0F87},   {0x0F8D, 0x0F97},   {0x0F99, 0x0FBC},
    {0x0FC6, 0x0FC6},   {0x102B, 0x103E},   {0x1056, 0x1059},
    {0x105E, 0x1060},   {0x1062, 0x1064},   {0x1067, 0x106D},
    {0x1071, 0x1074},   {0x1082, 0x108D},   {0x108F, 0x108F},
    {0x109A, 0x109D},   {0x135D, 0x135F},   {0x1712, 0x1715},
    {0x1732, 0x1734},   {0x1752, 0x1753},   {0x1772, 0x1773},
    {0x17B4, 0x17D3},   {0x17DD, 0x17DD},   {0x180B, 0x180F},
    {0x1885, 0x1886},   {0x18A9, 0x18A9},   {0x1920, 0x192B},
    {0x1930, 0x193B},   {0x1A17, 0x1A1B},   {0x1A55, 0x1A5E},
    {0x1A60, 0x1A7C},   {0x1A7F, 0x1A7F},   {0x1AB0, 0x1ACE},
    {0x1B00, 0x1B04},   {0x1B34, 0x1B44},   {0x1B6B, 0x1B73},
    {0x1B80, 0x1B82},   {0x1BA1, 0x1BAD},   {0x1BE6, 0x1BF3},
    {0x1C24, 0x1C37},   {0x1CD0, 0x1CD2},   {0x1CD4, 0x1CE8},
    {0x1CED, 0x1CED},   {0x1CF4, 0x1CF4},   {0x1CF7, 0x1CF9},
    {0x1DC0, 0x1DFF},   {0x200B, 0x200F},   {0x2028, 0x202E},
    {0x2060, 0x2064},   {0x2066, 0x206F},   {0x20D0, 0x20F0},
    {0x2CEF, 0x2CF1},   {0x2D7F, 0x2D7F},   {0x2DE0, 0x2DFF},
    {0x302A, 0x302D},   {0x3099, 0x309A},   {0xA66F, 0xA672},
    {0xA674, 0xA67D},   {0xA69E, 0xA69F},   {0xA6F0, 0xA6F1},
    {0xA802, 0xA802},   {0xA806, 0xA806},   {0xA80B, 0xA80B},
    {0xA823, 0xA827},   {0xA82C, 0xA82C},   {0xA880, 0xA881},
    {0xA8B4, 0xA8C5},   {0xA8E0, 0xA8F1},   {0xA8FF, 0xA8FF},
    {0xA926, 0xA92D},   {0xA947, 0xA953},   {0xA980, 0xA983},
    {0xA9B3, 0xA9C0},   {0xA9E5, 0xA9E5},   {0xAA29, 0xAA36},
    {0xAA43, 0xAA43},   {0xAA4C, 0xAA4D},   {0xAA7B, 0xAA7D},
    {0xAAB0, 0xAAB0},   {0xAAB2, 0xAAB4},   {0xAAB7, 0xAAB8},
    {0xAABE, 0xAABF},   {0xAAC1, 0xAAC1},   {0xAAEB, 0xAAEF},
    {0xAAF5, 0xAAF6},   {0xABE3, 0xABEA},   {0xABEC, 0xABED},
    {0xD800, 0xDFFF},   {0xFB1E, 0xFB1E},   {0xFE00, 0xFE0F},
    {0xFE20, 0xFE2F},   {0xFEFF, 0xFEFF},   {0xFFF9, 0xFFFB},
    {0x101FD, 0x101FD}, {0x102E0, 0x102E0}, {0x10376, 0x1037A},
    {0x10A01, 0x10A03}, {0x10A05, 0x10A06}, {0x10A0C, 0x10A0F},
    {0x10A38, 0x10A3A}, {0x10A3F, 0x10A3F}, {0x10AE5, 0x10AE6},
    {0x10D24, 0x10D27}, {0x10D69, 0x10D6D}, {0x10EAB, 0x10EAC},
    {0x10EFC, 0x10EFF}, {0x10F46, 0x10F50}, {0x10F82, 0x10F85},
    {0x11000, 0x11002}, {0x11038, 0x11046}, {0x11070, 0x11070},
    {0x11073, 0x11074}, {0x1107F, 0x11082}, {0x110B0, 0x110BA},
    {0x110BD, 0x110BD}, {0x110C2, 0x110C2}, {0x110CD, 0x110CD},
    {0x11100, 0x11102}, {0x11127, 0x11134}, {0x11145, 0x11146},
    {0x11173, 0x11173}, {0x11180, 0x11182}, {0x111B3, 0x111C0},
    {0x111C9, 0x111CC}, {0x111CE, 0x111CF}, {0x1122C, 0x11237},
    {0x1123E, 0x1123E}, {0x11241, 0x11241}, {0x112DF, 0x112EA},
    {0x11300, 0x11303}, {0x1133B, 0x1133C}, {0x1133E, 0x11344},
    {0x11347, 0x11348}, {0x1134B, 0x1134D}, {0x11357, 0x11357},
    {0x11362, 0x11363}, {0x11366, 0x1136C}, {0x11370, 0x11374},
    {0x113B8, 0x113C0}, {0x113C2, 0x113C2}, {0x113C5, 0x113C5},
    {0x113C7, 0x113CA}, {0x113CC, 0x113D0}, {0x113D2, 0x113D2},
    {0x113E1, 0x113E2}, {0x11435, 0x11446}, {0x1145E, 0x1145E},
    {0x114B0, 0x114C3}, {0x115AF, 0x115B5}, {0x115B8, 0x115C0},
    {0x115DC, 0x115DD}, {0x11630, 0x11640}, {0x116AB, 0x116B7},
    {0x1171D, 0x1172B}, {0x1182C, 0x1183A}, {0x11930, 0x11935},
    {0x11937, 0x11938}, {0x1193B, 0x1193E}, {0x11940, 0x11940},
    {0x11942, 0x11943}, {0x119D1, 0x119D7}, {0x119DA, 0x119E0},
    {0x119E4, 0x119E4}, {0x11A01, 0x11A0A}, {0x11A33, 0x11A39},
    {0x11A3B, 0x11A3E}, {0x11A47, 0x11A47}, {0x11A51, 0x11A5B},
    {0x11A8A, 0x11A99}, {0x11C2F, 0x11C36}, {0x11C38, 0x11C3F},
    {0x11C92, 0x11CA7}, {0x11CA9, 0x11CB6}, {0x11D31, 0x11D36},
    {0x11D3A, 0x11D3A}, {0x11D3C, 0x11D3D}, {0x11D3F, 0x11D45},
    {0x11D47, 0x11D47}, {0x11D8A, 0x11D8E}, {0x11D90, 0x11D91},
    {0x11D93, 0x11D97}, {0x11EF3, 0x11EF6}, {0x11F00, 0x11F01},
    {0x11F03, 0x11F03}, {0x11F34, 0x11F3A}, {0x11F3E, 0x11F42},
    {0x11F5A, 0x11F5A}, {0x13430, 0x13440}, {0x13447, 0x13455},
    {0x1611E, 0x1612F}, {0x16AF0, 0x16AF4}, {0x16B30, 0x16B36},
    {0x16F4F, 0x16F4F}, {0x16F51, 0x16F87}, {0x16F8F, 0x16F92},
    {0x16FE4, 0x16FE4}, {0x1BC9D, 0x1BC9E}, {0x1BCA0, 0x1BCA3},
    {0x1CF00, 0x1CF2D}, {0x1CF30, 0x1CF46}, {0x1D165, 0x1D169},
    {0x1D16D, 0x1D182}, {0x1D185, 0x1D18B}, {0x1D1AA, 0x1D1AD},
    {0x1D242, 0x1D244}, {0x1DA00, 0x1DA36}, {0x1DA3B, 0x1DA6C},
    {0x1DA75, 0x1DA75}, {0x1DA84, 0x1DA84}, {0x1DA9B, 0x1DA9F},
    {0x1DAA1, 0x1DAAF}, {0x1E000, 0x1E006}, {0x1E008, 0x1E018},
    {0x1E01B, 0x1E021}, {0x1E023, 0x1E024}, {0x1E026, 0x1E02A},
    {0x1E08F, 0x1E08F}, {0x1E130, 0x1E136}, {0x1E2AE, 0x1E2AE},
    {0x1E2EC, 0x1E2EF}, {0x1E4EC, 0x1E4EF}, {0x1E5EE, 0x1E5EF},
    {0x1E8D0, 0x1E8D6}, {0x1E944, 0x1E94A}, {0xE0001, 0xE0001},
    {0xE0020, 0xE007F}, {0xE0100, 0xE01EF}};

static const struct interval wide[] = {
    {0x1100, 0x115F},   {0x231A, 0x231B},   {0x2329, 0x232A},
    {0x23E9, 0x23EC},   {0x23F0, 0x23F0},   {0x23F3, 0x23F3},
    {0x25FD, 0x25FE},   {0x2614, 0x2615},   {0x2630, 0x2637},
    {0x2648, 0x2653},   {0x267F, 0x267F},   {0x268A, 0x268F},
    {0x2693, 0x2693},   {0x26A1, 0x26A1},   {0x26AA, 0x26AB},
    {0x26BD, 0x26BE},   {0x26C4, 0x26C5},   {0x26CE, 0x26CE},
    {0x26D4, 0x26D4},   {0x26EA, 0x26EA},   {0x26F2, 0x26F3},
    {0x26F5, 0x26F5},   {0x26FA, 0x26FA},   {0x26FD, 0x26FD},
    {0x2705, 0x2705},   {0x270A, 0x270B},   {0x2728, 0x2728},
    {0x274C, 0x274C},   {0x274E, 0x274E},   {0x2753, 0x2755},
    {0x2757, 0x2757},   {0x2795, 0x2797},   {0x27B0, 0x27B0},
    {0x27BF, 0x27BF},   {0x2B1B, 0x2B1C},   {0x2B50, 0x2B50},
    {0x2B55, 0x2B55},   {0x2E80, 0x2E99},   {0x2E9B, 0x2EF3},
    {0x2F00, 0x2FD5},   {0x2FF0, 0x3029},   {0x302E, 0x303E},
    {0x3041, 0x3096},   {0x309B, 0x30FF},   {0x3105, 0x312F},
    {0x3131, 0x318E},   {0x3190, 0x31E5},   {0x31EF, 0x321E},
    {0x3220, 0x3247},   {0x3250, 0xA48C},   {0xA490, 0xA4C6},
    {0xA960, 0xA97C},   {0xAC00, 0xD7A3},   {0xF900, 0xFA6D},
    {0xFA70, 0xFAD9},   {0xFE10, 0xFE19},   {0xFE30, 0xFE52},
    {0xFE54, 0xFE66},   {0xFE68, 0xFE6B},   {0xFF01, 0xFF60},
    {0xFFE0, 0xFFE6},   {0x16FE0, 0x16FE3}, {0x16FF0, 0x16FF1},
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
    {0x30000, 0x3134A}, {0x31350, 0x323AF}};

static const struct interval ambiguous[] = {
    {0x00A1, 0x00A1},   {0x00A4, 0x00A4},    {0x00A7, 0x00A8},
    {0x00AA, 0x00AA},   {0x00AD, 0x00AE},    {0x00B0, 0x00B4},
    {0x00B6, 0x00BA},   {0x00BC, 0x00BF},    {0x00C6, 0x00C6},
    {0x00D0, 0x00D0},   {0x00D7, 0x00D8},    {0x00DE, 0x00E1},
    {0x00E6, 0x00E6},   {0x00E8, 0x00EA},    {0x00EC, 0x00ED},
    {0x00F0, 0x00F0},   {0x00F2, 0x00F3},    {0x00F7, 0x00FA},
    {0x00FC, 0x00FC},   {0x00FE, 0x00FE},    {0x0101, 0x0101},
    {0x0111, 0x0111},   {0x0113, 0x0113},    {0x011B, 0x011B},
    {0x0126, 0x0127},   {0x012B, 0x012B},    {0x0131, 0x0133},
    {0x0138, 0x0138},   {0x013F, 0x0142},    {0x0144, 0x0144},
    {0x0148, 0x014B},   {0x014D, 0x014D},    {0x0152, 0x0153},
    {0x0166, 0x0167},   {0x016B, 0x016B},    {0x01CE, 0x01CE},
    {0x01D0, 0x01D0},   {0x01D2, 0x01D2},    {0x01D4, 0x01D4},
    {0x01D6, 0x01D6},   {0x01D8, 0x01D8},    {0x01DA, 0x01DA},
    {0x01DC, 0x01DC},   {0x0251, 0x0251},    {0x0261, 0x0261},
    {0x02C4, 0x02C4},   {0x02C7, 0x02C7},    {0x02C9, 0x02CB},
    {0x02CD, 0x02CD},   {0x02D0, 0x02D0},    {0x02D8, 0x02DB},
    {0x02DD, 0x02DD},   {0x02DF, 0x02DF},    {0x0391, 0x03A1},
    {0x03A3, 0x03A9},   {0x03B1, 0x03C1},    {0x03C3, 0x03C9},
    {0x0401, 0x0401},   {0x0410, 0x044F},    {0x0451, 0x0451},
    {0x2010, 0x2010},   {0x2013, 0x2016},    {0x2018, 0x2019},
    {0x201C, 0x201D},   {0x2020, 0x2022},    {0x2024, 0x2027},
    {0x2030, 0x2030},   {0x2032, 0x2033},    {0x2035, 0x2035},
    {0x203B, 0x203B},   {0x203E, 0x203E},    {0x2074, 0x2074},
    {0x207F, 0x207F},   {0x2081, 0x2084},    {0x20AC, 0x20AC},
    {0x2103, 0x2103},   {0x2105, 0x2105},    {0x2109, 0x2109},
    {0x2113, 0x2113},   {0x2116, 0x2116},    {0x2121, 0x2122},
    {0x2126, 0x2126},   {0x212B, 0x212B},    {0x2153, 0x2154},
    {0x215B, 0x215E},   {0x2160, 0x216B},    {0x2170, 0x2179},
    {0x2189, 0x2189},   {0x2190, 0x2199},    {0x21B8, 0x21B9},
    {0x21D2, 0x21D2},   {0x21D4, 0x21D4},    {0x21E7, 0x21E7},
    {0x2200, 0x2200},   {0x2202, 0x2203},    {0x2207, 0x2208},
    {0x220B, 0x220B},   {0x220F, 0x220F},    {0x2211, 0x2211},
    {0x2215, 0x2215},   {0x221A, 0x221A},    {0x221D, 0x2220},
    {0x2223, 0x2223},   {0x2225, 0x2225},    {0x2227, 0x222C},
    {0x222E, 0x222E},   {0x2234, 0x2237},    {0x223C, 0x223D},
    {0x2248, 0x2248},   {0x224C, 0x224C},    {0x2252, 0x2252},
    {0x2260, 0x2261},   {0x2264, 0x2267},    {0x226A, 0x226B},
    {0x226E, 0x226F},   {0x2282, 0x2283},    {0x2286, 0x2287},
    {0x2295, 0x2295},   {0x2299, 0x2299},    {0x22A5, 0x22A5},
    {0x22BF, 0x22BF},   {0x2312, 0x2312},    {0x2460, 0x24E9},
    {0x24EB, 0x254B},   {0x2550, 0x2573},    {0x2580, 0x258F},
    {0x2592, 0x2595},   {0x25A0, 0x25A1},    {0x25A3, 0x25A9},
    {0x25B2, 0x25B3},   {0x25B6, 0x25B7},    {0x25BC, 0x25BD},
    {0x25C0, 0x25C1},   {0x25C6, 0x25C8},    {0x25CB, 0x25CB},
    {0x25CE, 0x25D1},   {0x25E2, 0x25E5},    {0x25EF, 0x25EF},
    {0x2605, 0x2606},   {0x2609, 0x2609},    {0x260E, 0x260F},
    {0x261C, 0x261C},   {0x261E, 0x261E},    {0x2640, 0x2640},
    {0x2642, 0x2642},   {0x2660, 0x2661},    {0x2663, 0x2665},
    {0x2667, 0x266A},   {0x266C, 0x266D},    {0x266F, 0x266F},
    {0x269E, 0x269F},   {0x26BF, 0x26BF},    {0x26C6, 0x26CD},
    {0x26CF, 0x26D3},   {0x26D5, 0x26E1},    {0x26E3, 0x26E3},
    {0x26E8, 0x26E9},   {0x26EB, 0x26F1},    {0x26F4, 0x26F4},
    {0x26F6, 0x26F9},   {0x26FB, 0x26FC},    {0x26FE, 0x26FF},
    {0x273D, 0x273D},   {0x2776, 0x277F},    {0x2B56, 0x2B59},
    {0x3248, 0x324F},   {0xE000, 0xF8FF},    {0xFFFD, 0xFFFD},
    {0x1F100, 0x1F10A}, {0x1F110, 0x1F12D},  {0x1F130, 0x1F169},
    {0x1F170, 0x1F18D}, {0x1F18F, 0x1F190},  {0x1F19B, 0x1F1AC},
    {0xF0000, 0xFFFFD}, {0x100000, 0x10FFFD}};

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
    enum class BackgroundColor {
        black = 40,
        red,
        green,
        yellow,
        blue,
        magenta,
        cyan,
        white,
        normal = 49,
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

    enum class ResetStyle {
        bold = 22,
        dim = 22,
        italic = 23,

        underline = 24,
        double_underline = 24,

        blink = 25,
        fast_blink = 25,
        reverse = 27,
        concealed = 28,
        crossed = 29
    };

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

    typedef struct Range {
        int from;
        int to;

        Range(int from, int to) : from(from), to(to) {}
    } Range;

    typedef struct RGB {
        uint8_t r, g, b;

        RGB(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}
    } RGB;

    class Column {
        // styles applied to the whole content
        std::string global_styles;

        std::vector<std::string> splitted_content;
        std::list<std::string> words;
        Alignment alignment;
        unsigned int width;
        unsigned int top_padding;
        unsigned int bottom_padding;
        unsigned int special_characters;
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

                column.global_styles += style_stt;

                // later add special_characters of RESET in utils in the last element of splitted_content
                return *this;
            }

            // add multiple text styles to the whole column
            Config& text_style(const std::vector<Style>& styles) {
                if (styles.empty()) return *this;

                // Apply all styles at the beginning
                std::string styles_stt;
                for (const auto& style : styles) {
                    styles_stt += CSI + std::to_string(static_cast<int>(style)) + "m";
                }

                column.global_styles += styles_stt;

                return *this;
            }

            // add Background Color to the whole column
            Config& background_color(BackgroundColor color) {

                // Background Color statement
                std::string back_color_stt = CSI + std::to_string(static_cast<int>(color)) + "m";

                column.global_styles += back_color_stt;

                return *this;
            }

            // add Color to the whole column
            Config& color(Color color) {

                // Color statement
                std::string color_stt = CSI + std::to_string(static_cast<int>(color)) + "m";

                column.global_styles += color_stt;

                return *this;
            }

            // add RGB to the whole column
            Config& rgb(RGB rgb) {

                // RGB statement
                std::string rgb_stt = CSI "38;2;" + std::to_string(rgb.r) + ";" +
                                      std::to_string(rgb.g) + ";" +
                                      std::to_string(rgb.b) + "m";

                column.global_styles += rgb_stt;
                return *this;
            }

            // add background RGB to the whole column
            Config& background_rgb(RGB rgb) {

                // RGB statement
                std::string back_rgb_stt = CSI "48;2;" + std::to_string(rgb.r) + ";" +
                                           std::to_string(rgb.g) + ";" +
                                           std::to_string(rgb.b) + "m";

                column.global_styles += back_rgb_stt;

                return *this;
            }

            Config& multi_byte_chars(bool is_multi_byte) {
                column.is_multi_byte = is_multi_byte;

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

            unsigned int special_characters() { return column.special_characters; }

            std::vector<std::string> splitted_content() { return column.splitted_content; }

            std::list<std::string> words() { return column.words; }

            std::string global_styles() { return column.global_styles; }

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
        };

        inline bool isstrascii(std::string str) {
            size_t len = str.length();
            for (size_t i = 0; i < len; i++)
                if (static_cast<unsigned char>(str[i]) > 127) return false;

            return true;
        }

    public:
        std::string content;

        // automatic check for multi byte characters
        Column(std::string content) {
            this->content = content;
            alignment = Alignment::left;
            width = 0;
            top_padding = 0;
            bottom_padding = 0;
            special_characters = 0;

            if (!isstrascii(content))
                is_multi_byte = true;
            else
                is_multi_byte = false;
        }

        Config config() { return Config(*this); }

        Getters get() { return Getters(*this); }

        Setters set() { return Setters(*this); }
    };

    class Row {
        Alignment alignment;

        class Config {
            Row& row;

        public:
            Config(Row& row) : row(row) {}

            Config& alignment(Alignment alignment) {
                row.alignment = alignment;

                for (Column& col : row.columns)
                    col.config().alignment(alignment);

                return *this;
            }

            Config& padding(int padding) {
                for (Column& col : row.columns)
                    col.config().padding(padding);

                return *this;
            }

            Config& top_padding(int padding) {
                for (Column& col : row.columns)
                    col.config().top_padding(padding);

                return *this;
            }

            Config& bottom_padding(int padding) {
                for (Column& col : row.columns)
                    col.config().bottom_padding(padding);

                return *this;
            }

            Config& color(Color color) {
                for (Column& col : row.columns)
                    col.config().color(color);

                return *this;
            }

            Config& background_color(BackgroundColor back_color) {
                for (Column& col : row.columns)
                    col.config().background_color(back_color);

                return *this;
            }

            Config& rgb(RGB rgb) {
                for (Column& col : row.columns)
                    col.config().rgb(rgb);

                return *this;
            }

            Config& background_rgb(RGB background_rgb) {
                for (Column& col : row.columns)
                    col.config().background_rgb(background_rgb);

                return *this;
            }

            Config& multi_byte_chars(bool is_multi_byte) {
                for (Column& col : row.columns)
                    col.config().multi_byte_chars(is_multi_byte);

                return *this;
            }
        };

    public:
        std::vector<Column> columns;

        Row(std::vector<Column> columns)
            : columns(columns), alignment(Alignment::left) {}

        Config config() { return Config(*this); }

        int get_columns_number() { return columns.size(); }

        unsigned int get_full_row_width() {
            unsigned int width = columns.size() + 1;

            for (Column column : columns)
                width += column.get().width();

            return width;
        }

        Column& operator[](int index) {
            if (index >= this->columns.size() || index < 0)
                throw std::out_of_range("Index out of bounds");

            return this->columns[index];
        }
    };

    namespace maps {
        static Border get_border_template(BorderStyle borderStyle) {
            static std::map<BorderStyle, Border> templates{
                {BorderStyle::empty, {" ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "}},
                {BorderStyle::standard, {"|", "-", "+", "+", "+", "+", "+", "+", "+", "+", "+", "+"}},
                {BorderStyle::ANSI, {
                                        TABLE_MODE "x" RESET_TABLE_MODE, // vertical
                                        TABLE_MODE "q" RESET_TABLE_MODE, // horizontal
                                        TABLE_MODE "n" RESET_TABLE_MODE, // corner
                                        TABLE_MODE "j" RESET_TABLE_MODE, // bottom_right_corner
                                        TABLE_MODE "k" RESET_TABLE_MODE, // top_right_corner
                                        TABLE_MODE "l" RESET_TABLE_MODE, // top_left_corner
                                        TABLE_MODE "m" RESET_TABLE_MODE, // bottom_left_corner
                                        TABLE_MODE "n" RESET_TABLE_MODE, // middle_separator
                                        TABLE_MODE "t" RESET_TABLE_MODE, // middle_left_to_right
                                        TABLE_MODE "u" RESET_TABLE_MODE, // middle_right_to_left
                                        TABLE_MODE "v" RESET_TABLE_MODE, // middle_bottom_to_top
                                        TABLE_MODE "w" RESET_TABLE_MODE  // middle_top_to_bottom
                                    }}};
            return templates[borderStyle];
        }

        static ResetStyle get_style_reset(Style style) {
            static std::map<Style, ResetStyle> reset{
                {Style::bold, ResetStyle::bold},
                {Style::dim, ResetStyle::dim},
                {Style::italic, ResetStyle::italic},
                {Style::underline, ResetStyle::underline},
                {Style::double_underline, ResetStyle::double_underline},
                {Style::blink, ResetStyle::blink},
                {Style::fast_blink, ResetStyle::fast_blink},
                {Style::reverse, ResetStyle::reverse},
                {Style::concealed, ResetStyle::concealed},
                {Style::crossed, ResetStyle::crossed}};

            return reset[style];
        }
    }; // namespace maps

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

        inline void add_spaces(std::string& in, int spaces_num) {
            for (int i = 0; i < spaces_num; i++)
                in.append(" ");
        }

        // split by words AND save '\n' as a word too
        inline std::list<std::string> split_text(std::string str) {
            std::list<std::string> result;

            size_t str_size = str.size();
            size_t prev_start = 0;
            for (size_t i = 0; i < str_size; i++) {
                if (str.at(i) == '\n') {
                    if (i > prev_start)
                        result.push_back(str.substr(prev_start, i - prev_start));

                    result.push_back("\n");
                    prev_start = i + 1;
                } else if (str.at(i) == ' ') {
                    if (i > prev_start)
                        result.push_back(str.substr(prev_start, i - prev_start));

                    result.push_back(" ");
                    prev_start = i + 1;
                }
            }

            // Handles the last word if it exists
            if (prev_start < str_size)
                result.push_back(str.substr(prev_start));

            return result;
        }

        // note: add_spaces(line, 1); for space side
        inline void append_and_clear(std::vector<std::string>& result, std::string& sub, int usable_width, Column column) {
            std::string line;
            Alignment align = column.get().alignment();

            // auto horizontal padding of 1
            if (line.empty())
                add_spaces(line, 1);

            int start;
            if (align == Alignment::center)
                start = (usable_width - sub.size()) / 2;
            else if (align == Alignment::right)
                start = (usable_width - sub.size());
            else
                start = 0;

            add_spaces(line, start);

            std::string global_styles = column.get().global_styles();
            if (!global_styles.empty())
                line.append(global_styles);

            line.append(sub);

            if (!global_styles.empty())
                line.append(RESET);

            // auto horizontal padding
            add_spaces(line, 1);

            result.push_back(line);

            sub.clear();
        }

        inline std::vector<std::string> prepare_col_content(Column& column, int max_width) {
            std::string content = column.content;
            unsigned int top_padding = column.get().top_padding();
            unsigned int bottom_padding = column.get().bottom_padding();

            // which width to use
            if (column.get().width() != 0) max_width = column.get().width();

            if (content.empty() || max_width == 0)
                return std::vector<std::string>();

            // split the content into words to easily manipulate it
            auto words = split_text(content);

            // the return result
            std::vector<std::string> result;

            // TOP padding
            for (unsigned int i = 0; i < top_padding; i++)
                result.push_back(std::string());

            // e.g: MAX sub size POSSIBLE, - 2 for two sides spaces
            const int usable_width = (max_width - 2);

            // don't go back more than 30% when the last word is too long
            const int limit = (usable_width * CONTENT_MANIPULATION_BACK_LIMIT);

            std::string sub;
            for (auto it = words.begin(); it != words.end(); ++it) {
                std::string& word = *it;

                // add existing content if we reach new line
                if (word == "\n") {
                    append_and_clear(result, sub, usable_width, column);
                    continue;
                }

                // we need split
                if ((sub.size() + word.size()) > usable_width) {
                    int diff = usable_width - sub.size();
                    if (diff > limit) {
                        std::string part = word.substr(0, diff - 1);
                        part += '-';

                        sub += part;
                        append_and_clear(result, sub, usable_width, column);

                        std::string remaining = word.substr(diff - 1);
                        words.insert(std::next(it), remaining);
                    } else {
                        sub.pop_back(); // pop the space added previously which is a word itself
                        append_and_clear(result, sub, usable_width, column);
                        --it;
                    }
                } else
                    sub += word; // add a normall less than line word
            }

            // any remaining words
            if (!sub.empty())
                append_and_clear(result, sub, usable_width, column);

            // BOTTOM padding
            for (unsigned int i = 0; i < bottom_padding; i++)
                result.push_back(std::string());

            return result;
        }

        inline void format_row(unsigned int width, Row& row) {
            if (row.columns.size() == 0)
                return;

            int cols_num = row.columns.size();
            if (width <= 0 || cols_num <= 0)
                return;

            // for other columns width calculation we should decrease the specific ones
            for (Column& column : row.columns) {
                if (column.get().width() != 0) {
                    width -= column.get().width();
                    cols_num--;
                }
            }

            int individual_col_width = 0;
            int rest = 0;
            if (cols_num > 0) {
                individual_col_width = width / cols_num;
                rest = width % cols_num;
            }

            for (Column& col : row.columns) {
                if (col.get().width() != 0)
                    col.set().splitted_content(prepare_col_content(col, col.get().width()));

                else if (rest > 0) {
                    col.set().splitted_content(prepare_col_content(col, individual_col_width + 1));

                    col.set().width(individual_col_width + 1);
                    rest--;
                } else {
                    col.set().splitted_content(prepare_col_content(col, individual_col_width));

                    col.set().width(individual_col_width);
                }
            }
        }

        //   return the size of the tallest splittedContent vector
        inline size_t find_max_splitted_content_size(Row row) {
            size_t result = 0;
            for (Column col : row.columns) {
                size_t splitted_content_size = col.get().splitted_content().size();
                if (splitted_content_size > result)
                    result = splitted_content_size;
            }

            return result;
        }

        inline size_t mbswidth(std::string str) {
            wchar_t* wstr = utf8stws(str.c_str());
            return ah_wcswidth(wstr, wcslen(wstr));
        }
    } // namespace utils

    class Table {
        BorderStyle border_style;
        Border border_templates;
        unsigned int width; // for width we check if it is bigger than the terminal width so no problem
        bool forced_width;
        bool force_ansi;

        class Config {
            Table& table;

        public:
            Config(Table& table) : table(table) {}

            /* -----------------ALIGNMENT--------------------- */
            Config& alignment(Alignment align, int column) {
                if (column < 0)
                    return *this;

                for (Row& row : table.rows)
                    if (column < row.columns.size())
                        row[column].config().alignment(align);

                return *this;
            }

            Config& alignment(Alignment align) {
                for (Row& row : table.rows)
                    row.config().alignment(align);

                return *this;
            }

            /* -----------------PADDING--------------------- */
            Config& padding(int pad, int column) {
                if (pad < 0 || column < 0)
                    return *this;

                for (Row& row : table.rows)
                    if (column < row.columns.size())
                        row[column].config().padding(pad);

                return *this;
            }

            Config& padding(int pad) {
                if (pad < 0)
                    return *this; // Invalid padding

                for (Row& row : table.rows)
                    row.config().padding(pad);

                return *this;
            }

            /* -----------------TOP_PADDING--------------------- */
            Config& top_padding(int pad, int column) {
                if (pad < 0 || column < 0)
                    return *this;

                for (Row& row : table.rows)
                    if (column < row.columns.size())
                        row[column].config().top_padding(pad);

                return *this;
            }

            Config& top_padding(int pad) {
                if (pad < 0)
                    return *this; // Invalid padding

                for (Row& row : table.rows)
                    row.config().top_padding(pad);

                return *this;
            }

            /* -----------------BOTTOM_PADDING--------------------- */
            Config& bottom_padding(int pad, int column) {
                if (pad < 0 || column < 0)
                    return *this;

                for (Row& row : table.rows)
                    if (column < row.columns.size())
                        row[column].config().bottom_padding(pad);

                return *this;
            }

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

            Config& columns_width(int width, int column) {
                if (width < 0 || column < 0)
                    return *this;

                for (Row& row : table.rows)
                    if (column < row.columns.size())
                        row[column].config().width(width);

                return *this;
            }

            /* -------------------Colors------------------------- */
            Config& color(Color col, int column) {
                if (column < 0)
                    return *this;

                for (Row& row : table.rows)
                    if (column < row.columns.size())
                        row[column].config().color(col);

                return *this;
            }

            Config& color(Color color) {
                for (Row& row : table.rows)
                    row.config().color(color);

                return *this;
            }

            /* ---------------Background Colors--------------------- */
            Config& background_color(BackgroundColor back_color, int column) {
                if (column < 0)
                    return *this;

                for (Row& row : table.rows)
                    if (column < row.columns.size())
                        row[column].config().background_color(back_color);

                return *this;
            }

            Config& background_color(BackgroundColor back_color) {
                for (Row& row : table.rows)
                    row.config().background_color(back_color);

                return *this;
            }

            /* -------------------RGB------------------------- */
            Config& rgb(RGB rgb, int column) {
                if (column < 0)
                    return *this;

                for (Row& row : table.rows)
                    if (column < row.columns.size())
                        row[column].config().rgb(rgb);

                return *this;
            }

            Config& rgb(RGB rgb) {
                for (Row& row : table.rows)
                    row.config().rgb(rgb);

                return *this;
            }

            /* ---------------Background RGB--------------------- */
            Config& background_rgb(RGB back_rgb, int column) {
                if (column < 0)
                    return *this;

                for (Row& row : table.rows)
                    if (column < row.columns.size())
                        row[column].config().background_rgb(back_rgb);

                return *this;
            }

            Config& background_rgb(RGB back_rgb) {
                for (Row& row : table.rows)
                    row.config().background_rgb(back_rgb);

                return *this;
            }

            /* -----------------BORDER PARTS--------------------- */

            Config& corner(char corner) {
                table.border_templates.corner = corner;
                return *this;
            }

            Config& horizontal(char horizontal) {
                table.border_templates.horizontal = horizontal;
                return *this;
            }

            Config& vertical(char vertical) {
                table.border_templates.vertical = vertical;
                return *this;
            }

            Config& bottom_right_corner(char corner) {
                table.border_templates.bottom_right_corner = corner;
                return *this;
            }

            Config& top_right_corner(char corner) {
                table.border_templates.top_right_corner = corner;
                return *this;
            }

            Config& top_left_corner(char corner) {
                table.border_templates.top_left_corner = corner;
                return *this;
            }

            Config& bottom_left_corner(char corner) {
                table.border_templates.bottom_left_corner = corner;
                return *this;
            }

            Config& middle_separator(char separator) {
                table.border_templates.middle_separator = separator;
                return *this;
            }

            Config& middle_left_to_right(char connector) {
                table.border_templates.left_to_right = connector;
                return *this;
            }

            Config& middle_right_to_left(char connector) {
                table.border_templates.right_to_left = connector;
                return *this;
            }

            Config& middle_bottom_to_top(char connector) {
                table.border_templates.bottom_to_top = connector;
                return *this;
            }

            Config& middle_top_to_bottom(char connector) {
                table.border_templates.top_to_bottom = connector;
                return *this;
            }

            Config& border(BorderStyle style) {
                table.border_style = style;
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
                table.border_style = BorderStyle::ANSI;

                table.force_ansi = forced;

                return *this;
            }
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

        void print_row(std::ostream& stream, Row& row, int width_reference) {
            if (width_reference != 0) {
                // tableSplits = (row.columns.size() + 1)
                unsigned int usable_width = width_reference - (row.columns.size() + 1);
                utils::format_row(usable_width, row);
            }

            std::string vertical = border_templates.vertical;

            size_t max_splitted_content_size = utils::find_max_splitted_content_size(row); // tallest vector of splitted strings
            for (unsigned int i = 0; i < max_splitted_content_size; i++) {
                // side border
                stream << '\n'
                       << vertical;

                // printing the n element of splitted_content for each column
                for (Column column : row.columns) {
                    int rest = column.get().width();
                    int splitted_content_size = column.get().splitted_content().size();
                    std::string current_line;

                    if (i < splitted_content_size) {
                        current_line = column.get().splitted_content().at(i);
                        stream << current_line;

                        int special_characters = column.get().special_characters();
                        std::string global_styles = column.get().global_styles();

                        // for each splitted_content element has a one or more global_styles, it has one RESET at the end
                        if (!global_styles.empty()) {
                            std::string reset = RESET;
                            special_characters += reset.size();
                            special_characters += global_styles.size();
                        }

                        size_t curr_line_size;

                        if (column.get().is_multi_byte())
                            curr_line_size = utils::mbswidth(current_line);
                        else
                            curr_line_size = current_line.size();

                        rest -= curr_line_size - special_characters; // to balance the line
                    }

                    for (int k = 0; k < rest; k++)
                        stream << ' ';

                    stream << vertical;
                }
            }
        }

        void print_border(std::ostream& stream, std::vector<Row>::iterator& it, bool is_first, bool is_last, bool regular) {

            Row reference = *it;
            std::vector<int> next_row_corners;

            if (is_first)
                next_row_corners = find_stops(*it);
            else if (!is_last)
                next_row_corners = find_stops(*(it + 1));

            if (!is_first)
                stream << '\n';

            std::string horizontal = border_templates.horizontal;

            // (vertical separators)/corners
            std::string left_corner;
            std::string right_corner;

            // horizontal separator
            std::string middle_separator;
            std::string top_to_bottom;
            std::string bottom_to_top;

            if (is_first) {
                left_corner = border_templates.top_left_corner;
                right_corner = border_templates.top_right_corner;

                middle_separator = border_templates.top_to_bottom;
                top_to_bottom = border_templates.top_to_bottom;
                bottom_to_top = border_templates.top_to_bottom;
            } else if (is_last) {
                left_corner = border_templates.bottom_left_corner;
                right_corner = border_templates.bottom_right_corner;

                middle_separator = border_templates.bottom_to_top;
                top_to_bottom = border_templates.bottom_to_top;
                bottom_to_top = border_templates.bottom_to_top;
            } else {
                left_corner = border_templates.left_to_right;
                right_corner = border_templates.right_to_left;

                middle_separator = border_templates.middle_separator;
                top_to_bottom = border_templates.top_to_bottom;
                bottom_to_top = border_templates.bottom_to_top;
            }

            stream << left_corner;

            size_t cols_num = reference.columns.size();

            int tracker = 0;
            for (size_t j = 0; j < cols_num; j++) {
                Column column = reference.columns[j];
                unsigned col_width = column.get().width();

                // tracker++;

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

        unsigned int find_max_rows_width() {
            unsigned int result = 0;
            for (Row row : rows) {
                unsigned int row_width = row.get_full_row_width();
                if (row_width > result) result = row_width;
            }

            return result;
        }

    public:
        std::vector<Row> rows;

        Table() : border_style(BorderStyle::standard), width(0), forced_width(false) {}

        // configure the table
        Config config() { return Config(*this); }

        Setters set() { return Setters(*this); }

        unsigned int get_width() { return width; }

        void add_row(std::vector<std::string> contents) {
            std::vector<Column> columns;
            for (std::string content : contents)
                columns.push_back(Column(content));

            rows.push_back(Row(columns));
        }

        /* Regularity means it has the same number of columns in each row */
        bool is_regular(Range range) {
            size_t reference = rows[range.from].columns.size();

            for (int i = range.from + 1; i <= range.to; i++)
                if (rows[i].columns.size() != reference)
                    return false;

            return true;
        }

        bool is_regular() {
            return is_regular(Range(0, rows.size() - 1));
        }

        // * return 2 for empty rows and 3 for terminal space problems
        int print(std::ostream& stream) {
            if (rows.size() == 0)
                return 2;

            // chose width to use
            unsigned int usable_width;
            if (forced_width)
                usable_width = width;
            else {
                unsigned short terminal_width = utils::get_terminal_width();
                usable_width = terminal_width * DEFAULT_WIDTH_PERCENT;
                if (this->width <= 0 || this->width > terminal_width)
                    width = usable_width;
                else
                    usable_width = width;
            }

            size_t cols_num;
            size_t row_usable_width;
            // edit rows to match the width
            for (Row& row : rows) {
                cols_num = row.columns.size();
                row_usable_width = usable_width - (cols_num + 1); // ... - tableSplits

                if (row_usable_width <= cols_num)
                    return 3;

                utils::format_row(row_usable_width, row);
            }

            // check if the table has consistent number of columns across all rows
            bool regular = is_regular();

            if (border_style == BorderStyle::ANSI)
                if (!utils::check_terminal() && !force_ansi)
                    border_style = BorderStyle::standard;

            Border templates = border_templates;
            border_templates = maps::get_border_template(border_style);

            if (!templates.corner.empty()) border_templates.corner = templates.corner;
            if (!templates.horizontal.empty()) border_templates.horizontal = templates.horizontal;
            if (!templates.vertical.empty()) border_templates.vertical = templates.vertical;
            if (!templates.top_left_corner.empty()) border_templates.top_left_corner = templates.top_left_corner;
            if (!templates.top_right_corner.empty()) border_templates.top_right_corner = templates.top_right_corner;
            if (!templates.bottom_left_corner.empty()) border_templates.bottom_left_corner = templates.bottom_left_corner;
            if (!templates.bottom_right_corner.empty()) border_templates.bottom_right_corner = templates.bottom_right_corner;
            if (!templates.middle_separator.empty()) border_templates.middle_separator = templates.middle_separator;
            if (!templates.left_to_right.empty()) border_templates.left_to_right = templates.left_to_right;
            if (!templates.right_to_left.empty()) border_templates.right_to_left = templates.right_to_left;
            if (!templates.top_to_bottom.empty()) border_templates.top_to_bottom = templates.top_to_bottom;
            if (!templates.bottom_to_top.empty()) border_templates.bottom_to_top = templates.bottom_to_top;

            /* ------ printing the table ------- */

            // 0 to check in print_row
            unsigned int row_width_reference = regular ? 0 : find_max_rows_width();

            if (!regular)
                for (auto it = rows.begin(); it != rows.end(); ++it)
                    utils::format_row(row_width_reference - (it->columns.size() + 1), *it);

            bool is_first = true, is_last = false;
            if (rows.size() == 1)
                is_last = true;

            auto it = rows.begin();
            print_border(stream, it, is_first, is_last, regular);

            is_first = false;
            for (it = rows.begin(); it != rows.end(); ++it) {
                Row& row = *it;

                print_row(stream, row, row_width_reference);

                if ((it + 1) == rows.end())
                    is_last = true;

                print_border(stream, it, is_first, is_last, regular);
            }

            return 0;
        }

        Row& operator[](int index) {
            if (index >= this->rows.size() || index < 0)
                throw std::out_of_range("Index out of bounds");

            return this->rows[index];
        }
    };

    inline std::ostream& operator<<(std::ostream& stream, const Table& table) {
        const_cast<Table&>(table).print(stream);
        return stream;
    }
} // namespace tabular
