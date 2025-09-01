#pragma once

#include <cstdint>
#include <vector>

namespace tabular {
  class Color {
    public:
      enum class Builtin : uint8_t {
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

        Normal = 39,
      };
      struct Rgb { // to avoid collapsing with windows.h RGB macro
        uint8_t r, g, b;
        Rgb(uint8_t r, uint8_t g, uint8_t b);
      };

      Color();

      Color(Builtin color);
      Color(Rgb rgb);

      Color(const Color& other);
      Color& operator=(const Color& other);

      void setColor(Builtin color);
      void setColor(Rgb rgb);

      Builtin getBuiltin() const;
      Rgb getRGB() const;
      int getIndex() const;

      std::vector<uint8_t> getCode() const;
      std::vector<uint8_t> getBackCode() const;

    private:
      // using `std::variant` will break c++11 support
      enum class Knd : int { Builtin = 0, RGB } dataKnd;
      union Data {
        Builtin color;
        Rgb rgb;

        Data() {}
        ~Data() {}
      } data;
  };
}
