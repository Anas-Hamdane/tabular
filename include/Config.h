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
      std::vector<Color> getColors() const;
      std::vector<Color> getBackColors() const;

      Color getBaseColor() const;
      Alignment getAlign() const;
      Padding getPadd() const;
      int getWidth() const;

      void addAttr(Attribute attr);
      void addColor(Color color);
      void addBackColor(Color back);

      void setAttrs(std::vector<Attribute> attrs);
      void setColors(std::vector<Color> colors);
      void setBackColors(std::vector<Color> back);

      void setBaseColor(Color base);
      void setAlign(Alignment alignment);
      void setPadd(Padding padd);
      void setWidth(int width);

    private:
      std::vector<Attribute> attrs;
      std::vector<Color> colors;
      std::vector<Color> back;
      Color base;
      Alignment alignment;
      Padding padding;
      int width = -1;
  };
}
