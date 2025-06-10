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

/* Lookup table for UTF-8 sequence lengths */
static const unsigned char utf8_sequence_length[256] = {
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
    0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

/* Decode UTF-8 to code point */
inline int utf8_decode(const char* s, uint32_t* codepoint, int* consumed) {
    const unsigned char* u = (const unsigned char*)s;
    unsigned char c = u[0];
    *consumed = utf8_sequence_length[c];
    if (*consumed == 0) return 0;

    switch (*consumed) {
        case 1:
            *codepoint = c;
            return 1;
        case 2:
            if ((u[1] & 0xC0) != 0x80) return 0;
            *codepoint = ((c & 0x1F) << 6) | (u[1] & 0x3F);
            return *codepoint >= 0x80;
        case 3:
            if ((u[1] & 0xC0) != 0x80 || (u[2] & 0xC0) != 0x80) return 0;
            *codepoint = ((c & 0x0F) << 12) | ((u[1] & 0x3F) << 6) | (u[2] & 0x3F);
            return *codepoint >= 0x800 && (*codepoint < 0xD800 || *codepoint > 0xDFFF);
        case 4:
            if ((u[1] & 0xC0) != 0x80 || (u[2] & 0xC0) != 0x80 || (u[3] & 0xC0) != 0x80) return 0;
            *codepoint = ((c & 0x07) << 18) | ((u[1] & 0x3F) << 12) |
                         ((u[2] & 0x3F) << 6) | (u[3] & 0x3F);
            return *codepoint >= 0x10000 && *codepoint <= 0x10FFFF;
    }
    return 0;
}

/* Encode code point to UTF-16 */
inline int utf16_encode(uint32_t codepoint, wchar_t* output) {
    if (codepoint < 0xD800 || (codepoint > 0xDFFF && codepoint <= 0xFFFF)) {
        *output = (wchar_t)codepoint;
        return 1;
    }
    if (codepoint <= 0x10FFFF) {
        codepoint -= 0x10000;
        output[0] = (wchar_t)(0xD800 | (codepoint >> 10));
        output[1] = (wchar_t)(0xDC00 | (codepoint & 0x3FF));
        return 2;
    }
    return 0;
}

/* Encode code point to UTF-32 */
inline int utf32_encode(uint32_t codepoint, wchar_t* output) {
    if (codepoint <= 0x10FFFF && (codepoint < 0xD800 || codepoint > 0xDFFF)) {
        *output = (wchar_t)codepoint;
        return 1;
    }
    return 0;
}

/* Convert UTF-8 to wchar_t string */
inline wchar_t* utf8stws(const char* input, size_t* out_len) {
    if (!input) {
        if (out_len) *out_len = 0;
        return nullptr;
    }

    const size_t len = strlen(input);
    if (len == 0) {
        wchar_t* output = (wchar_t*)malloc(sizeof(wchar_t));
        if (output) *output = L'\0';
        if (out_len) *out_len = 0;
        return output;
    }

    const size_t max_output = USE_UTF16 ? (len / 2 + 1) * 2 : len + 1;
    wchar_t* output = (wchar_t*)malloc(max_output * sizeof(wchar_t));
    if (!output) return nullptr;

    size_t out_index = 0;
    const char* p = input;
    const char* end = input + len;

    while (p < end) {
        uint32_t cp;
        int consumed;

        if (utf8_decode(p, &cp, &consumed)) {
            int written = USE_UTF16 ? utf16_encode(cp, &output[out_index]) : utf32_encode(cp, &output[out_index]);

            if (written > 0) {
                out_index += written;
                p += consumed;
                continue;
            }
        }
        output[out_index++] = 0xFFFD; // replacement character
        p++;
    }

    output[out_index] = L'\0';
    wchar_t* final_output = (wchar_t*)realloc(output, (out_index + 1) * sizeof(wchar_t));
    if (final_output) output = final_output;
    if (out_len) *out_len = out_index;
    return output;
}

#endif // !UTF8_STWS_HPP
