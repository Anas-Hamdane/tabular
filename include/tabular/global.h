#pragma once

#include <cstdint>

namespace tabular {
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
      : r((hex >> 16) & 0xFF), g((hex >> 8) & 0xFF), b(hex & 0xFF) {}
  constexpr uint32_t toHex() const
  {
    return (static_cast<uint32_t>(r) << 16) | (static_cast<uint32_t>(g) << 8) |
           (static_cast<uint32_t>(b));
  }
};

enum class Alignment { Left, Right, Center };
struct Padding {
  uint8_t top = 0;
  uint8_t bottom = 0;
  uint8_t left = 1;
  uint8_t right = 1;

  constexpr Padding() = default;
  constexpr Padding(uint8_t vertical, uint8_t horizontal)
    : top(vertical), bottom(vertical), left(horizontal), right(horizontal) {}
  constexpr Padding(uint8_t top, uint8_t bottom, uint8_t left, uint8_t right)
      : top(top), bottom(bottom), left(left), right(right) {}
};

constexpr auto RESET_ESC = "\x1b[0m";
constexpr uint8_t WORD_LENGTH_AVERAGE = 5;
} // namespace tabular
