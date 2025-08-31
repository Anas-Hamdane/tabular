#include "../include/Attribute.h"

namespace tabular {
  Attribute::Attribute() = default;
  Attribute::Attribute(Knd attr)
  {
    this->attrs.push_back(attr);
  }
  Attribute::Attribute(std::vector<Knd> attrs)
    : attrs(std::move(attrs)) {}

  void Attribute::setAttrs(std::vector<Knd> attrs)
  {
    this->attrs = std::move(attrs);
  }

  std::vector<Attribute::Knd> Attribute::getAttrs() const
  {
    return this->attrs;
  }
  std::vector<uint8_t> Attribute::getCode() const
  {
    std::vector<uint8_t> result;
    result.reserve(this->attrs.size());

    for (const auto& attr : attrs)
      result.push_back(static_cast<uint8_t>(attr));

    return result;
  }
}
