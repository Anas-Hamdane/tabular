#include "../include/Color.h"

namespace tabular {
  Color::RGB::RGB(uint8_t r, uint8_t g, uint8_t b)
    : r(r), g(g), b(b) {}

  Color::Color()
  {
    this->dataKnd = Knd::Builtin;
    this->optKnd = OptKnd::Global;
    this->data.color = Builtin::Normal;
  }
  Color::~Color()
  {
    destroyOptions();
  }

  Color::Color(Builtin color)
  {
    this->data.color = color;
    this->dataKnd = Knd::Builtin;
    this->optKnd = OptKnd::Global;
  }
  Color::Color(RGB rgb)
  {
    this->data.rgb = rgb;
    this->dataKnd = Knd::RGB;
    this->optKnd = OptKnd::Global;
  }

  Color::Color(Builtin color, std::pair<size_t, size_t> range)
  {
    this->data.color = color;
    this->dataKnd = Knd::Builtin;

    new(&opt.range) std::pair<size_t, size_t>(std::move(range));
    this->optKnd = OptKnd::Range;
  }
  Color::Color(RGB rgb, std::pair<size_t, size_t> range)
  {
    this->data.rgb = rgb;
    this->dataKnd = Knd::RGB;

    new(&opt.range) std::pair<size_t, size_t>(std::move(range));
    this->optKnd = OptKnd::Range;
  }

  Color::Color(Builtin color, std::string target)
  {
    this->data.color = color;
    this->dataKnd = Knd::Builtin;

    new(&opt.target) std::string(std::move(target));
    this->optKnd = OptKnd::Target;
  }
  Color::Color(RGB rgb, std::string target)
  {
    this->data.rgb = rgb;
    this->dataKnd = Knd::RGB;

    new(&opt.target) std::string(std::move(target));
    this->optKnd = OptKnd::Target;
  }

  Color::Color(const Color& other)
  {
    this->optKnd = other.optKnd;
    switch (optKnd)
    {
      case OptKnd::Range:
        this->opt.range = other.opt.range;
        break;
      case OptKnd::Target:
        this->opt.target = other.opt.target;
        break;
      default:
        break;
    };

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

    destroyOptions();

    this->optKnd = other.optKnd;
    switch (optKnd)
    {
      case OptKnd::Range:
        this->opt.range = other.opt.range;
        break;
      case OptKnd::Target:
        this->opt.target = other.opt.target;
        break;
      default:
        break;
    };

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
  void Color::setColor(RGB rgb)
  {
    this->data.rgb = rgb;
    this->dataKnd = Knd::RGB;
  }

  void Color::setRange(std::pair<size_t, size_t> range)
  {
    destroyOptions();
    this->opt.range = std::move(range);
    this->optKnd = OptKnd::Range;
  }
  void Color::setTarget(std::string target)
  {
    destroyOptions();
    this->opt.target = std::move(target);
    this->optKnd = OptKnd::Target;
  }

  Color::Builtin Color::getBuiltin() const
  {
    return this->data.color;
  }
  Color::RGB Color::getRGB() const
  {
    return this->data.rgb;
  }

  std::pair<size_t, size_t>& Color::getRange()
  {
    return this->opt.range;
  }
  std::string& Color::getTarget()
  {
    return this->opt.target;
  }

  const std::pair<size_t, size_t>& Color::getRange() const
  {
    return this->opt.range;
  }
  const std::string& Color::getTarget() const
  {
    return this->opt.target;
  }

  std::string Color::toString(size_t offset) const
  {
    std::string format = "\033[";

    switch (this->dataKnd)
    {
      case Knd::Builtin:
        format += std::to_string(static_cast<int>(this->data.color) + offset);
        break;
      case Knd::RGB:
        format += std::to_string(38 + offset) + ";2;";
        format += std::to_string(this->data.rgb.r) + ";";
        format += std::to_string(this->data.rgb.g) + ";";
        format += std::to_string(this->data.rgb.b);
        break;
    }

    format += 'm';
    return format;
  }

  void Color::destroyOptions()
  {
    switch (this->optKnd)
    {
      case OptKnd::Target:
        this->opt.target.~basic_string();
        break;
      case OptKnd::Range:
        this->opt.range.~pair();
        break;
      default:
        break;
    }
  }
}
