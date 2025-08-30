#pragma once

#include <cstdint>
#include <string>

namespace tabular {
  class Color {
    public:
      enum class Builtin : int {
        Black = 30,
        Red,
        Green,
        Yellow,
        Blue,
        Magenta,
        Cyan,
        White,
        Normal = 39,
      };
      struct RGB {
        uint8_t r, g, b;
        RGB(uint8_t r, uint8_t g, uint8_t b);
      };

      Color();
      ~Color();

      Color(Builtin color);
      Color(RGB rgb);

      Color(Builtin color, std::pair<size_t, size_t> range);
      Color(RGB rgb, std::pair<size_t, size_t> range);

      Color(Builtin color, std::string target);
      Color(RGB rgb, std::string target);

      Color(const Color& other);
      Color& operator=(const Color& other);

      void setColor(Builtin color);
      void setColor(RGB rgb);

      void setRange(std::pair<size_t, size_t> range);
      void setTarget(std::string target);

      Builtin getBuiltin() const;
      RGB getRGB() const;

      std::pair<size_t, size_t>& getRange();
      std::string& getTarget();

      const std::pair<size_t, size_t>& getRange() const;
      const std::string& getTarget() const;

      std::string toString(size_t offset = 0) const;

    private:
      enum class Knd { Builtin, RGB } dataKnd;
      union Data {
        Builtin color;
        RGB rgb;

        Data() {}
        ~Data() {}
      } data;

      enum class OptKnd { Global, Range, Target } optKnd;
      union Option {
        std::pair<size_t, size_t> range;
        std::string target;

        Option() {}
        ~Option() {}
      } opt;

      void destroyOptions();
  };
}
