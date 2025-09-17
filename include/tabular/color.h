#pragma once

#include <cstdint>
namespace tabular {
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

    bool isSet() { return value_ != 0; }
    bool isColor() { return isSet() && (value_ & (1u << 24)) == 0; }
    bool isRgb() { return isSet() && (value_ & (1u << 24)) != 0; }

    Color color() { return static_cast<Color>(value_ & 0xFFFFFF); }
    Rgb rgb() { return {value_ & 0xFFFFFF}; }

    private:
    uint32_t value_;
  };
}
} // namespace tabular
