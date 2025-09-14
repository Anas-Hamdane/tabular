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
} // namespace tabular
