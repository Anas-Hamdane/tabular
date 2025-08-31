#pragma once

#include <cstdint>
#include <vector>
#include <string>

namespace tabular {
  class Attribute {
    public:
      enum class Knd : uint8_t {
        Bold = 1,
        Dim = 2,
        Italic = 3,

        Underline = 4,
        DoubleUnderline = 21, // not popular

        Blink = 5, // rarely supported
        FastBlink = 6, // rarely supported
        Reverse = 7,
        Concealed = 8,
        Crossed = 9,
      };
      Attribute();
      Attribute(Knd attr); // for convenience
      Attribute(std::vector<Knd> attrs);

      void setAttrs(std::vector<Knd> attrs);
      std::vector<Knd> getAttrs() const;

      std::vector<uint8_t> getCode() const;

    private:
      std::vector<Knd> attrs; // the attributes
  };
}
