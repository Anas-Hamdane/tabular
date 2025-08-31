#pragma once

#include <cstdint>
#include "Attribute.h"
#include "Color.h"

namespace tabular {
  enum class Alignment { Left, Right, Center };
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

      void addAttr(Attribute attr);
      void setAttrs(std::vector<Attribute> attrs);

      void setFG(Color colors);
      void setBG(Color back);
      void setBase(Color base);

      void setAlign(Alignment alignment);
      void setPadd(Padding padd);
      void setWidth(int width);

    private:
      std::vector<Attribute> attrs;

      Color fg;
      Color bg;
      Color base;

      Alignment alignment;
      Padding padding;
      int width = -1;
  };
}
