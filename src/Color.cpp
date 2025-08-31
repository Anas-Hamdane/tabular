#include "../include/Color.h"

namespace tabular {
  Color::rgb::rgb(uint8_t r, uint8_t g, uint8_t b)
    : r(r), g(g), b(b) {}

  Color::Color()
  {
    this->dataKnd = Knd::Builtin;
    this->data.color = Builtin::Normal;
  }

  Color::Color(Builtin color)
  {
    this->data.color = color;
    this->dataKnd = Knd::Builtin;
  }
  Color::Color(rgb rgb)
  {
    this->data.rgb = rgb;
    this->dataKnd = Knd::RGB;
  }

  Color::Color(const Color& other)
  {
    this->dataKnd = other.dataKnd;
    switch (dataKnd)
    {
      case Knd::Builtin:
        this->data.color = other.data.color;
        break;
      case Knd::RGB:
        this->data.rgb = other.data.rgb;
        break;
    };
  }
  Color& Color::operator=(const Color& other)
  {
    if (this == &other)
      return *this;

    this->dataKnd = other.dataKnd;
    switch (dataKnd)
    {
      case Knd::Builtin:
        this->data.color = other.data.color;
        break;
      case Knd::RGB:
        this->data.rgb = other.data.rgb;
        break;
    };

    return *this;
  }

  void Color::setColor(Builtin color)
  {
    this->data.color = color;
    this->dataKnd = Knd::Builtin;
  }
  void Color::setColor(rgb rgb)
  {
    this->data.rgb = rgb;
    this->dataKnd = Knd::RGB;
  }

  Color::Builtin Color::getBuiltin() const
  {
    return this->data.color;
  }
  Color::rgb Color::getRGB() const
  {
    return this->data.rgb;
  }
  int Color::getIndex() const
  {
    return static_cast<int>(this->dataKnd);
  }

  std::vector<uint8_t> Color::getCode() const
  {
    std::vector<uint8_t> result;

    switch (this->dataKnd)
    {
      case Knd::Builtin:
        result.push_back(static_cast<uint8_t>(this->data.color));
        break;
      case Knd::RGB:
        result.reserve(5);
        result.push_back(38); // FG
        result.push_back(2);

        // rgb
        result.push_back(this->data.rgb.r);
        result.push_back(this->data.rgb.g);
        result.push_back(this->data.rgb.b);
        break;
    }

    return result;
  }
  std::vector<uint8_t> Color::getBackCode() const
  {
    std::vector<uint8_t> result;

    switch (this->dataKnd)
    {
      case Knd::Builtin:
        result.push_back(static_cast<uint8_t>(this->data.color) + 10);
        break;
      case Knd::RGB:
        result.reserve(5);
        result.push_back(48); // BG
        result.push_back(2);

        // rgb
        result.push_back(this->data.rgb.r);
        result.push_back(this->data.rgb.g);
        result.push_back(this->data.rgb.b);
        break;
    }

    return result;
  }
}
