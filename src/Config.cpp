#include "../include/Config.h"

namespace tabular {
  Padding::Padding() : top(0), bottom(0) {}
  Padding::Padding(uint8_t padd) : top(padd), bottom(padd) {}
  Padding::Padding(uint8_t top, uint8_t bottom) : top(top), bottom(bottom) {}

  Config::Config() : alignment(Alignment::Left) {}

  std::vector<Attribute> Config::getAttrs() { return this->attrs; }
  std::vector<Color> Config::getColors() { return this->colors; }
  std::vector<Color> Config::getBackColors() { return this->back; }

  Color Config::getBaseColor() { return this->base; }
  Alignment Config::getAlign() { return this->alignment; }
  Padding Config::getPadd() { return this->padding; }
  size_t Config::getWidth() { return this->width; }

  void Config::addAttr(Attribute attr) { this->attrs.push_back(std::move(attr)); }
  void Config::addColor(Color color) { this->colors.push_back(std::move(color)); }
  void Config::addBackColor(Color back) { this->back.push_back(std::move(back)); }

  void Config::setAttrs(std::vector<Attribute> attrs) { this->attrs = std::move(attrs); }
  void Config::setColors(std::vector<Color> colors) { this->colors = std::move(colors); }
  void Config::setBackColors(std::vector<Color> back) { this->back = std::move(back); }

  void Config::setBaseColor(Color base) { this->base = std::move(base); }
  void Config::setAlign(Alignment alignment) { this->alignment = alignment; }
  void Config::setPadd(Padding padding) { this->padding = padding; }
  void Config::setWidth(size_t width) { this->width = width; }
}
