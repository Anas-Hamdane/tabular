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

#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace tabular {
namespace detail {
struct Range {
  uint32_t first;
  uint32_t last;
};

constexpr static Range combining[] = {
    {0x0000, 0x001F}, {0x007F, 0x009F}, {0x0300, 0x036F}, {0x0483, 0x0489},
    {0x0591, 0x05BD}, {0x05BF, 0x05BF}, {0x05C1, 0x05C2}, {0x05C4, 0x05C5},
    {0x05C7, 0x05C7}, {0x0600, 0x0605}, {0x0610, 0x061A}, {0x061C, 0x061C},
    {0x064B, 0x065F}, {0x0670, 0x0670}, {0x06D6, 0x06DD}, {0x06DF, 0x06E4},
    {0x06E7, 0x06E8}, {0x06EA, 0x06ED}, {0x070F, 0x070F}, {0x0711, 0x0711},
    {0x0730, 0x074A}, {0x07A6, 0x07B0}, {0x07EB, 0x07F3}, {0x07FD, 0x07FD},
    {0x0816, 0x0819}, {0x081B, 0x0823}, {0x0825, 0x0827}, {0x0829, 0x082D},
    {0x0859, 0x085B}, {0x0890, 0x0891}, {0x0897, 0x089F}, {0x08CA, 0x0903},
    {0x093A, 0x093C}, {0x093E, 0x094F}, {0x0951, 0x0957}, {0x0962, 0x0963},
    {0x0981, 0x0983}, {0x09BC, 0x09BC}, {0x09BE, 0x09C4}, {0x09C7, 0x09C8},
    {0x09CB, 0x09CD}, {0x09D7, 0x09D7}, {0x09E2, 0x09E3}, {0x09FE, 0x09FE},
    {0x0A01, 0x0A03}, {0x0A3C, 0x0A3C}, {0x0A3E, 0x0A42}, {0x0A47, 0x0A48},
    {0x0A4B, 0x0A4D}, {0x0A51, 0x0A51}, {0x0A70, 0x0A71}, {0x0A75, 0x0A75},
    {0x0A81, 0x0A83}, {0x0ABC, 0x0ABC}, {0x0ABE, 0x0AC5}, {0x0AC7, 0x0AC9},
    {0x0ACB, 0x0ACD}, {0x0AE2, 0x0AE3}, {0x0AFA, 0x0AFF}, {0x0B01, 0x0B03},
    {0x0B3C, 0x0B3C}, {0x0B3E, 0x0B44}, {0x0B47, 0x0B48}, {0x0B4B, 0x0B4D},
    {0x0B55, 0x0B57}, {0x0B62, 0x0B63}, {0x0B82, 0x0B82}, {0x0BBE, 0x0BC2},
    {0x0BC6, 0x0BC8}, {0x0BCA, 0x0BCD}, {0x0BD7, 0x0BD7}, {0x0C00, 0x0C04},
    {0x0C3C, 0x0C3C}, {0x0C3E, 0x0C44}, {0x0C46, 0x0C48}, {0x0C4A, 0x0C4D},
    {0x0C55, 0x0C56}, {0x0C62, 0x0C63}, {0x0C81, 0x0C83}, {0x0CBC, 0x0CBC},
    {0x0CBE, 0x0CC4}, {0x0CC6, 0x0CC8}, {0x0CCA, 0x0CCD}, {0x0CD5, 0x0CD6},
    {0x0CE2, 0x0CE3}, {0x0CF3, 0x0CF3}, {0x0D00, 0x0D03}, {0x0D3B, 0x0D3C},
    {0x0D3E, 0x0D44}, {0x0D46, 0x0D48}, {0x0D4A, 0x0D4D}, {0x0D57, 0x0D57},
    {0x0D62, 0x0D63}, {0x0D81, 0x0D83}, {0x0DCA, 0x0DCA}, {0x0DCF, 0x0DD4},
    {0x0DD6, 0x0DD6}, {0x0DD8, 0x0DDF}, {0x0DF2, 0x0DF3}, {0x0E31, 0x0E31},
    {0x0E34, 0x0E3A}, {0x0E47, 0x0E4E}, {0x0EB1, 0x0EB1}, {0x0EB4, 0x0EBC},
    {0x0EC8, 0x0ECE}, {0x0F18, 0x0F19}, {0x0F35, 0x0F35}, {0x0F37, 0x0F37},
    {0x0F39, 0x0F39}, {0x0F3E, 0x0F3F}, {0x0F71, 0x0F84}, {0x0F86, 0x0F87},
    {0x0F8D, 0x0F97}, {0x0F99, 0x0FBC}, {0x0FC6, 0x0FC6}, {0x102B, 0x103E},
    {0x1056, 0x1059}, {0x105E, 0x1060}, {0x1062, 0x1064}, {0x1067, 0x106D},
    {0x1071, 0x1074}, {0x1082, 0x108D}, {0x108F, 0x108F}, {0x109A, 0x109D},
    {0x135D, 0x135F}, {0x1712, 0x1715}, {0x1732, 0x1734}, {0x1752, 0x1753},
    {0x1772, 0x1773}, {0x17B4, 0x17D3}, {0x17DD, 0x17DD}, {0x180B, 0x180F},
    {0x1885, 0x1886}, {0x18A9, 0x18A9}, {0x1920, 0x192B}, {0x1930, 0x193B},
    {0x1A17, 0x1A1B}, {0x1A55, 0x1A5E}, {0x1A60, 0x1A7C}, {0x1A7F, 0x1A7F},
    {0x1AB0, 0x1ACE}, {0x1B00, 0x1B04}, {0x1B34, 0x1B44}, {0x1B6B, 0x1B73},
    {0x1B80, 0x1B82}, {0x1BA1, 0x1BAD}, {0x1BE6, 0x1BF3}, {0x1C24, 0x1C37},
    {0x1CD0, 0x1CD2}, {0x1CD4, 0x1CE8}, {0x1CED, 0x1CED}, {0x1CF4, 0x1CF4},
    {0x1CF7, 0x1CF9}, {0x1DC0, 0x1DFF}, {0x200B, 0x200F}, {0x2028, 0x202E},
    {0x2060, 0x2064}, {0x2066, 0x206F}, {0x20D0, 0x20F0}, {0x2CEF, 0x2CF1},
    {0x2D7F, 0x2D7F}, {0x2DE0, 0x2DFF}, {0x302A, 0x302D}, {0x3099, 0x309A},
    {0xA66F, 0xA672}, {0xA674, 0xA67D}, {0xA69E, 0xA69F}, {0xA6F0, 0xA6F1},
    {0xA802, 0xA802}, {0xA806, 0xA806}, {0xA80B, 0xA80B}, {0xA823, 0xA827},
    {0xA82C, 0xA82C}, {0xA880, 0xA881}, {0xA8B4, 0xA8C5}, {0xA8E0, 0xA8F1},
    {0xA8FF, 0xA8FF}, {0xA926, 0xA92D}, {0xA947, 0xA953}, {0xA980, 0xA983},
    {0xA9B3, 0xA9C0}, {0xA9E5, 0xA9E5}, {0xAA29, 0xAA36}, {0xAA43, 0xAA43},
    {0xAA4C, 0xAA4D}, {0xAA7B, 0xAA7D}, {0xAAB0, 0xAAB0}, {0xAAB2, 0xAAB4},
    {0xAAB7, 0xAAB8}, {0xAABE, 0xAABF}, {0xAAC1, 0xAAC1}, {0xAAEB, 0xAAEF},
    {0xAAF5, 0xAAF6}, {0xABE3, 0xABEA}, {0xABEC, 0xABED}, {0xD800, 0xDFFF},
    {0xFB1E, 0xFB1E}, {0xFE00, 0xFE0F}, {0xFE20, 0xFE2F}, {0xFEFF, 0xFEFF},
    {0xFFF9, 0xFFFB}, {0x101FD, 0x101FD}, {0x102E0, 0x102E0}, {0x10376, 0x1037A},
    {0x10A01, 0x10A03}, {0x10A05, 0x10A06}, {0x10A0C, 0x10A0F}, {0x10A38, 0x10A3A},
    {0x10A3F, 0x10A3F}, {0x10AE5, 0x10AE6}, {0x10D24, 0x10D27}, {0x10D69, 0x10D6D},
    {0x10EAB, 0x10EAC}, {0x10EFC, 0x10EFF}, {0x10F46, 0x10F50}, {0x10F82, 0x10F85},
    {0x11000, 0x11002}, {0x11038, 0x11046}, {0x11070, 0x11070}, {0x11073, 0x11074},
    {0x1107F, 0x11082}, {0x110B0, 0x110BA}, {0x110BD, 0x110BD}, {0x110C2, 0x110C2},
    {0x110CD, 0x110CD}, {0x11100, 0x11102}, {0x11127, 0x11134}, {0x11145, 0x11146},
    {0x11173, 0x11173}, {0x11180, 0x11182}, {0x111B3, 0x111C0}, {0x111C9, 0x111CC},
    {0x111CE, 0x111CF}, {0x1122C, 0x11237}, {0x1123E, 0x1123E}, {0x11241, 0x11241},
    {0x112DF, 0x112EA}, {0x11300, 0x11303}, {0x1133B, 0x1133C}, {0x1133E, 0x11344},
    {0x11347, 0x11348}, {0x1134B, 0x1134D}, {0x11357, 0x11357}, {0x11362, 0x11363},
    {0x11366, 0x1136C}, {0x11370, 0x11374}, {0x113B8, 0x113C0}, {0x113C2, 0x113C2},
    {0x113C5, 0x113C5}, {0x113C7, 0x113CA}, {0x113CC, 0x113D0}, {0x113D2, 0x113D2},
    {0x113E1, 0x113E2}, {0x11435, 0x11446}, {0x1145E, 0x1145E}, {0x114B0, 0x114C3},
    {0x115AF, 0x115B5}, {0x115B8, 0x115C0}, {0x115DC, 0x115DD}, {0x11630, 0x11640},
    {0x116AB, 0x116B7}, {0x1171D, 0x1172B}, {0x1182C, 0x1183A}, {0x11930, 0x11935},
    {0x11937, 0x11938}, {0x1193B, 0x1193E}, {0x11940, 0x11940}, {0x11942, 0x11943},
    {0x119D1, 0x119D7}, {0x119DA, 0x119E0}, {0x119E4, 0x119E4}, {0x11A01, 0x11A0A},
    {0x11A33, 0x11A39}, {0x11A3B, 0x11A3E}, {0x11A47, 0x11A47}, {0x11A51, 0x11A5B},
    {0x11A8A, 0x11A99}, {0x11C2F, 0x11C36}, {0x11C38, 0x11C3F}, {0x11C92, 0x11CA7},
    {0x11CA9, 0x11CB6}, {0x11D31, 0x11D36}, {0x11D3A, 0x11D3A}, {0x11D3C, 0x11D3D},
    {0x11D3F, 0x11D45}, {0x11D47, 0x11D47}, {0x11D8A, 0x11D8E}, {0x11D90, 0x11D91},
    {0x11D93, 0x11D97}, {0x11EF3, 0x11EF6}, {0x11F00, 0x11F01}, {0x11F03, 0x11F03},
    {0x11F34, 0x11F3A}, {0x11F3E, 0x11F42}, {0x11F5A, 0x11F5A}, {0x13430, 0x13440},
    {0x13447, 0x13455}, {0x1611E, 0x1612F}, {0x16AF0, 0x16AF4}, {0x16B30, 0x16B36},
    {0x16F4F, 0x16F4F}, {0x16F51, 0x16F87}, {0x16F8F, 0x16F92}, {0x16FE4, 0x16FE4},
    {0x1BC9D, 0x1BC9E}, {0x1BCA0, 0x1BCA3}, {0x1CF00, 0x1CF2D}, {0x1CF30, 0x1CF46},
    {0x1D165, 0x1D169}, {0x1D16D, 0x1D182}, {0x1D185, 0x1D18B}, {0x1D1AA, 0x1D1AD},
    {0x1D242, 0x1D244}, {0x1DA00, 0x1DA36}, {0x1DA3B, 0x1DA6C}, {0x1DA75, 0x1DA75},
    {0x1DA84, 0x1DA84}, {0x1DA9B, 0x1DA9F}, {0x1DAA1, 0x1DAAF}, {0x1E000, 0x1E006},
    {0x1E008, 0x1E018}, {0x1E01B, 0x1E021}, {0x1E023, 0x1E024}, {0x1E026, 0x1E02A},
    {0x1E08F, 0x1E08F}, {0x1E130, 0x1E136}, {0x1E2AE, 0x1E2AE}, {0x1E2EC, 0x1E2EF},
    {0x1E4EC, 0x1E4EF}, {0x1E5EE, 0x1E5EF}, {0x1E8D0, 0x1E8D6}, {0x1E944, 0x1E94A},
    {0xE0001, 0xE0001}, {0xE0020, 0xE007F}, {0xE0100, 0xE01EF}};

// Wide characters (East Asian wide/fullwidth)
constexpr static Range wide[] = {
    {0x1100, 0x115F}, {0x231A, 0x231B}, {0x2329, 0x232A}, {0x23E9, 0x23EC},
    {0x23F0, 0x23F0}, {0x23F3, 0x23F3}, {0x25FD, 0x25FE}, {0x2614, 0x2615},
    {0x2630, 0x2637}, {0x2648, 0x2653}, {0x267F, 0x267F}, {0x268A, 0x268F},
    {0x2693, 0x2693}, {0x26A1, 0x26A1}, {0x26AA, 0x26AB}, {0x26BD, 0x26BE},
    {0x26C4, 0x26C5}, {0x26CE, 0x26CE}, {0x26D4, 0x26D4}, {0x26EA, 0x26EA},
    {0x26F2, 0x26F3}, {0x26F5, 0x26F5}, {0x26FA, 0x26FA}, {0x26FD, 0x26FD},
    {0x2705, 0x2705}, {0x270A, 0x270B}, {0x2728, 0x2728}, {0x274C, 0x274C},
    {0x274E, 0x274E}, {0x2753, 0x2755}, {0x2757, 0x2757}, {0x2795, 0x2797},
    {0x27B0, 0x27B0}, {0x27BF, 0x27BF}, {0x2B1B, 0x2B1C}, {0x2B50, 0x2B50},
    {0x2B55, 0x2B55}, {0x2E80, 0x2E99}, {0x2E9B, 0x2EF3}, {0x2F00, 0x2FD5},
    {0x2FF0, 0x3029}, {0x302E, 0x303E}, {0x3041, 0x3096}, {0x309B, 0x30FF},
    {0x3105, 0x312F}, {0x3131, 0x318E}, {0x3190, 0x31E5}, {0x31EF, 0x321E},
    {0x3220, 0x3247}, {0x3250, 0xA48C}, {0xA490, 0xA4C6}, {0xA960, 0xA97C},
    {0xAC00, 0xD7A3}, {0xF900, 0xFA6D}, {0xFA70, 0xFAD9}, {0xFE10, 0xFE19},
    {0xFE30, 0xFE52}, {0xFE54, 0xFE66}, {0xFE68, 0xFE6B}, {0xFF01, 0xFF60},
    {0xFFE0, 0xFFE6}, {0x16FE0, 0x16FE3}, {0x16FF0, 0x16FF1}, {0x17000, 0x187F7},
    {0x18800, 0x18CD5}, {0x18CFF, 0x18D08}, {0x1AFF0, 0x1AFF3}, {0x1AFF5, 0x1AFFB},
    {0x1AFFD, 0x1AFFE}, {0x1B000, 0x1B122}, {0x1B132, 0x1B132}, {0x1B150, 0x1B152},
    {0x1B155, 0x1B155}, {0x1B164, 0x1B167}, {0x1B170, 0x1B2FB}, {0x1D300, 0x1D356},
    {0x1D360, 0x1D376}, {0x1F004, 0x1F004}, {0x1F0CF, 0x1F0CF}, {0x1F18E, 0x1F18E},
    {0x1F191, 0x1F19A}, {0x1F200, 0x1F202}, {0x1F210, 0x1F23B}, {0x1F240, 0x1F248},
    {0x1F250, 0x1F251}, {0x1F260, 0x1F265}, {0x1F300, 0x1F320}, {0x1F32D, 0x1F335},
    {0x1F337, 0x1F37C}, {0x1F37E, 0x1F393}, {0x1F3A0, 0x1F3CA}, {0x1F3CF, 0x1F3D3},
    {0x1F3E0, 0x1F3F0}, {0x1F3F4, 0x1F3F4}, {0x1F3F8, 0x1F43E}, {0x1F440, 0x1F440},
    {0x1F442, 0x1F4FC}, {0x1F4FF, 0x1F53D}, {0x1F54B, 0x1F54E}, {0x1F550, 0x1F567},
    {0x1F57A, 0x1F57A}, {0x1F595, 0x1F596}, {0x1F5A4, 0x1F5A4}, {0x1F5FB, 0x1F64F},
    {0x1F680, 0x1F6C5}, {0x1F6CC, 0x1F6CC}, {0x1F6D0, 0x1F6D2}, {0x1F6D5, 0x1F6D7},
    {0x1F6DC, 0x1F6DF}, {0x1F6EB, 0x1F6EC}, {0x1F6F4, 0x1F6FC}, {0x1F7E0, 0x1F7EB},
    {0x1F7F0, 0x1F7F0}, {0x1F90C, 0x1F93A}, {0x1F93C, 0x1F945}, {0x1F947, 0x1F9FF},
    {0x1FA70, 0x1FA7C}, {0x1FA80, 0x1FA89}, {0x1FA8F, 0x1FAC6}, {0x1FACE, 0x1FADC},
    {0x1FADF, 0x1FAE9}, {0x1FAF0, 0x1FAF8}, {0x20000, 0x2A6DF}, {0x2A700, 0x2B739},
    {0x2B740, 0x2B81D}, {0x2B820, 0x2CEA1}, {0x2CEB0, 0x2EBE0}, {0x2EBF0, 0x2EE5D},
    {0x2F800, 0x2FA1D}, {0x30000, 0x3134A}, {0x31350, 0x323AF}};
// lookup table
static constexpr unsigned char utf8Len[256] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

inline bool bisearch(uint32_t ucs, const Range* interval, int max)
{
  int min = 0;

  if (ucs < interval[0].first || ucs > interval[max].last) return false;

  while (max >= min)
  {
    const int mid = (min + max) / 2;

    if (ucs > interval[mid].last) min = mid + 1;
    else if (ucs < interval[mid].first) max = mid - 1;
    else return true;
  }

  return false;
}

inline bool bisearch(const std::vector<size_t>& vec, size_t value)
{
  size_t left = 0;
  size_t right = vec.size();

  while (left < right)
  {
    size_t mid = left + (right - left) / 2;

    if (vec[mid] == value) return true;

    if (vec[mid] < value) left = mid + 1;
    else right = mid;
  }
  return false;
}

struct Str {
  std::string str;
  size_t dw;
};
}

