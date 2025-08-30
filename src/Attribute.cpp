#include "../include/Attribute.h"

namespace tabular {
  Attribute::Attribute() = default;
  Attribute::Attribute(Knd attr)
  {
    this->attrs.push_back(attr);
    this->opt = Option::Global;
  }
  Attribute::Attribute(std::vector<Knd> attrs)
    : attrs(std::move(attrs)), opt(Option::Global) {}

  Attribute::Attribute(Knd attr, std::pair<size_t, size_t> range)
  {
    this->attrs.push_back(attr);
    new(&this->data.range) std::pair<size_t, size_t>(std::move(range));
    this->opt = Option::Range;
  }
  Attribute::Attribute(std::vector<Knd> attrs, std::pair<size_t, size_t> range)
    : attrs(std::move(attrs))
  {
    new(&this->data.range) std::pair<size_t, size_t>(std::move(range));
    this->opt = Option::Range;
  }

  Attribute::Attribute(Knd attr, std::string target)
  {
    this->attrs.push_back(attr);
    new(&this->data.target) std::string(std::move(target));
    this->opt = Option::String;
  }
  Attribute::Attribute(std::vector<Knd> attrs, std::string target)
    : attrs(std::move(attrs))
  {
    new(&this->data.target) std::string(std::move(target));
    this->opt = Option::String;
  }

  Attribute::~Attribute()
  {
    destroyUnion();
  }

  void Attribute::setAttrs(std::vector<Knd> attrs)
  {
    this->attrs = std::move(attrs);
  }
  void Attribute::setRange(std::pair<size_t, size_t> range)
  {
    destroyUnion();
    new(&this->data.range) std::pair<size_t, size_t>(std::move(range));
    this->opt = Option::Range;
  }
  void Attribute::setTarget(std::string target)
  {
    destroyUnion();
    new(&this->data.target) std::string(std::move(target));
  }

  std::vector<Attribute::Knd> Attribute::getAttrs()
  {
    return this->attrs;
  }
  std::pair<size_t, size_t> Attribute::getRange()
  {
    return this->data.range;
  }
  std::string Attribute::getTarget()
  {
    return this->data.target;
  }

  void Attribute::destroyUnion()
  {
    switch (this->opt)
    {
      case Option::Range:
        this->data.range.~pair();
        break;
      case Option::String:
        this->data.target.~basic_string();
        break;
      default:
        break;
    }
  }
}
