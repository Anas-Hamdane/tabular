#pragma once

#include <string>
#include "color.h"
#include "global.h"

namespace tabular {
// clang-format off
class Border {
public:
  class Part {
  public:
    Part(const uint32_t glyph)
      : glyph_(glyph), dirty_(true)
    {
    }

    void makeDirty() const { this->dirty_ = true; }
    void makeClean() const { this->dirty_ = false; }

    Part& glyph(const uint32_t glyph)
    {
      this->glyph_ = glyph;
      makeDirty();
      return *this;
    }

    Part& fg(const Color color)
    {
      this->fg_ = static_cast<uint32_t>(color);
      makeDirty();
      return *this;
    }
    Part& fg(const Rgb rgb)
    {
      this->fg_ = rgb.toHex() | (1u << 24);
      makeDirty();
      return *this;
    }

    Part& bg(const Color color)
    {
      this->bg_ = static_cast<uint32_t>(color);
      makeDirty();
      return *this;
    }
    Part& bg(const Rgb rgb)
    {
      this->bg_ = rgb.toHex() | (1u << 24);
      makeDirty();
      return *this;
    }

    uint32_t fg() const
    {
      return this->fg_;
    }
    uint32_t bg() const
    {
      return this->bg_;
    }
    uint32_t glyph() const
    {
      return this->glyph_;
    }

    Part& clrFg()
    {
      this->fg_ = 0;
      makeDirty();
      return *this;
    }
    Part& clrBg()
    {
      this->bg_ = 0;
      makeDirty();
      return *this;
    }
    Part& clr()
    {
      this->fg_ = 0;
      this->bg_ = 0;
      makeDirty();
      return *this;
    }

    const std::string& toStr() const
    {
      if (this->dirty_)
      {
        this->str = reGenStr();
        makeClean();
      }

      return this->str;
    }
    std::string reGenStr() const
    {
      using namespace detail;
      std::string buffer;

      ColorType fg = this->fg_;
      ColorType bg = this->bg_;

      if (fg.isColor())
      {
        buffer.append("\x1b[");
        buffer.append(std::to_string(static_cast<uint8_t>(fg.color())));
        buffer.push_back(';');
      }
      else if (fg.isRgb())
      {
        Rgb rgb = fg.rgb();
        buffer.append("\x1b[38;2;");

        buffer.append(std::to_string(rgb.r));
        buffer.push_back(';');
        buffer.append(std::to_string(rgb.g));
        buffer.push_back(';');
        buffer.append(std::to_string(rgb.b));
        buffer.push_back(';');
      }

      if (bg.isColor())
      {
        if (buffer.empty()) buffer.append("\x1b[");
        buffer.append(std::to_string(static_cast<uint8_t>(bg.color()) + 10));
        buffer.push_back(';');
      }
      else if (bg.isRgb())
      {
        Rgb rgb = bg.rgb();
        if (buffer.empty()) buffer.append("\x1b[48;2;");
        buffer.append(std::to_string(rgb.r));
        buffer.push_back(';');
        buffer.append(std::to_string(rgb.g));
        buffer.push_back(';');
        buffer.append(std::to_string(rgb.b));
        buffer.push_back(';');
      }

      const std::string glyph = glyphToStr();
      buffer.append(glyph);

      if (buffer.length() - glyph.length() > 0)
        buffer.append(RESET_ESC);

      return buffer;
    }

    operator const std::string&() const
    {
      return this->toStr();
    }

  private:
    uint32_t glyph_ = 0; // Unicode code point
    uint32_t fg_ = 0; // fg color
    uint32_t bg_ = 0; // bg color

    // cache
    mutable bool dirty_ = false;
    mutable std::string str;

    std::string glyphToStr() const
    {
      std::string result;

      if (this->glyph_ <= 0x7F)
      {
        result += static_cast<char>(this->glyph_);
      }

      else if (this->glyph_ <= 0x7FF)
      {
        result += static_cast<char>(0xC0 | ((this->glyph_ >> 6) & 0x1F));
        result += static_cast<char>(0x80 | (this->glyph_ & 0x3F));
      }

      else if (this->glyph_ <= 0xFFFF)
      {
        result += static_cast<char>(0xE0 | ((this->glyph_ >> 12) & 0x0F));
        result += static_cast<char>(0x80 | ((this->glyph_ >> 6) & 0x3F));
        result += static_cast<char>(0x80 | (this->glyph_ & 0x3F));
      }

      else
      {
        result += static_cast<char>(0xF0 | ((this->glyph_ >> 18) & 0x07));
        result += static_cast<char>(0x80 | ((this->glyph_ >> 12) & 0x3F));
        result += static_cast<char>(0x80 | ((this->glyph_ >> 6) & 0x3F));
        result += static_cast<char>(0x80 | (this->glyph_ & 0x3F));
      }

      return result;
    }
  };


