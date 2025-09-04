#pragma once

#include <cstdint>

namespace tabular {
  enum class Attribute : uint16_t {
    Bold = 1,
    Dim = 1 << 1,
    Italic = 1 << 2,

    Underline = 1 << 3,
    Dunderline= 1 << 4, // not popular

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
    uint8_t r, g, b;
    constexpr Rgb() : r(0), g(0), b(0) {}
    constexpr Rgb(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}
    constexpr Rgb(uint32_t hex)
      : r((hex >> 16) & 0xFF), g((hex >> 8) & 0xFF), b(hex & 0xFF) {}
    constexpr uint32_t toHex() const
    {
      return (static_cast<uint32_t>(r) << 16) |
             (static_cast<uint32_t>(g) <<  8) |
             (static_cast<uint32_t>(b));
    }
  };
  class ColorType {
    public:
      constexpr ColorType() = default;
      constexpr ColorType(Color color)
        : color(static_cast<uint32_t>(color)) {}
      constexpr ColorType(Rgb rgb)
        : color(rgb.toHex() | (1u << 24)) {}
      constexpr ColorType(uint32_t value)
        : color(value) {}

      constexpr bool isSet() const { return color != 0; }
      constexpr bool isColor() const { return isSet() && (color & (1u << 24)) == 0; }
      constexpr bool isRgb() const { return isSet() && (color & (1u << 24)) != 0; }

      constexpr Color getColor() const { return static_cast<Color>(color & 0xFFFFFF); }
      constexpr Rgb getRgb() const { return Rgb(color & 0xFFFFFF); }

      void setColor(Color color) { this->color = static_cast<uint32_t>(color); }
      void setRgb(const Rgb& rgb) { color = rgb.toHex() | (1u << 24); }
      void reset() { color = 0; }

    private:
      /*
       * bits 0-23   : Reserved for the color value itself (24-bit color)
       * bit 24      : Type flag (0: Color, 1 Rgb)
       */
      uint32_t color = 0;
  };
  class ColStyle {
    public:
      constexpr ColStyle() = default;
      constexpr ColStyle(Attribute attr)
        : attrs_(static_cast<uint16_t>(attr)) {}

      ColStyle& operator|=(Attribute attr)
      {
        this->attrs_ |= static_cast<uint16_t>(attr);
        return *this;
      }
      ColStyle operator|(Attribute attr) const
      {
        ColStyle result = *this;
        return result |= attr;
      }

      ColStyle& fg(Color color)
      {
        this->fg_ = static_cast<uint32_t>(color);
        return *this;
      }
      ColStyle& fg(Rgb rgb)
      {
        this->fg_ = rgb.toHex() | (1u << 24);
        return *this;
      }
      ColStyle& bg(Color color)
      {
        this->bg_ = static_cast<uint32_t>(color);
        return *this;
      }
      ColStyle& bg(Rgb rgb)
      {
        this->bg_ = rgb.toHex() | (1u << 24);
        return *this;
      }
      ColStyle& base(Color color)
      {
        this->base_ = static_cast<uint32_t>(color);
        return *this;
      }
      ColStyle& base(Rgb rgb)
      {
        this->base_ = rgb.toHex() | (1u << 24);
        return *this;
      }
      ColStyle& attrs(Attribute attr)
      {
        this->attrs_ |= static_cast<uint16_t>(attr);
        return *this;
      }

      constexpr bool hasFg() const { return this->fg_ != 0; }
      constexpr bool hasBg() const { return this->bg_ != 0; }
      constexpr bool hasBase() const { return this->base_ != 0; }
      constexpr bool hasAttrs() const { return this->attrs_ != 0; }

      constexpr ColorType getFg() const { return this->fg_; }
      constexpr ColorType getBg() const { return this->bg_; }
      constexpr ColorType getBase() const { return this->base_; }
      constexpr Attribute getAttrs() const { return static_cast<Attribute>(attrs_); }

      void resetFg() { this->fg_ = 0; }
      void resetBg() { this->bg_ = 0; }
      void resetBase() { this->base_ = 0; }
      void resetAttrs() { this->attrs_ = 0; }
      void reset() 
      {
        this->fg_ = 0;
        this->bg_ = 0;
        this->base_ = 0;
        this->attrs_ = 0;
      }

    private:
      // representing all the colors
      uint32_t fg_ = 0;
      uint32_t bg_ = 0;
      uint32_t base_ = 0;

      // all the attributes
      uint16_t attrs_ = 0;
  };
  inline ColStyle operator|(Attribute lhs, Attribute rhs)
  {
    return ColStyle(lhs) | rhs;
  }
}
