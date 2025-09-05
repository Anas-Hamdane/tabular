#pragma once

#include <cstdint>
#include "string.h"

namespace tabular {
  enum class Alignment { Left, Right, Center };
  struct Padding {
    uint8_t top = 0;
    uint8_t bottom = 0;

    constexpr Padding() = default;
    constexpr Padding(uint8_t padd) : top(padd), bottom(padd) {}
    constexpr Padding(uint8_t top, uint8_t bottom) : top(top), bottom(bottom) {}
  };
  class Config {
    public:
      constexpr Config() = default;

      Alignment align() const { return this->align_; }
      Padding padd() const { return this->padd_; }
      size_t width() const { return this->width_; }
      String delimiter() const { return this->delimiter_; }

      Config& align(Alignment alignment)
      {
        this->align_ = alignment;
        return *this;
      }
      Config& padd(Padding padd)
      {
        this->padd_ = padd;
        return *this;
      }
      Config& width(size_t width)
      {
        this->width_ = width;
        return *this;
      }
      Config& delimiter(String delimiter)
      {
        this->delimiter_ = delimiter;
        return *this;
      }

    private:
      Alignment align_ = Alignment::Left;
      Padding padd_ = Padding(0);
      String delimiter_ = String("-");
      size_t width_ = 0;
  };
}