namespace string_utils {
inline bool isSpace(const char c)
{
  return c == ' ' || c == '\n' || c == '\t' || c == '\v' || c == '\f' || c == '\r';
}

constexpr bool isAscii(const char character)
{
  return static_cast<unsigned char>(character) <= 127;
}

constexpr bool isAlpha(const char character)
{
  return (character >= 'A' && character <= 'Z') ||
         (character >= 'a' && character <= 'z');
}

// convert a utf8 encoded sequence to a wide characters
// returns true if it's a valid utf8 string
inline bool utf8twc(const char* s, uint32_t& wc, int& consumed)
{
  const auto* u = reinterpret_cast<const unsigned char*>(s);
  const unsigned char c = u[0];

  // ascii
  if (c < 0x80)
  {
    wc = static_cast<uint32_t>(c);
    consumed = 1;
    return true;
  }

  // utf8
  consumed = detail::utf8Len[c];
  if (consumed == 0) return false;

  switch (consumed)
  {
  case 2:
    if ((u[1] & 0xC0) != 0x80) return false;
    wc = ((c & 0x1F) << 6) | (u[1] & 0x3F);
    return wc >= 0x80;

  case 3:
    if ((u[1] & 0xC0) != 0x80 || (u[2] & 0xC0) != 0x80) return false;
    wc = ((c & 0x0F) << 12) | ((u[1] & 0x3F) << 6) | (u[2] & 0x3F);
    return wc >= 0x800 && (wc < 0xD800 || wc > 0xDFFF);

  case 4:
    if ((u[1] & 0xC0) != 0x80 || (u[2] & 0xC0) != 0x80 || (u[3] & 0xC0) != 0x80)
      return false;
    wc = ((c & 0x07) << 18) | ((u[1] & 0x3F) << 12) | ((u[2] & 0x3F) << 6) |
                (u[3] & 0x3F);
    return wc >= 0x10000 && wc <= 0x10FFFF;

  default:
    return false;
  }
}


// return the display width of a Unicode character
inline size_t wcwidth(const uint32_t ucs)
{
  using namespace detail;

  if (ucs == 0) return 0;
  if (ucs < 32 || (ucs >= 0x7f && ucs < 0xa0)) return 0;

  if (bisearch(ucs, combining,
               (sizeof(combining) / sizeof(std::pair<uint32_t, uint32_t>)) - 1))
    return 0;

  if (bisearch(ucs, wide,
               (sizeof(wide) / sizeof(std::pair<uint32_t, uint32_t>)) - 1))
    return 2;

  return 1;
}

// calculates the display width of a given utf8 string
inline size_t dw(const char* str)
{
  using namespace detail;

  size_t width = 0;
  const char* ptr = str;

  while (*ptr)
  {
    // skip ansi escape sequences
    if (*ptr == '\x1b')
    {
      // skip every ascii character until a non-ascii one
      // or an alphabet is found
      ++ptr;
      while (*ptr && isAscii(*ptr) && !isAlpha(*ptr))
        ++ptr;

      // skip the first alphabet too
      if (*ptr && isAlpha(*ptr)) ++ptr;
      continue;
    }

    // UTF-8 characters
    uint32_t wc;
    int consumed = 1;
    if (utf8twc(ptr, wc, consumed))
    {
      width += string_utils::wcwidth(wc);
      ptr += consumed;
    }

    // invalid UTF-8 are treated as 1 width;
    else
    {
      ++width;
      ++ptr;
    }
  }

  return width;
}

inline size_t dw(const std::string& str)
{
  return dw(str.c_str());
}

inline std::string readUtf8Char(const std::string& str, const size_t pos)
{
  if (pos >= str.length()) return "";

  const auto first_byte = static_cast<unsigned char>(str[pos]);

  // if it's a continuation byte then it's invalid
  if ((first_byte & 0xC0) == 0x80) return "";

  // find the length of the sequence from the start byte
  size_t len;
  if ((first_byte & 0x80) == 0)
    len = 1;
  else if ((first_byte & 0xE0) == 0xC0)
    len = 2;
  else if ((first_byte & 0xF0) == 0xE0)
    len = 3;
  else if ((first_byte & 0xF8) == 0xF0)
    len = 4;
  else
    return "";

  // not enough bytes
  if (pos + len > str.length()) return "";

  // validate
  for (size_t i = 1; i < len; ++i)
  {
    if ((static_cast<unsigned char>(str[pos + i]) & 0xC0) != 0x80)
    {
      return "";
    }
  }

  return str.substr(pos, len);
}

inline bool endsWith(const std::string& str, const std::string& with)
{
  const size_t len = with.size();
  const size_t n = str.size();
  if (n < len) return false;
  return str.compare(n - len, len, with) == 0;
}
}

