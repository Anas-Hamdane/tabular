// move utf8conv and display width stuff here
#ifndef TABULAR_CODEC_HPP
#define TABULAR_CODEC_HPP

#include <cstddef>
#include <cstdint>
#include <string>

namespace tabular {
  namespace detail {
    namespace codec {

      struct interval {
        uint32_t first;
        uint32_t last;
      };

      // clang-format off
    static constexpr interval wide[] = {
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
      // clang-format on

      static constexpr size_t wide_size = sizeof(wide) / sizeof(interval);

      static constexpr unsigned char utf8_len[256] = {
          1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
          1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
          1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
          1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
          1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
          1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
          1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
          1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
          2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
          2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
          3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
          4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

      // binary search in `wide` array of `interval`s
      inline bool wide_bisearch(uint32_t cp) noexcept {
        if (cp < wide[0].first || cp > wide[wide_size - 1].last) {
          return false;
        }

        size_t lo = 0, hi = wide_size - 1;

        while (lo <= hi) {
          const size_t mid = lo + ((hi - lo) >> 1);
          const interval& range = wide[mid];

          if (cp < range.first)
            hi = mid - 1;

          else if (cp > range.last)
            lo = mid + 1;

          else
            return true;
        }

        return false;
      }

      /* Decode UTF-8 to code point */
      inline bool utf8_decode(const char* s, uint32_t& codepoint, int& consumed) {
        const unsigned char* u = reinterpret_cast<const unsigned char*>(s);
        unsigned char c = u[0];
        consumed = utf8_len[c];
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

      inline size_t wcwidth(uint32_t cp) noexcept {
        if (cp == 0) return 0;

        // custom check for tabular project
        if (cp == 0x1B) return 1;

        return wide_bisearch(cp) ? 2 : 1;
      }

      inline size_t utf8dw(const char* s) noexcept {
        size_t width = 0;
        const unsigned char* p = reinterpret_cast<const unsigned char*>(s);

        while (*p) {
          const unsigned char c = static_cast<unsigned char>(*p);

          // ASCII chars
          if (c < 0x80) {
            width += (c >= 0x20) ? 1 : (c == 0x1B ? 1 : (c == 0 ? 0 : 1));
            ++p;
            continue;
          }

          // Multi-byte UTF-8 sequence
          const unsigned char len = utf8_len[*p];
          if (len == 0) {
            // Invalid UTF-8 byte, treated as width 1
            ++width;
            ++p;
            continue;
          }

          uint32_t cp;
          int consumed;
          if (utf8_decode(reinterpret_cast<const char*>(p), cp, consumed)) {
            width += wcwidth(cp);
            p += consumed;
          }

          // Invalid UTF-8 byte, treated as width 1
          else {
            ++width;
            ++p;
          }
        }

        return width;
      }

      /*
       * Read a the next UTF-8 character (which may be 1-4 bytes long)
       * starting at the specified position `pos` and copies it into
       * a provided buffer `buffer`.
       *
       * @param str: The UTF-8 encoded string to extract from.
       * @param buffer: Output buffer to store the character sequence.
       * @param pos: The position where to start extraction.
       * @param len: Reference parameter for the length
       *             of th extracted `buffer` in bytes.
       *
       * @return true: when a valid UTF-8 sequence was extracted, otherwise
       *               it returns false.
       *
       */
      inline bool next_utf8_sequence(const std::string& str, char buffer[5], size_t pos,
                                     size_t& len) {
        const unsigned char first_byte = static_cast<unsigned char>(str[pos]);

        // the first byte MUST indicate the start of a UTF-8 sequence
        if ((first_byte & 0xC0) == 0x80 || pos >= str.length()) {
          len = 0;
          buffer[0] = '\0';
          return false;
        }

        len = utf8_len[first_byte];

        // Invalid UTF-8
        if (len == 0) {
          len = 0;
          buffer[0] = str[pos];
          buffer[1] = '\0';
          return false;
        }

        if (pos + len > str.length())
          len = str.length() - pos;

        // Copy bytes
        switch (len) {
          case 4:
            buffer[3] = str[pos + 3];
          case 3:
            buffer[2] = str[pos + 2];
          case 2:
            buffer[1] = str[pos + 1];
          case 1:
            buffer[0] = str[pos];
            break;
          default:
            // in case
            buffer[0] = str[pos];
            len = 1;
        }

        buffer[len] = '\0';

        return true;
      }

    } // namespace codec
  } // namespace detail
} // namespace tabular

#endif // !TABULAR_CODEC_HPP
