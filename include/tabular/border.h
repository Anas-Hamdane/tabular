#pragma once

#include <string>
#include "color.h"
#include "global.h"

namespace tabular {
class Border {
public:
  class Part {
  public:
    Part(const uint32_t form)
      : form_(form), regenerate_(true)
    {
    }

    Part& form(const uint32_t form)
    {
      this->form_ = form;
      this->regenerate_ = true;
      return *this;
    }

    Part& fg(const Color color)
    {
      this->fg_ = static_cast<uint32_t>(color);
      this->regenerate_ = true;
      return *this;
    }
    Part& fg(const Rgb rgb)
    {
      this->fg_ = rgb.toHex() | (1u << 24);
      this->regenerate_ = true;
      return *this;
    }
    uint32_t fg()const
    {
      return this->fg_;
    }

    Part& bg(const Color color)
    {
      this->bg_ = static_cast<uint32_t>(color);
      this->regenerate_ = true;
      return *this;
    }
    Part& bg(const Rgb rgb)
    {
      this->bg_ = rgb.toHex() | (1u << 24);
      this->regenerate_ = true;
      return *this;
    }
    uint32_t bg()const
    {
      return this->bg_;
    }

    Part& resetFg()
    {
      this->fg_ = 0;
      this->regenerate_ = true;
      return *this;
    }
    Part& resetBg()
    {
      this->bg_ = 0;
      this->regenerate_ = true;
      return *this;
    }

    std::string toStr() const
    {
      if (this->regenerate_)
      {
        this->str = reConstructStr();
        this->regenerate_ = false;
      }

      return this->str;
    }
    std::string reConstructStr() const
    {
      using namespace detail;
      std::string buffer;

      ColorType fg = this->fg_;
      ColorType bg = this->bg_;

      if (fg.isColor())
      {
        buffer.append("\x1b[");
        buffer += std::to_string(static_cast<uint8_t>(fg.color())) + ';';
      }
      else if (fg.isRgb())
      {
        Rgb rgb = fg.rgb();
        buffer.append("\x1b[38;2;");
        buffer += std::to_string(rgb.r) + ';';
        buffer += std::to_string(rgb.g) + ';';
        buffer += std::to_string(rgb.b) + ';';
      }

      if (bg.isColor())
      {
        if (buffer.empty()) buffer.append("\x1b[");
        buffer += std::to_string(static_cast<uint8_t>(bg.color()) + 10) + ';';
      }
      else if (bg.isRgb())
      {
        Rgb rgb = bg.rgb();
        if (buffer.empty()) buffer.append("\x1b[38;2;");
        buffer += std::to_string(rgb.r) + ';';
        buffer += std::to_string(rgb.g) + ';';
        buffer += std::to_string(rgb.b) + ';';
      }

      std::string form = formts();
      buffer.append(form);

      if (buffer.length() - form.length() > 0)
        buffer.append(RESET_ESC);

      return buffer;
    }

    operator std::string() const
    {
      return this->toStr();
    }

  private:
    uint32_t form_ = 0; // Unicode code point
    uint32_t fg_ = 0; // fg color
    uint32_t bg_ = 0; // bg color

    mutable bool regenerate_ = false; // cache flag
    mutable std::string str; // cached string form

    std::string formts() const
    {
      std::string result;

      if (form_ <= 0x7F)
      {
        result += static_cast<char>(form_);
      }

      else if (form_ <= 0x7FF)
      {
        result += static_cast<char>(0xC0 | ((form_ >> 6) & 0x1F));
        result += static_cast<char>(0x80 | (form_ & 0x3F));
      }

      else if (form_ <= 0xFFFF)
      {
        result += static_cast<char>(0xE0 | ((form_ >> 12) & 0x0F));
        result += static_cast<char>(0x80 | ((form_ >> 6) & 0x3F));
        result += static_cast<char>(0x80 | (form_ & 0x3F));
      }

      else
      {
        result += static_cast<char>(0xF0 | ((form_ >> 18) & 0x07));
        result += static_cast<char>(0x80 | ((form_ >> 12) & 0x3F));
        result += static_cast<char>(0x80 | ((form_ >> 6) & 0x3F));
        result += static_cast<char>(0x80 | (form_ & 0x3F));
      }

      return result;
    }
  };