enum class Color : uint8_t {
  Black = 30,
  Red,
  Green,
  Yellow,
  Blue,
  Magenta,
  Cyan,
  White,

  BrightBlack = 90,
  BrightRed,
  BrightGreen,
  BrightYellow,
  BrightBlue,
  BrightMagenta,
  BrightCyan,
  BrightWhite,
};
struct Rgb {
  uint8_t r = 0;
  uint8_t g = 0;
  uint8_t b = 0;

  constexpr Rgb() = default;
  constexpr Rgb(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}
  constexpr Rgb(uint32_t hex)
      : r((hex >> 16) & 0xFF), g((hex >> 8) & 0xFF), b(hex & 0xFF)
  {
  }
  constexpr uint32_t toHex() const
  {
    return (static_cast<uint32_t>(r) << 16) | (static_cast<uint32_t>(g) << 8) |
           (static_cast<uint32_t>(b));
  }
};

namespace detail {
class ColorType {
public:
  ColorType(uint32_t value)
    : value_(value) {}

  bool isSet() { return this->value_ != 0; }
  bool isColor() { return isSet() && (this->value_ & (1u << 24)) == 0; }
  bool isRgb() { return isSet() && (this->value_ & (1u << 24)) != 0; }

  Color color() { return static_cast<Color>(this->value_ & 0xFFFFFF); }
  Rgb rgb() { return {this->value_ & 0xFFFFFF}; }

private:
  uint32_t value_;
};
}