  Part& horizontal() { return this->horizontal_; }
  Part& vertical() { return this->vertical_; }
  Part& cornerTopLeft() { return this->cornerTopLeft_; }
  Part& cornerTopRight() { return this->cornerTopRight_; }
  Part& cornerBottomLeft() { return this->cornerBottomLeft_; }
  Part& cornerBottomRight() { return this->cornerBottomRight_; }
  Part& intersection() { return this->intersection_; }
  Part& connectorLeft() { return this->connectorLeft_; }
  Part& connectorRight() { return this->connectorRight_; }
  Part& connectorTop() { return this->connectorTop_; }
  Part& connectorBottom() { return this->connectorBottom_; }

  const Part& horizontal() const { return this->horizontal_; }
  const Part& vertical() const { return this->vertical_; }
  const Part& cornerTopLeft() const { return this->cornerTopLeft_; }
  const Part& cornerTopRight() const { return this->cornerTopRight_; }
  const Part& cornerBottomLeft() const { return this->cornerBottomLeft_; }
  const Part& cornerBottomRight() const { return this->cornerBottomRight_; }
  const Part& intersection() const { return this->intersection_; }
  const Part& connectorLeft() const { return this->connectorLeft_; }
  const Part& connectorRight() const { return this->connectorRight_; }
  const Part& connectorTop() const { return this->connectorTop_; }
  const Part& connectorBottom() const { return this->connectorBottom_; }

  Border& horizontal(const uint32_t glyph)
  {
    this->horizontal_.glyph(glyph);
    return *this;
  }
  Border& vertical(const uint32_t glyph)
  {
    this->vertical_.glyph(glyph);
    return *this;
  }
  Border& cornerTopLeft(const uint32_t glyph)
  {
    this->cornerTopLeft_.glyph(glyph);
    return *this;
  }
  Border& cornerTopRight(const uint32_t glyph)
  {
    this->cornerTopRight_.glyph(glyph);
    return *this;
  }
  Border& cornerBottomLeft(const uint32_t glyph)
  {
    this->cornerBottomLeft_.glyph(glyph);
    return *this;
  }
  Border& cornerBottomRight(const uint32_t glyph)
  {
    this->cornerBottomRight_.glyph(glyph);
    return *this;
  }
  Border& intersection(const uint32_t glyph)
  {
    this->intersection_.glyph(glyph);
    return *this;
  }
  Border& connectorLeft(const uint32_t glyph)
  {
    this->connectorLeft_.glyph(glyph);
    return *this;
  }
  Border& connectorRight(const uint32_t glyph)
  {
    this->connectorRight_.glyph(glyph);
    return *this;
  }
  Border& connectorTop(const uint32_t glyph)
  {
    this->connectorTop_.glyph(glyph);
    return *this;
  }
  Border& connectorBottom(const uint32_t glyph)
  {
    this->connectorBottom_.glyph(glyph);
    return *this;
  }

  static Border Default()
  {
    // already default constructed
    return {};
  }
  static Border Filled(const uint32_t c)
  {
    Border border;
    border.horizontal_ = c;
    border.vertical_ = c;

    border.cornerTopLeft_ = c;
    border.cornerTopRight_ = c;
    border.cornerBottomLeft_ = c;
    border.cornerBottomRight_ = c;

    border.intersection_ = c;

    border.connectorLeft_ = c;
    border.connectorRight_ = c;
    border.connectorTop_ = c;
    border.connectorBottom_ = c;

    return border;
  }
  static Border None()
  {
    return Filled(U'\0');
  }
  static Border Blank()
  {
    return Filled(U' ');
  }
  static Border Modern()
  {
    Border border;
    border.horizontal_ = U'─';
    border.vertical_ = U'│';

    border.cornerTopLeft_ = U'┌';
    border.cornerTopRight_ = U'┐';
    border.cornerBottomLeft_ = U'└';
    border.cornerBottomRight_ = U'┘';

    border.intersection_ = U'┼';

    border.connectorLeft_ = U'├';
    border.connectorRight_ = U'┤';
    border.connectorTop_ = U'┬';
    border.connectorBottom_ = U'┴';

    return border;
  }


private:
  Part horizontal_ = U'-'; // ─
  Part vertical_ = U'|'; // │

  Part cornerTopLeft_ = U'+'; // ┌
  Part cornerTopRight_ = U'+'; // ┐
  Part cornerBottomLeft_ = U'+'; // └
  Part cornerBottomRight_ = U'+'; // ┘

  Part intersection_ = U'+'; // ┼

  Part connectorLeft_ = U'+'; // ├
  Part connectorRight_ = U'+'; // ┤
  Part connectorTop_ = U'-'; // ┬
  Part connectorBottom_ = U'-'; // ┴
};
// clang-format on
}