  Part& horizontal() { return this->horizontal_; }
  const Part& horizontal() const { return this->horizontal_; }
  Border& horizontal(const uint32_t form)
  {
    this->horizontal_.form(form);
    return *this;
  }

  Part& vertical() { return this->vertical_; }
  const Part& vertical() const { return this->vertical_; }
  Border& vertical(const uint32_t form)
  {
    this->vertical_.form(form);
    return *this;
  }

  Part& cornerTopLeft() { return this->cornerTopLeft_; }
  const Part& cornerTopLeft() const { return this->cornerTopLeft_; }
  Border& cornerTopLeft(const uint32_t form)
  {
    this->cornerTopLeft_.form(form);
    return *this;
  }

  Part& cornerTopRight() { return this->cornerTopRight_; }
  const Part& cornerTopRight() const { return this->cornerTopRight_; }
  Border& cornerTopRight(const uint32_t form)
  {
    this->cornerTopRight_.form(form);
    return *this;
  }

  Part& cornerBottomLeft() { return this->cornerBottomLeft_; }
  const Part& cornerBottomLeft() const { return this->cornerBottomLeft_; }
  Border& cornerBottomLeft(const uint32_t form)
  {
    this->cornerBottomLeft_.form(form);
    return *this;
  }

  Part& cornerBottomRight() { return this->cornerBottomRight_; }
  const Part& cornerBottomRight() const { return this->cornerBottomRight_; }
  Border& cornerBottomRight(const uint32_t form)
  {
    this->cornerBottomRight_.form(form);
    return *this;
  }

  Part& intersection() { return this->intersection_; }
  const Part& intersection() const { return this->intersection_; }
  Border& intersection(const uint32_t form)
  {
    this->intersection_.form(form);
    return *this;
  }

  Part& connectorLeft() { return this->connectorLeft_; }
  const Part& connectorLeft() const { return this->connectorLeft_; }
  Border& connectorLeft(const uint32_t form)
  {
    this->connectorLeft_.form(form);
    return *this;
  }

  Part& connectorRight() { return this->connectorRight_; }
  const Part& connectorRight() const { return this->connectorRight_; }
  Border& connectorRight(const uint32_t form)
  {
    this->connectorRight_.form(form);
    return *this;
  }

  Part& connectorTop() { return this->connectorTop_; }
  const Part& connectorTop() const { return this->connectorTop_; }
  Border& connectorTop(const uint32_t form)
  {
    this->connectorTop_.form(form);
    return *this;
  }

  Part& connectorBottom() { return this->connectorBottom_; }
  const Part& connectorBottom() const { return this->connectorBottom_; }
  Border& connectorBottom(const uint32_t form)
  {
    this->connectorBottom_.form(form);
    return *this;
  }

  static Border Default()
  {
    // already default constructed
    return Border();
  }
  static Border None()
  {
    Border border;
    border.horizontal_ = '\0';
    border.vertical_ = '\0';

    border.cornerTopLeft_ = '\0';
    border.cornerTopRight_ = '\0';
    border.cornerBottomLeft_ = '\0';
    border.cornerBottomRight_ = '\0';

    border.intersection_ = '\0';

    border.connectorLeft_ = '\0';
    border.connectorRight_ = '\0';
    border.connectorTop_ = '\0';
    border.connectorBottom_ = '\0';

    return border;
  }
  static Border Blank()
  {
    Border border;
    border.horizontal_ = ' ';
    border.vertical_ = ' ';

    border.cornerTopLeft_ = ' ';
    border.cornerTopRight_ = ' ';
    border.cornerBottomLeft_ = ' ';
    border.cornerBottomRight_ = ' ';

    border.intersection_ = ' ';

    border.connectorLeft_ = ' ';
    border.connectorRight_ = ' ';
    border.connectorTop_ = ' ';
    border.connectorBottom_ = ' ';

    return border;
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
  Part horizontal_ = '-'; // ─
  Part vertical_ = '|'; // │

  Part cornerTopLeft_ = '+'; // ┌
  Part cornerTopRight_ = '+'; // ┐
  Part cornerBottomLeft_ = '+'; // └
  Part cornerBottomRight_ = '+'; // ┘

  Part intersection_ = '+'; // ┼

  Part connectorLeft_ = '+'; // ├
  Part connectorRight_ = '+'; // ┤
  Part connectorTop_ = '-'; // ┬
  Part connectorBottom_ = '-'; // ┴
};
}