enum class Attribute : uint16_t {
  Bold = 1,
  Dim = 1 << 1,
  Italic = 1 << 2,

  Underline = 1 << 3,
  Dunderline = 1 << 4, // not popular

  Blink = 1 << 5, // rarely supported
  Flink = 1 << 6, // rarely supported
  Reverse = 1 << 7,
  Concealed = 1 << 8,
  Crossed = 1 << 9,
};

enum class Alignment { Left, Right, Center };
struct Padding {
  uint8_t top = 0;
  uint8_t bottom = 0;
  uint8_t left = 1;
  uint8_t right = 1;

  constexpr Padding() = default;
  constexpr Padding(uint8_t vertical, uint8_t horizontal)
      : top(vertical), bottom(vertical), left(horizontal), right(horizontal)
  {
  }
  constexpr Padding(uint8_t top, uint8_t bottom, uint8_t left, uint8_t right)
      : top(top), bottom(bottom), left(left), right(right)
  {
  }
};

constexpr auto RESET_ESC = "\x1b[0m";
constexpr uint8_t WORD_LENGTH_AVERAGE = 5;
constexpr uint8_t MIN_COLUMN_WIDTH = 2;

class Column {
public:
  class Style {
  public:
    explicit Style(Column& parent) : parent(parent)
    {
    }

    Style& fg(Color color)
    {
      this->fg_ = static_cast<uint32_t>(color);
      this->parent.makeDirty();
      return *this;
    }
    Style& bg(Color color)
    {
      this->bg_ = static_cast<uint32_t>(color);
      this->parent.makeDirty();
      return *this;
    }
    Style& base(Color color)
    {
      this->base_ = static_cast<uint32_t>(color);
      this->parent.makeDirty();
      return *this;
    }

    Style& fg(const Rgb rgb)
    {
      this->fg_ = rgb.toHex() | (1u << 24);
      this->parent.makeDirty();
      return *this;
    }
    Style& bg(const Rgb rgb)
    {
      this->bg_ = rgb.toHex() | (1u << 24);
      this->parent.makeDirty();
      return *this;
    }
    Style& base(const Rgb rgb)
    {
      this->base_ = rgb.toHex() | (1u << 24);
      this->parent.makeDirty();
      return *this;
    }

    Style& attrs(Attribute attr)
    {
      this->attrs_ |= static_cast<uint16_t>(attr);
      this->parent.makeDirty();
      return *this;
    }
    Style& attrs(const Style& attr)
    {
      this->attrs_ |= attr.attrs_;
      this->parent.makeDirty();
      return *this;
    }

    bool hasFg() const { return this->fg_ != 0; }
    bool hasBg() const { return this->bg_ != 0; }
    bool hasBase() const { return this->base_ != 0; }
    bool hasAttrs() const { return this->attrs_ != 0; }

    uint32_t fg() const { return this->fg_; }
    uint32_t bg() const { return this->bg_; }
    uint32_t base() const { return this->base_; }
    Attribute attrs() const { return static_cast<Attribute>(attrs_); }

    void resetFg()
    {
      this->fg_ = 0;
      this->parent.makeDirty();
    }
    void resetBg()
    {
      this->bg_ = 0;
      this->parent.makeDirty();
    }
    void resetBase()
    {
      this->base_ = 0;
      this->parent.makeDirty();
    }
    void resetAttrs()
    {
      this->attrs_ = 0;
      this->parent.makeDirty();
    }
    void reset()
    {
      this->fg_ = 0;
      this->bg_ = 0;
      this->base_ = 0;
      this->attrs_ = 0;
      this->parent.makeDirty();
    }

  private:
    Column& parent;

    // representing all the colors
    uint32_t fg_ = 0;
    uint32_t bg_ = 0;
    uint32_t base_ = 0;

    // all the attributes
    uint16_t attrs_ = 0;
  };
  class Config {
  public:
    explicit Config(Column& parent) : parent(parent)
    {
    }

    Alignment align() const { return this->align_; }
    Padding padd() const { return this->padd_; }
    size_t width() const { return this->width_; }
    std::string delimiter() const { return this->delimiter_; }
    bool skipEmptyLineIndent() const { return this->skipEmptyLineIndent_; }

    Config& align(const Alignment alignment)
    {
      this->align_ = alignment;
      this->parent.makeDirty();
      return *this;
    }
    Config& padd(const Padding padd)
    {
      this->padd_ = padd;
      this->parent.makeDirty();
      return *this;
    }
    Config& width(const size_t width)
    {
      this->width_ = width;
      this->parent.makeDirty();
      return *this;
    }
    Config& delimiter(std::string delimiter)
    {
      this->delimiter_ = std::move(delimiter);
      this->parent.makeDirty();
      return *this;
    }
    Config& skipEmptyLineIndent(const bool skip)
    {
      this->skipEmptyLineIndent_ = skip;
      this->parent.makeDirty();
      return *this;
    }

    void reset()
    {
      this->align_ = Alignment::Left;
      this->padd_ = Padding();
      this->delimiter_ = "-";
      this->width_ = 0;
      this->parent.makeDirty();
    }

  private:
    Column& parent;

    Alignment align_ = Alignment::Left;
    Padding padd_ = Padding();
    std::string delimiter_ = "-";
    size_t width_ = 0;
    bool skipEmptyLineIndent_ = true;
  };

public:
  Column() = default;

  Column(std::string content)
    : content_(std::move(content)), dirty_(true)
  {
  }

  void content(std::string content)
  {
    this->content_ = std::move(content);
    makeDirty();
  }

  Config& config() { return this->config_; }
  Style& style() { return this->style_; }

  const Config& config() const { return this->config_; }
  const Style& style() const { return this->style_; }

  const std::string& content() const { return this->content_; }
  std::string& content()
  {
    makeDirty();
    return this->content_;
  }

  explicit operator const std::string&() const
  {
    return this->content_;
  }
  explicit operator std::string&()
  {
    makeDirty();
    return this->content_;
  }

  char& operator[](int index)
  {
    makeDirty();
    return this->content_.at(index);
  }
  const char& operator[](int index) const
  {
    return this->content_.at(index);
  }

  void clr()
  {
    this->content_.clear();
    this->config_.reset();
    this->style_.reset();
    this->lines_.clear();
    makeClean();
  }

  const std::vector<std::string>& lines() const
  {
    if (dirty_)
    {
      lines_ = genLines();
      makeClean();
    }
    return lines_;
  }
  std::string genEmptyLine() const
  {
    const std::string base = resolveBase();
    const size_t width = config().width();

    std::string empty = base;
    empty.reserve(width);

    empty += std::string(width, ' ');
    if (!base.empty()) empty += RESET_ESC;

    return empty;
  }

private:
  Config config_{*this};
  Style style_{*this};
  std::string content_;

  // cache
  mutable std::vector<std::string> lines_;
  mutable bool dirty_ = false;

  void makeDirty() const { this->dirty_ = true; }
  void makeClean() const { this->dirty_ = false; }

