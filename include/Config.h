#pragma once

#include <cstdint>
#include "Color.h"
#include "String.h"

namespace tabular {
  enum class Alignment { Left, Right, Center };
  enum class Attribute : uint8_t {
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
  struct Padding {
    uint8_t top, bottom;
    Padding();
    Padding(uint8_t padd);
    Padding(uint8_t top, uint8_t bottom);
  };
  class Config {
    public:
      Config();

      std::vector<Attribute> getAttrs() const;

      Color getFG() const;
      Color getBG() const;
      Color getBase() const;

      Alignment getAlign() const;
      Padding getPadd() const;
      int getWidth() const;
      String getDelimiter() const;

      void addAttr(Attribute attr);
      void setAttrs(std::vector<Attribute> attrs);

      void setFG(Color colors);
      void setBG(Color back);
      void setBase(Color base);

      void setAlign(Alignment alignment);
      void setPadd(Padding padd);
      void setWidth(int width);

      void setDelimiter(String delimiter);

    private:
      std::vector<Attribute> attrs;

      Color fg;
      Color bg;
      Color base;

      Alignment alignment;
      Padding padding;
      int width = -1;

      String delimiter;
  };
}
