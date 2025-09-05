#pragma once

#include "global.h"
#include <cstdint>

namespace tabular {

class Style {
public:
  constexpr Style() = default;
  constexpr Style(Attribute attr) : attrs_(static_cast<uint16_t>(attr)) {}

  Style &operator|=(Attribute attr)
  {
    this->attrs_ |= static_cast<uint16_t>(attr);
    return *this;
  }
  Style operator|(Attribute attr) const
  {
    Style result = *this;
    return result |= attr;
  }

  Style &fg(Color color)
  {
    this->fg_ = static_cast<uint32_t>(color);
    return *this;
  }
  Style &fg(Rgb rgb)
  {
    this->fg_ = rgb.toHex() | (1u << 24);
    return *this;
  }
  Style &bg(Color color)
  {
    this->bg_ = static_cast<uint32_t>(color);
    return *this;
  }
  Style &bg(Rgb rgb)
  {
    this->bg_ = rgb.toHex() | (1u << 24);
    return *this;
  }
  Style &base(Color color)
  {
    this->base_ = static_cast<uint32_t>(color);
    return *this;
  }
  Style &base(Rgb rgb)
  {
    this->base_ = rgb.toHex() | (1u << 24);
    return *this;
  }
  Style &attrs(Attribute attr)
  {
    this->attrs_ |= static_cast<uint16_t>(attr);
    return *this;
  }

  constexpr bool hasFg() const { return this->fg_ != 0; }
  constexpr bool hasBg() const { return this->bg_ != 0; }
  constexpr bool hasBase() const { return this->base_ != 0; }
  constexpr bool hasAttrs() const { return this->attrs_ != 0; }

  constexpr uint32_t getFg() const { return this->fg_; }
  constexpr uint32_t getBg() const { return this->bg_; }
  constexpr uint32_t getBase() const { return this->base_; }
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

inline Style operator|(Attribute lhs, Attribute rhs) { return Style(lhs) | rhs; }
} // namespace tabular