  std::vector<std::string> genLines() const
  {
    std::string delimiter = config().delimiter();
    size_t delimiterDw = string_utils::dw(delimiter);
    Padding padd = config().padd();

    size_t width = config().width();
    if (width < MIN_COLUMN_WIDTH) width = MIN_COLUMN_WIDTH;

    if (padd.right + padd.left >= width)
    {
      padd.right = 0;
      padd.left = 0;
    }

    width -= (padd.left + padd.right);

    if (delimiterDw >= width)
    {
      delimiter = "";
      delimiterDw = 0;
    }

    // split the content into words
    const auto words = split(this->content_);

    // wrap the words into lines
    const std::vector<detail::Str> lines =
        wrap(words, width, delimiter, delimiterDw);

    // format the lines handling padding, alignment and the base styles
    return format(lines, padd);
  }
  static void handleColor(std::string& styles, detail::ColorType colortype,
                          bool back)
  {
    if (colortype.isColor())
    {
      Color c = colortype.color();
      styles += std::to_string(static_cast<uint8_t>(c) + (back ? 10 : 0)) + ';';
    }

    else if (colortype.isRgb())
    {
      Rgb rgb = colortype.rgb();
      styles += back ? "48;2;" : "38;2;";
      styles += std::to_string(rgb.r) + ';';
      styles += std::to_string(rgb.g) + ';';
      styles += std::to_string(rgb.b) + ';';
    }
  }
  static void handleAttrs(std::string& styles, Attribute attr)
  {
    auto hasAttr = [](Attribute attr, Attribute flag) -> bool {
      return static_cast<uint16_t>(attr) & static_cast<uint16_t>(flag);
    };

    if (hasAttr(attr, Attribute::Bold)) styles += "1;";
    if (hasAttr(attr, Attribute::Dim)) styles += "2;";
    if (hasAttr(attr, Attribute::Italic)) styles += "3;";
    if (hasAttr(attr, Attribute::Underline)) styles += "4;";
    if (hasAttr(attr, Attribute::Dunderline)) styles += "21;";
    if (hasAttr(attr, Attribute::Blink)) styles += "5;";
    if (hasAttr(attr, Attribute::Flink)) styles += "6;";
    if (hasAttr(attr, Attribute::Reverse)) styles += "7;";
    if (hasAttr(attr, Attribute::Concealed)) styles += "8;";
    if (hasAttr(attr, Attribute::Crossed)) styles += "9;";
  }

  std::string resolveStyles() const
  {
    std::string styles = "\x1b[";

    if (style().hasAttrs()) handleAttrs(styles, style().attrs());

    if (style().hasFg()) handleColor(styles, style().fg(), false);

    if (style().hasBg()) handleColor(styles, style().bg(), true);

    if (styles == "\x1b[") return ""; // empty

    if (styles.back() == ';') styles.back() = 'm';
    return styles;
  }
  std::string resolveBase() const
  {
    std::string base = "\x1b[";
    handleColor(base, style().base(), true);

    if (base == "\x1b[") return ""; // empty

    if (base.back() == ';') base.back() = 'm';
    return base;
  }

  std::vector<std::string> split(const std::string& str) const
  {
    using namespace string_utils;

    const size_t len = str.length();

    std::vector<std::string> words;
    words.reserve((len / WORD_LENGTH_AVERAGE) + 1); // average

    std::string buffer;
    buffer.reserve(WORD_LENGTH_AVERAGE);

    size_t i = 0;
    while (i < len)
    {
      if (str[i] == '\x1b')
      {
        if (!buffer.empty()) words.emplace_back(std::move(buffer));
        buffer.clear();

        while (i < len && isAscii(str[i]) && !isAlpha(str[i]))
          buffer += str[i++];

        if (i < len) buffer += str[i++];
        words.emplace_back(std::move(buffer));
        buffer.clear();
        continue;
      }

      if (isSpace(str[i]))
      {
        if (!buffer.empty()) words.emplace_back(std::move(buffer));
        buffer.clear();

        words.emplace_back(1, str[i++]);
        continue;
      }

      buffer += str[i++];
    }

    if (!buffer.empty()) words.emplace_back(std::move(buffer));
    return words;
  }
  std::vector<detail::Str> wrap(const std::vector<std::string>& words,
                                const size_t width,
                                const std::string& delimiter,
                                const size_t delimiterDw) const
  {
    using namespace string_utils;

    const std::string styles = resolveStyles();
    const bool skipBlanks = this->config_.skipEmptyLineIndent();

    std::vector<detail::Str> lines;
    lines.reserve(this->content_.length() / width + 1);

    std::string buffer;
    size_t bufferDw = 0; // the display width

    buffer.reserve(width);
    if (!styles.empty()) buffer += styles;

    // in case the line contains active escape sequences, and they were NOT
    // reset (with '\x1b[0m'), we need to register them, reset them
    // at the end of the line, and restore them in the next line.
    std::string activeEscs;

    // helper lambdas to avoid repeating code
    auto appendResetIfNeeded = [&]() {
      if (!(activeEscs.empty() && styles.empty()) && !endsWith(buffer, RESET_ESC))
        buffer += RESET_ESC;
    };
    auto startNewLine = [&]() {
      appendResetIfNeeded();
      lines.push_back({buffer, bufferDw});

      buffer.clear();
      bufferDw = 0;

      if (!styles.empty()) buffer += styles;
      if (!activeEscs.empty()) buffer += activeEscs;
    };

    for (size_t i = 0; i < words.size(); ++i)
    {
      std::string word = words[i];

      // SKIP empty strings
      if (word.empty()) continue;

      // SKIP spaces at the start of a new line
      if (skipBlanks && buffer.empty() && word == " ") continue;

      // HANDLE new lines
      if (word == "\n")
      {
        startNewLine();
        continue;
      }

      // IGNORE other space characters
      if (word.length() == 1 && isSpace(word[0]) && word != " ") continue;

      // HANDLE escape sequences
      if (word[0] == '\x1b')
      {
        if (word.back() != 'm') continue;

        const auto* nextWord = i + 1 < words.size() ? &words[i + 1] : nullptr;
        if (word == "\x1b[0m")
        {
          activeEscs.clear();
          buffer += word;

          if (nextWord && dw(*nextWord) + bufferDw <= width) buffer += styles;

          continue;
        }

        activeEscs += word;

        if (nextWord && dw(*nextWord) + bufferDw <= width) buffer += word;

        continue;
      }

      // word display width
      size_t wordDw = dw(word);

      // the word fits in the line
      if (wordDw + bufferDw <= width)
      {
        buffer += word;
        bufferDw += wordDw;
        continue;
      }

      // the word fits in the next line
      if (wordDw <= width)
      {
        startNewLine();

        // add the next word and avoid spaces
        if (word != " ")
        {
          buffer += word;
          bufferDw += wordDw;
        }

        continue;
      }

      /*
       * if we reach here that means the word's display width
       * exceeds the line width
       */

      // no free space, append the current line and process the others
      if (bufferDw >= width - delimiterDw) startNewLine();

      while (wordDw > width)
      {
        size_t limit = width - delimiterDw - bufferDw;

        std::string firstPart;
        size_t firstPartDw = 0;
        firstPart.reserve(limit);

        size_t pos = 0;
        while (firstPartDw < limit)
        {
          std::string utf8char = readUtf8Char(word, pos);
          size_t utf8charDw = dw(utf8char);

          // if it will exceed the limit don't append
          if (utf8charDw + firstPartDw > limit) break;

          // otherwise append this part
          firstPart += utf8char;
          firstPartDw += utf8charDw;

          if (utf8char.empty()) pos++;
          else pos += utf8char.length();
        }

        // prepare the next word
        word = word.substr(pos);
        wordDw -= firstPartDw;

        // add the delimiter
        firstPart += delimiter;
        firstPartDw += delimiterDw;

        // add the stuff into the buffer
        buffer += firstPart;
        bufferDw += firstPartDw;

        // append a new line
        startNewLine();
      }

      buffer += word;
      bufferDw += wordDw;
    }

    if (!buffer.empty())
    {
      appendResetIfNeeded();
      lines.push_back({buffer, bufferDw});
    }

    return lines;
  }
  std::vector<std::string> format(const std::vector<detail::Str>& lines,
                                  const Padding padd) const
  {
    using namespace string_utils;

    const size_t width = config().width();
    const std::string base = resolveBase();
    const Alignment align = config().align();

    std::vector<std::string> formatted;
    formatted.reserve(lines.size() + padd.top + padd.bottom);

    const bool styled = !base.empty();
    const std::string empty = genEmptyLine();
    formatted.insert(formatted.end(), padd.top, empty);

    // temporary buffer
    std::string buffer;
    buffer.reserve(width);

    for (const auto& line : lines)
    {
      // append the base styles
      if (styled) buffer += base;

      // calculate the total line width
      const size_t lineWidth = line.dw + padd.left + padd.right;
      const size_t freeSpace = (width > lineWidth) ? width - lineWidth : 0;

      size_t leftSpace = 0, rightSpace = 0;
      switch (align)
      {
      case Alignment::Left:
        leftSpace = 0;
        rightSpace = freeSpace;
        break;
      case Alignment::Center: {
        leftSpace = freeSpace / 2;
        rightSpace = freeSpace - leftSpace;
        break;
      }
      case Alignment::Right:
        leftSpace = freeSpace;
        rightSpace = 0;
        break;
      }

      buffer.append(leftSpace + padd.left, ' ');
      buffer += line.str;

      if (endsWith(line.str, RESET_ESC)) buffer += base;

      buffer.append(rightSpace + padd.right, ' ');

      // reset the base styles
      if (styled) buffer += RESET_ESC;

      formatted.emplace_back(std::move(buffer));
      buffer.clear();
    }

    formatted.insert(formatted.end(), padd.bottom, empty);
    return formatted;
  }
};

