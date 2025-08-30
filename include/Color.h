#pragma once

#include <cstdint>
#include <string>

namespace tabular {
  class color {
    public:
      enum class builtin : int {
        black = 30,
        red,
        green,
        yellow,
        blue,
        magenta,
        cyan,
        white,
        normal = 39,
      };
      struct rgb {
        uint8_t r, g, b;
        rgb(uint8_t r, uint8_t g, uint8_t b);
      };

      color();
      ~color();

      color(builtin color);
      color(rgb rgb);

      color(builtin color, std::pair<size_t, size_t> range);
      color(rgb rgb, std::pair<size_t, size_t> range);

      color(builtin color, std::string target);
      color(rgb rgb, std::string target);

      color(const color& other);
      color& operator=(const color& other);

      void setcolor(builtin color);
      void setcolor(rgb rgb);

      void setrange(std::pair<size_t, size_t> range);
      void settarget(std::string target);

      builtin getbuiltin();
      rgb getrgb();

      std::pair<size_t, size_t>& getrange();
      std::string& gettarget();

      const std::pair<size_t, size_t>& getrange() const;
      const std::string& gettarget() const;

    private:
      enum class knd { builtin, rgb } dataknd;
      union data {
        builtin color;
        rgb rgb;

        data() {}
        ~data() {}
      } data;

      enum class optknd { global, range, target } optknd;
      union option {
        std::pair<size_t, size_t> range;
        std::string target;

        option() {}
        ~option() {}
      } opt;

      void destroyoptions();
  };
}
