#pragma once


#include <cstdint>
#include <string>
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
      struct rgb { // to avoid collapsing with windows.h RGB macro
        uint8_t r, g, b;
        rgb(uint8_t r, uint8_t g, uint8_t b);
      };

      Color();

      Color(Builtin color);
      Color(rgb rgb);

      Color(const Color& other);
      Color& operator=(const Color& other);

      void setColor(Builtin color);
      void setColor(rgb rgb);

      Builtin getBuiltin() const;
      rgb getRGB() const;
      int getIndex() const;

      std::vector<uint8_t> getCode() const;
      std::vector<uint8_t> getBackCode() const;

    private:
      // using `std::variant` will break c++11 support
      enum class Knd : int { Builtin = 0, RGB } dataKnd;
      union Data {
        Builtin color;
        rgb rgb;

        Data() {}
        ~Data() {}
      } data;
  };
}