inline Attribute operator|(Attribute lhs, Attribute rhs) noexcept
{
  return static_cast<Attribute>(static_cast<uint16_t>(lhs) |
                                static_cast<uint16_t>(rhs));
}

inline Attribute& operator|=(Attribute& lhs, const Attribute rhs) noexcept
{
  lhs = lhs | rhs;
  return lhs;
}

class Border {
public:
  class Part {
  public:
    Part(const uint32_t glyph)
      : glyph_(glyph), dirty_(true)
    {
    }

    Part& glyph(const uint32_t glyph)
    {
      this->glyph_ = glyph;
      makeDirty();
      return *this;
    }

    Part& fg(const Color color)
    {
      this->fg_ = static_cast<uint32_t>(color);
      makeDirty();
      return *this;
    }
    Part& fg(const Rgb rgb)
    {
      this->fg_ = rgb.toHex() | (1u << 24);
      makeDirty();
      return *this;
    }

    Part& bg(const Color color)
    {
      this->bg_ = static_cast<uint32_t>(color);
      makeDirty();
      return *this;
    }
    Part& bg(const Rgb rgb)
    {
      this->bg_ = rgb.toHex() | (1u << 24);
      makeDirty();
      return *this;
    }

    uint32_t fg() const
    {
      return this->fg_;
    }
    uint32_t bg() const
    {
      return this->bg_;
    }
    uint32_t glyph() const
    {
      return this->glyph_;
    }

    Part& clrFg()
    {
      this->fg_ = 0;
      makeDirty();
      return *this;
    }
    Part& clrBg()
    {
      this->bg_ = 0;
      makeDirty();
      return *this;
    }
    Part& clr()
    {
      this->fg_ = 0;
      this->bg_ = 0;
      makeDirty();
      return *this;
    }

    explicit operator const std::string&() const { return this->str(); }
    const std::string& str() const
    {
      if (this->dirty_)
      {
        this->str_ = genStr();
        makeClean();
      }

      return this->str_;
    }

  private:
    uint32_t glyph_ = 0; // Unicode code point
    uint32_t fg_ = 0; // fg color
    uint32_t bg_ = 0; // bg color

    // cache
    mutable bool dirty_ = false;
    mutable std::string str_;

    void makeDirty() const { this->dirty_ = true; }
    void makeClean() const { this->dirty_ = false; }

    std::string genStr() const
    {
      using namespace detail;
      std::string buffer;

      ColorType fg = this->fg_;
      ColorType bg = this->bg_;

      if (fg.isColor())
      {
        buffer.append("\x1b[");
        buffer.append(std::to_string(static_cast<uint8_t>(fg.color())));
        buffer.push_back(';');
      }
      else if (fg.isRgb())
      {
        Rgb rgb = fg.rgb();
        buffer.append("\x1b[38;2;");

        buffer.append(std::to_string(rgb.r));
        buffer.push_back(';');
        buffer.append(std::to_string(rgb.g));
        buffer.push_back(';');
        buffer.append(std::to_string(rgb.b));
        buffer.push_back(';');
      }

      if (bg.isColor())
      {
        if (buffer.empty()) buffer.append("\x1b[");
        buffer.append(std::to_string(static_cast<uint8_t>(bg.color()) + 10));
        buffer.push_back(';');
      }
      else if (bg.isRgb())
      {
        Rgb rgb = bg.rgb();
        if (buffer.empty()) buffer.append("\x1b[48;2;");
        buffer.append(std::to_string(rgb.r));
        buffer.push_back(';');
        buffer.append(std::to_string(rgb.g));
        buffer.push_back(';');
        buffer.append(std::to_string(rgb.b));
        buffer.push_back(';');
      }

      const std::string glyph = glyphToStr();
      buffer.append(glyph);

      if (buffer.length() - glyph.length() > 0)
        buffer.append(RESET_ESC);

      return buffer;
    }
    std::string glyphToStr() const
    {
      std::string result;

      if (this->glyph_ <= 0x7F)
      {
        result += static_cast<char>(this->glyph_);
      }

      else if (this->glyph_ <= 0x7FF)
      {
        result += static_cast<char>(0xC0 | ((this->glyph_ >> 6) & 0x1F));
        result += static_cast<char>(0x80 | (this->glyph_ & 0x3F));
      }

      else if (this->glyph_ <= 0xFFFF)
      {
        result += static_cast<char>(0xE0 | ((this->glyph_ >> 12) & 0x0F));
        result += static_cast<char>(0x80 | ((this->glyph_ >> 6) & 0x3F));
        result += static_cast<char>(0x80 | (this->glyph_ & 0x3F));
      }

      else
      {
        result += static_cast<char>(0xF0 | ((this->glyph_ >> 18) & 0x07));
        result += static_cast<char>(0x80 | ((this->glyph_ >> 12) & 0x3F));
        result += static_cast<char>(0x80 | ((this->glyph_ >> 6) & 0x3F));
        result += static_cast<char>(0x80 | (this->glyph_ & 0x3F));
      }

      return result;
    }
  };

  Part& horizontal() { return this->horizontal_; }
  Part& vertical() { return this->vertical_; }
  Part& cornerTopLeft() { return this->cornerTopLeft_; }
  Part& cornerTopRight() { return this->cornerTopRight_; }
  Part& cornerBottomLeft() { return this->cornerBottomLeft_; }
  Part& cornerBottomRight() { return this->cornerBottomRight_; }
  Part& intersection() { return this->intersection_; }
  Part& connectorLeft() { return this->connectorLeft_; }
  Part& connectorRight() { return this->connectorRight_; }
  Part& connectorTop() { return this->connectorTop_; }
  Part& connectorBottom() { return this->connectorBottom_; }

  const Part& horizontal() const { return this->horizontal_; }
  const Part& vertical() const { return this->vertical_; }
  const Part& cornerTopLeft() const { return this->cornerTopLeft_; }
  const Part& cornerTopRight() const { return this->cornerTopRight_; }
  const Part& cornerBottomLeft() const { return this->cornerBottomLeft_; }
  const Part& cornerBottomRight() const { return this->cornerBottomRight_; }
  const Part& intersection() const { return this->intersection_; }
  const Part& connectorLeft() const { return this->connectorLeft_; }
  const Part& connectorRight() const { return this->connectorRight_; }
  const Part& connectorTop() const { return this->connectorTop_; }
  const Part& connectorBottom() const { return this->connectorBottom_; }

