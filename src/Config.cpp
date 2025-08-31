#include "../include/Config.h"

namespace tabular {
  Padding::Padding() : top(0), bottom(0) {}
  Padding::Padding(uint8_t padd) : top(padd), bottom(padd) {}
  Padding::Padding(uint8_t top, uint8_t bottom) : top(top), bottom(bottom) {}
  Config::Config() : alignment(Alignment::Left) {}

  std::vector<Attribute> Config::getAttrs() const { return this->attrs; }

  Color Config::getFG() const { return this->fg; }
  Color Config::getBG() const { return this->bg; }
  Color Config::getBase() const { return this->base; }

  Alignment Config::getAlign() const { return this->alignment; }
  Padding Config::getPadd() const { return this->padding; }
  int Config::getWidth() const { return this->width; }

  void Config::addAttr(Attribute attr) { this->attrs.push_back(std::move(attr)); }
  void Config::setAttrs(std::vector<Attribute> attrs) { this->attrs = std::move(attrs); }

  void Config::setFG(Color color) { this->fg = color; }
  void Config::setBG(Color back) { this->bg = back; }
  void Config::setBase(Color base) { this->base = base; }

  void Config::setAlign(Alignment alignment) { this->alignment = alignment; }
  void Config::setPadd(Padding padding) { this->padding = padding; }
  void Config::setWidth(int width) { this->width = width; }
}
