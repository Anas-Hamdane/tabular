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

    Part& glyph(const uint32_t glyph)
    {
      glyph_ = glyph;
      dirty_ = true;
      return *this;
    }

    Part& fg(const Color color)
    {
      fg_ = static_cast<uint32_t>(color);
      dirty_ = true;
      return *this;
    }
    Part& fg(const Rgb rgb)
    {
      fg_ = rgb.toHex() | (1u << 24);
      dirty_ = true;
      return *this;
    }

    Part& bg(const Color color)
    {
      bg_ = static_cast<uint32_t>(color);
      dirty_ = true;
      return *this;
    }
    Part& bg(const Rgb rgb)
    {
      bg_ = rgb.toHex() | (1u << 24);
      dirty_ = true;
      return *this;
    }

    uint32_t fg() const
    {
      return fg_;
    }
    uint32_t bg() const
    {
      return bg_;
    }
    uint32_t glyph() const
    {
      return glyph_;
    }

    Part& clrFg()
    {
      fg_ = 0;
      dirty_ = true;
      return *this;
    }
    Part& clrBg()
    {
      bg_ = 0;
      dirty_ = true;
      return *this;
    }
    Part& clr()
    {
      fg_ = 0;
      bg_ = 0;
      dirty_ = true;
      return *this;
    }

    explicit operator const std::string&() const { return str(); }
    const std::string& str() const
    {
      if (dirty_)
      {
        str_ = genStr();
        dirty_ = false;
      }

      return str_;
    }

  private:
    uint32_t glyph_ = 0; // Unicode code point
    uint32_t fg_ = 0; // fg color
    uint32_t bg_ = 0; // bg color

    // cache
    mutable bool dirty_ = false;
    mutable std::string str_;

    std::string genStr() const
    {
      using namespace detail;
      std::string buffer;

      ColorType fg = fg_;
      ColorType bg = bg_;

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
    std::string glyphToStr() const
    {
      std::string result;

      if (glyph_ <= 0x7F)
      {
        result += static_cast<char>(glyph_);
      }

      else if (glyph_ <= 0x7FF)
      {
        result += static_cast<char>(0xC0 | ((glyph_ >> 6) & 0x1F));
        result += static_cast<char>(0x80 | (glyph_ & 0x3F));
      }

      else if (glyph_ <= 0xFFFF)
      {
        result += static_cast<char>(0xE0 | ((glyph_ >> 12) & 0x0F));
        result += static_cast<char>(0x80 | ((glyph_ >> 6) & 0x3F));
        result += static_cast<char>(0x80 | (glyph_ & 0x3F));
      }

      else
      {
        result += static_cast<char>(0xF0 | ((glyph_ >> 18) & 0x07));
        result += static_cast<char>(0x80 | ((glyph_ >> 12) & 0x3F));
        result += static_cast<char>(0x80 | ((glyph_ >> 6) & 0x3F));
        result += static_cast<char>(0x80 | (glyph_ & 0x3F));
      }

      return result;
    }
  };

  Part& horizontal() { return horizontal_; }
  Part& vertical() { return vertical_; }
  Part& cornerTopLeft() { return cornerTopLeft_; }
  Part& cornerTopRight() { return cornerTopRight_; }
  Part& cornerBottomLeft() { return cornerBottomLeft_; }
  Part& cornerBottomRight() { return cornerBottomRight_; }
  Part& intersection() { return intersection_; }
  Part& connectorLeft() { return connectorLeft_; }
  Part& connectorRight() { return connectorRight_; }
  Part& connectorTop() { return connectorTop_; }
  Part& connectorBottom() { return connectorBottom_; }

  const Part& horizontal() const { return horizontal_; }
  const Part& vertical() const { return vertical_; }
  const Part& cornerTopLeft() const { return cornerTopLeft_; }
  const Part& cornerTopRight() const { return cornerTopRight_; }
  const Part& cornerBottomLeft() const { return cornerBottomLeft_; }
  const Part& cornerBottomRight() const { return cornerBottomRight_; }
  const Part& intersection() const { return intersection_; }
  const Part& connectorLeft() const { return connectorLeft_; }
  const Part& connectorRight() const { return connectorRight_; }
  const Part& connectorTop() const { return connectorTop_; }
  const Part& connectorBottom() const { return connectorBottom_; }

  Border& horizontal(const uint32_t glyph)
  {
    horizontal_.glyph(glyph);
    return *this;
  }
  Border& vertical(const uint32_t glyph)
  {
    vertical_.glyph(glyph);
    return *this;
  }
  Border& cornerTopLeft(const uint32_t glyph)
  {
    cornerTopLeft_.glyph(glyph);
    return *this;
  }
  Border& cornerTopRight(const uint32_t glyph)
  {
    cornerTopRight_.glyph(glyph);
    return *this;
  }
  Border& cornerBottomLeft(const uint32_t glyph)
  {
    cornerBottomLeft_.glyph(glyph);
    return *this;
  }
  Border& cornerBottomRight(const uint32_t glyph)
  {
    cornerBottomRight_.glyph(glyph);
    return *this;
  }
  Border& intersection(const uint32_t glyph)
  {
    intersection_.glyph(glyph);
    return *this;
  }
  Border& connectorLeft(const uint32_t glyph)
  {
    connectorLeft_.glyph(glyph);
    return *this;
  }
  Border& connectorRight(const uint32_t glyph)
  {
    connectorRight_.glyph(glyph);
    return *this;
  }
  Border& connectorTop(const uint32_t glyph)
  {
    connectorTop_.glyph(glyph);
    return *this;
  }
  Border& connectorBottom(const uint32_t glyph)
  {
    connectorBottom_.glyph(glyph);
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
  static Border Double()
  {
    Border border;
    border.horizontal_ = U'═';
    border.vertical_ = U'║';

    border.cornerTopLeft_ = U'╔';
    border.cornerTopRight_ = U'╗';
    border.cornerBottomLeft_ = U'╚';
    border.cornerBottomRight_ = U'╝';

    border.intersection_ = U'╬';

    border.connectorLeft_ = U'╠';
    border.connectorRight_ = U'╣';
    border.connectorTop_ = U'╦';
    border.connectorBottom_ = U'╩';

    return border;
  }
  static Border Rounded()
  {
    Border border;
    border.horizontal_ = U'─';
    border.vertical_ = U'│';

    border.cornerTopLeft_ = U'╭';
    border.cornerTopRight_ = U'╮';
    border.cornerBottomLeft_ = U'╰';
    border.cornerBottomRight_ = U'╯';

    border.intersection_ = U'┼';

    border.connectorLeft_ = U'├';
    border.connectorRight_ = U'┤';
    border.connectorTop_ = U'┬';
    border.connectorBottom_ = U'┴';

    return border;
  }
  static Border Heavy()
  {
    Border border;
    border.horizontal_ = U'━';
    border.vertical_ = U'┃';

    border.cornerTopLeft_ = U'┏';
    border.cornerTopRight_ = U'┓';
    border.cornerBottomLeft_ = U'┗';
    border.cornerBottomRight_ = U'┛';

    border.intersection_ = U'╋';

    border.connectorLeft_ = U'┣';
    border.connectorRight_ = U'┫';
    border.connectorTop_ = U'┳';
    border.connectorBottom_ = U'┻';

    return border;
  }

  void reset()
  {
    horizontal_ = U'-'; // ─
    vertical_ = U'|'; // │

    cornerTopLeft_ = U'+'; // ┌
    cornerTopRight_ = U'+'; // ┐
    cornerBottomLeft_ = U'+'; // └
    cornerBottomRight_ = U'+'; // ┘

    intersection_ = U'+'; // ┼

    connectorLeft_ = U'+'; // ├
    connectorRight_ = U'+'; // ┤
    connectorTop_ = U'-'; // ┬
    connectorBottom_ = U'-'; // ┴
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