  Border& horizontal(const uint32_t glyph)
  {
    this->horizontal_.glyph(glyph);
    return *this;
  }
  Border& vertical(const uint32_t glyph)
  {
    this->vertical_.glyph(glyph);
    return *this;
  }
  Border& cornerTopLeft(const uint32_t glyph)
  {
    this->cornerTopLeft_.glyph(glyph);
    return *this;
  }
  Border& cornerTopRight(const uint32_t glyph)
  {
    this->cornerTopRight_.glyph(glyph);
    return *this;
  }
  Border& cornerBottomLeft(const uint32_t glyph)
  {
    this->cornerBottomLeft_.glyph(glyph);
    return *this;
  }
  Border& cornerBottomRight(const uint32_t glyph)
  {
    this->cornerBottomRight_.glyph(glyph);
    return *this;
  }
  Border& intersection(const uint32_t glyph)
  {
    this->intersection_.glyph(glyph);
    return *this;
  }
  Border& connectorLeft(const uint32_t glyph)
  {
    this->connectorLeft_.glyph(glyph);
    return *this;
  }
  Border& connectorRight(const uint32_t glyph)
  {
    this->connectorRight_.glyph(glyph);
    return *this;
  }
  Border& connectorTop(const uint32_t glyph)
  {
    this->connectorTop_.glyph(glyph);
    return *this;
  }
  Border& connectorBottom(const uint32_t glyph)
  {
    this->connectorBottom_.glyph(glyph);
    return *this;
  }

  static Border Default()
  {
    // already default constructed
    return {};
  }
  static Border Filled(const uint32_t c)
  {
    Border border;
    border.horizontal_ = c;
    border.vertical_ = c;

    border.cornerTopLeft_ = c;
    border.cornerTopRight_ = c;
    border.cornerBottomLeft_ = c;
    border.cornerBottomRight_ = c;

    border.intersection_ = c;

    border.connectorLeft_ = c;
    border.connectorRight_ = c;
    border.connectorTop_ = c;
    border.connectorBottom_ = c;

    return border;
  }
  static Border None()
  {
    return Filled(U'\0');
  }
  static Border Blank()
  {
    return Filled(U' ');
  }
  static Border Modern()
  {
    Border border;
    border.horizontal_ = U'─';
    border.vertical_ = U'│';

    border.cornerTopLeft_ = U'┌';
    border.cornerTopRight_ = U'┐';
    border.cornerBottomLeft_ = U'└';
    border.cornerBottomRight_ = U'┘';

    border.intersection_ = U'┼';

    border.connectorLeft_ = U'├';
    border.connectorRight_ = U'┤';
    border.connectorTop_ = U'┬';
    border.connectorBottom_ = U'┴';

    return border;
  }

  void reset()
  {
    this->horizontal_ = U'-'; // ─
    this->vertical_ = U'|'; // │

    this->cornerTopLeft_ = U'+'; // ┌
    this->cornerTopRight_ = U'+'; // ┐
    this->cornerBottomLeft_ = U'+'; // └
    this->cornerBottomRight_ = U'+'; // ┘

    this->intersection_ = U'+'; // ┼

    this->connectorLeft_ = U'+'; // ├
    this->connectorRight_ = U'+'; // ┤
    this->connectorTop_ = U'-'; // ┬
    this->connectorBottom_ = U'-'; // ┴
  }

private:
  Part horizontal_ = U'-'; // ─
  Part vertical_ = U'|'; // │

  Part cornerTopLeft_ = U'+'; // ┌
  Part cornerTopRight_ = U'+'; // ┐
  Part cornerBottomLeft_ = U'+'; // └
  Part cornerBottomRight_ = U'+'; // ┘

  Part intersection_ = U'+'; // ┼

  Part connectorLeft_ = U'+'; // ├
  Part connectorRight_ = U'+'; // ┤
  Part connectorTop_ = U'-'; // ┬
  Part connectorBottom_ = U'-'; // ┴
};

class Row {
public:
  class Config {
  public:
    explicit Config(Row& parent)
      : parent(parent)
    {
    }

    void width(const size_t width)
    {
      parent.makeDirty();
      this->width_ = width;
    }
    void hasBottom(bool has)
    {
      this->parent.makeDirty();
      this->hasBottom_ = has;
    }
    void vertical(Border::Part part)
    {
      this->parent.makeDirty();
      this->vertical_ = std::move(part);
    }

    size_t width() const { return this->width_; }
    bool hasBottom() const { return this->hasBottom_; }
    const Border::Part& vertical() const { return this->vertical_; }

    void reset()
    {
      this->width_ = 50;
      this->vertical_ = 0;
      this->parent.makeDirty();
    }

  private:
    Row& parent;
    size_t width_ = 50;
    bool hasBottom_ = true;
    Border::Part vertical_ = 0;
  };

public:
  Row() = default;

  explicit Row(std::vector<Column> columns)
    : columns_(std::move(columns))
  {
  }
  explicit Row(std::vector<std::string> columns)
  {
    this->columns_.reserve(columns.size());

    for (auto& column : columns)
    {
      this->columns_.emplace_back(std::move(column));
    }
  }

  void columns(std::vector<Column> columns)
  {
    makeDirty();
    this->columns_ = std::move(columns);
  }

  Config& config() { return this->config_; }
  const Config& config() const { return this->config_; }

  std::vector<Column>& columns()
  {
    makeDirty();
    return this->columns_;
  }
  const std::vector<Column>& columns() const { return this->columns_; }

  Column& column(int index)
  {
    makeDirty();
    return this->columns_.at(index);
  }
  const Column& column(int index) const
  {
    return this->columns_.at(index);
  }

  Column& operator[](int index)
  {
    makeDirty();
    return this->columns_.at(index);
  }
  const Column& operator[](int index) const
  {
    return this->columns_.at(index);
  }

  void clr()
  {
    this->columns_.clear();
    this->str_.clear();
    this->config_.reset();
    makeClean();
  }

  const std::string& str() const
  {
    if (this->dirty_)
    {
      this->str_ = genStr();
      makeClean();
    }

    return this->str_;
  }

private:
  std::vector<Column> columns_;
  Config config_{*this};

  // cache
  mutable bool dirty_ = false;
  mutable std::string str_;

  void makeDirty() const { this->dirty_ = true; }
  void makeClean() const { this->dirty_ = false; }

  std::string genStr() const
  {
    if (this->columns_.empty()) return "";
    const size_t maxLines = getMaxLines();

    std::string rowStr;
    rowStr.reserve(this->config_.width() * maxLines + (this->columns_.size() + 1));
    const auto& vertical = config().vertical().str();

    for (size_t i = 0; i < maxLines; ++i)
    {
      if (!rowStr.empty()) rowStr.push_back('\n');
      rowStr.append(vertical);

      for (const auto& column : this->columns_)
      {
        const auto& lines = column.lines();

        if (lines.size() > i)
          rowStr.append(lines[i]);
        else
          rowStr.append(column.genEmptyLine());

        rowStr.append(vertical);
      }
    }

    return rowStr;
  }
  size_t getMaxLines() const
  {
    size_t maxLines = 0;

    for (const auto& column : this->columns_)
      maxLines = (std::max)(maxLines, column.lines().size());

    return maxLines;
  }
};

class Table {
public:
  class Config {
  public:
    explicit Config(Table& table) : parent_(table)
    {
    }

    void width(const size_t width)
    {
      parent_.makeDirty();
      this->width_ = width;
    }

    size_t width() const { return this->width_; }

    void reset()
    {
      this->width_ = 50;
      this->parent_.makeDirty();
    }

  private:
    Table& parent_;
    size_t width_ = 50;
  };

public:
  Table() = default;

  explicit Table(std::vector<Row> rows, Border border = {})
    : rows_(std::move(rows)), border_(std::move(border)), dirty_(true)
  {
  }

  void rows(std::vector<Row> rows)
  {
    this->rows_ = std::move(rows);
    makeDirty();
  }
  void border(Border border)
  {
    this->border_ = std::move(border);
    makeDirty();
  }

  Table& addRow(Row row)
  {
    this->rows_.emplace_back(std::move(row));
    makeDirty();
    return *this;
  }
  Table& addRow(std::vector<std::string> row)
  {
    this->rows_.emplace_back(std::move(row));
    makeDirty();
    return *this;
  }

  Config& config() { return this->config_; }
  const Config& config() const { return this->config_; }

  Border& border()
  {
    makeDirty();
    return this->border_;
  }
  const Border& border() const { return this->border_; }

  std::vector<Row>& rows()
  {
    makeDirty();
    return this->rows_;
  }
  const std::vector<Row>& rows() const { return this->rows_; }

  Row& row(int index)
  {
    makeDirty();
    return this->rows_.at(index);
  }
  const Row& row(int index) const { return this->rows_.at(index); }

  Row& operator[](int index)
  {
    makeDirty();
    return this->rows_.at(index);
  }
  const Row& operator[](int index) const
  {
    return this->rows_.at(index);
  }

  void clr()
  {
    this->rows_.clear();
    config_.reset();
    border_.reset();
    this->str_.clear();
    makeClean();
  }

  const std::string& str() const
  {
    if (this->dirty_)
    {
      this->str_ = genStr();
      makeClean();
    }

    return this->str_;
  }

private:
  std::vector<Row> rows_;
  Config config_{*this};
  Border border_;

  // cache
  mutable bool dirty_ = false;
  mutable std::string str_;

  void makeDirty() const { dirty_ = true; }
  void makeClean() const { dirty_ = false; }

  std::string genStr() const
  {
    // avoid reference
    auto rows = this->rows_;
    if (rows.empty()) return "";

    std::string tableStr;
    tableStr.reserve(rows.size() * this->config_.width());

    adjustWidth(rows);
    configureRows(rows);
    tableStr += getBorderHeader(rows) + '\n';

    const size_t rowsSize = this->rows_.size();
    for (size_t i = 0; i < rowsSize; ++i)
    {
      auto& row = rows[i];

      tableStr += row.str() + '\n';

      if (i + 1 < rowsSize && row.config().hasBottom())
        tableStr += getBorderMiddle(rows, i) + '\n';
    }

    tableStr += getBorderFooter(rows);
    return tableStr;
  }
  static size_t calculateWidth(const Row& row, size_t& unspecified)
  {
    const auto& columns = row.columns();
    if (columns.empty()) return 0;
    size_t width = columns.size() + 1;

    for (const auto& column : columns)
    {
      size_t w = column.config().width();

      if (w == 0) unspecified++;
      else width += w;
    }

    return width;
  }

  void setWidth(Row& row) const
  {
    auto& columns = row.columns();
    size_t width = this->config_.width();

    // subtract the splits
    width -= (columns.size() + 1);
    size_t indivWidth = width / columns.size();
    size_t rest = width % columns.size();

    for (auto& column : columns)
    {
      column.config().width(indivWidth + (rest > 0));
      rest -= (rest > 0);
    }

    // if there's still a rest
    if (rest > 0) columns.back().config().width(indivWidth + rest);
  }
  void setUnspecifiedWidth(Row& row, size_t unspecified) const
  {
    std::vector<Column>& columns = row.columns();
    size_t width = this->config_.width();

    // subtract the splits
    width -= (columns.size() + 1);
    size_t indivWidth = width / unspecified;
    size_t rest = width % unspecified;

    size_t lastUnsp = 0;
    for (size_t i = 0; i < columns.size(); ++i)
    {
      auto& column = columns[i];
      if (column.config().width() != 0) continue;

      column.config().width(indivWidth + (rest > 0));
      rest -= (rest > 0);
      lastUnsp = i;
    }

    // if there's still a rest
    if (rest > 0) columns[lastUnsp].config().width(indivWidth + rest);
  }

  void adjustWidth(std::vector<Row>& rows) const
  {
    if (rows.empty()) return;

    for (Row& row : rows)
    {
      const size_t estimatedWidth = config().width();

      size_t unspecified = 0;
      const size_t rowWidth = calculateWidth(row, unspecified);

      if (rowWidth < estimatedWidth && unspecified != 0)
        setUnspecifiedWidth(row, unspecified);
      else
        setWidth(row);
    }
  }
  void configureRows(std::vector<Row>& rows) const
  {
    auto& verticalBorder = this->border_.vertical();
    for (auto& row : rows)
    {
      auto& config = row.config();

      if (config.vertical().glyph() == '\0')
        config.vertical(verticalBorder);
    }
  }

  static std::vector<size_t> connections(const Row& row)
  {
    std::vector<size_t> connections;
    connections.reserve(row.columns().size());

    size_t track = 1;
    for (const Column& column : row.columns())
    {
      track += column.config().width();
      connections.push_back(track++);
    }

    return connections;
  }
  std::string getBorderHeader(std::vector<Row>& rows) const
  {
    const auto& columns = rows[0].columns();

    std::string header = this->border_.cornerTopLeft().str();
    header.reserve(this->config_.width());

    const std::string& horizontal = this->border_.horizontal().str();
    for (size_t i = 0; i < columns.size(); ++i)
    {
      size_t width = columns[i].config().width();

      for (size_t j = 0; j < width; ++j)
        header += horizontal;

      if (i + 1 < columns.size()) header += this->border_.connectorTop().str();
    }

    header += this->border_.cornerTopRight().str();
    return header;
  }
  std::string getBorderFooter(std::vector<Row>& rows) const
  {
    const auto& columns = rows.back().columns();

    std::string footer = this->border_.cornerBottomLeft().str();
    footer.reserve(this->config_.width());

    const std::string& horizontal = this->border_.horizontal().str();
    for (size_t i = 0; i < columns.size(); ++i)
    {
      size_t width = columns[i].config().width();

      for (size_t j = 0; j < width; ++j)
        footer += horizontal;

      if (i + 1 < columns.size()) footer += this->border_.connectorBottom().str();
    }

    footer += this->border_.cornerBottomRight().str();
    return footer;
  }
  std::string getBorderMiddle(std::vector<Row>& rows, size_t index) const
  {
    const auto nextRowConnections = connections(rows[index + 1]);

    std::string middle = this->border_.connectorLeft().str();
    middle.reserve(this->config_.width());

    const auto& columns = rows[index].columns();

    size_t tracker = 0;
    const std::string& horizontal = this->border_.horizontal().str();
    for (size_t i = 0; i < columns.size(); ++i)
    {
      size_t width = columns[i].config().width();

      for (size_t j = 0; j < width; ++j)
      {
        if (detail::bisearch(nextRowConnections, ++tracker))
          middle += this->border_.connectorTop().str();
        else
          middle += horizontal;
      }
      tracker++;

      if (i + 1 >= columns.size()) continue;

      if (detail::bisearch(nextRowConnections, tracker))
        middle += this->border_.intersection().str();
      else
        middle += this->border_.connectorBottom().str();
    }

    middle += this->border_.connectorRight().str();
    return middle;
  }
};

#if defined(_WIN32) || defined(_WIN64)
#include "windows.h"
#include <io.h>
#endif

inline void render(const std::string& str, FILE* out)
{
  if (str.empty())
    return;

#if defined(_WIN32) || defined(_WIN64)
  HANDLE handle;

  if (out == stdout)
    handle = GetStdHandle(STD_OUTPUT_HANDLE);
  else if (out == stderr)
    handle = GetStdHandle(STD_ERROR_HANDLE);
  else
    handle = reinterpret_cast<HANDLE>(_get_osfhandle(_fileno(out)));

  if (handle == INVALID_HANDLE_VALUE)
    return;

  DWORD mode;
  DWORD written;

  if (!GetConsoleMode(handle, &mode))
  {
    WriteFile(handle, str.c_str(), str.length(), &written, nullptr);
    return;
  }

  // convert UTF-8 to UTF-16 because it's Windows
  int wlen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);

  // fallback
  if (wlen <= 0)
  {
    WriteFile(handle, str.c_str(), str.length(), &written, nullptr);
    return;
  }

  wchar_t* buffer = new wchar_t[wlen];
  int result = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, buffer, wlen);

  if (result <= 0)
  {
    delete[] buffer;
    WriteFile(handle, str.c_str(), str.length(), &written, nullptr);
    return;
  }

  WriteConsoleW(handle, buffer, static_cast<DWORD>(wlen - 1), &written, nullptr);
  delete[] buffer;
#else
  fwrite(str.c_str(), 1, str.length(), out);
#endif
}
}

/*
  ------------------------------------------------------------------------------
  MIT License
  ------------------------------------------------------------------------------
  Copyright (c) 2025 Anas
  ------------------------------------------------------------------------------
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  ------------------------------------------------------------------------------
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
  ------------------------------------------------------------------------------
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
  ------------------------------------------------------------------------------
*/
