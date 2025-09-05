#pragma once

#include "global.h"
#include "string.h"
#include <vector>

namespace tabular {

namespace detail {
// for strings
inline std::vector<std::string> split(const std::string& text)
{
  size_t len = text.length();

  std::vector<std::string> words;
  words.reserve(len / 5); // average

  size_t start = 0;
  for (size_t i = 0; i < len; ++i)
  {
    char c = text[i];
    if (isspace(c))
    {
      if (start < i)
        words.emplace_back(text.begin() + start, text.begin() + i);

      words.emplace_back(1, c);
      start = i + 1;
    }
  }

  if (start < len)
    words.emplace_back(text.begin() + start, text.end());

  return words;
}
inline bool isspace(char c)
{
  return c == ' ' || c == '\n' || c == '\t' || c == '\v' || c == '\f' || c == '\r';
}
inline std::string readUtf8Char(const std::string& str, size_t pos)
{
  if (pos >= str.length())
    return "";

  unsigned char first_byte = static_cast<unsigned char>(str[pos]);

  // if it's a continuation byte then it's invalid
  if ((first_byte & 0xC0) == 0x80)
    return "";

  // find the length of the sequence from the start byte
  size_t len;
  if ((first_byte & 0x80) == 0)
    len = 1;
  else if ((first_byte & 0xE0) == 0xC0)
    len = 2;
  else if ((first_byte & 0xF0) == 0xE0)
    len = 3;
  else if ((first_byte & 0xF8) == 0xF0)
    len = 4;
  else
    return "";

  // not enough bytes
  if (pos + len > str.length())
    return "";

  // validate
  for (size_t i = 1; i < len; ++i)
  {
    if ((static_cast<unsigned char>(str[pos + i]) & 0xC0) != 0x80)
    {
      return "";
    }
  }

  return str.substr(pos, len);
}

// for colors
constexpr bool isColorSet(uint32_t colorValue) { return colorValue != 0; }
constexpr bool isColor(uint32_t colorValue)
{
  return isColorSet(colorValue) && (colorValue & (1u << 24)) == 0;
}
constexpr bool isRgb(uint32_t colorValue)
{
  return isColorSet(colorValue) && (colorValue & (1u << 24)) != 0;
}

constexpr Color getColor(uint32_t colorValue)
{
  return static_cast<Color>(colorValue & 0xFFFFFF);
}
constexpr Rgb getRgb(uint32_t colorValue) { return Rgb(colorValue & 0xFFFFFF); }
} // namespace detail

class Column {
public:
  class Style {
  public:
    constexpr Style() = default;
    constexpr Style(Attribute attr) : attrs_(static_cast<uint16_t>(attr)) {}

    Style& operator|=(Attribute attr)
    {
      this->attrs_ |= static_cast<uint16_t>(attr);
      return *this;
    }
    Style operator|(Attribute attr) const
    {
      Style result = *this;
      return result |= attr;
    }

    Style& fg(Color color)
    {
      this->fg_ = static_cast<uint32_t>(color);
      return *this;
    }
    Style& fg(Rgb rgb)
    {
      this->fg_ = rgb.toHex() | (1u << 24);
      return *this;
    }
    Style& bg(Color color)
    {
      this->bg_ = static_cast<uint32_t>(color);
      return *this;
    }
    Style& bg(Rgb rgb)
    {
      this->bg_ = rgb.toHex() | (1u << 24);
      return *this;
    }
    Style& base(Color color)
    {
      this->base_ = static_cast<uint32_t>(color);
      return *this;
    }
    Style& base(Rgb rgb)
    {
      this->base_ = rgb.toHex() | (1u << 24);
      return *this;
    }
    Style& attrs(Attribute attr)
    {
      this->attrs_ |= static_cast<uint16_t>(attr);
      return *this;
    }
    Style& attrs(const Style& attr)
    {
      this->attrs_ |= attr.attrs_;
      return *this;
    }

    constexpr bool hasFg() const { return this->fg_ != 0; }
    constexpr bool hasBg() const { return this->bg_ != 0; }
    constexpr bool hasBase() const { return this->base_ != 0; }
    constexpr bool hasAttrs() const { return this->attrs_ != 0; }

    constexpr uint32_t getFg() const { return this->fg_; }
    constexpr uint32_t getBg() const { return this->bg_; }
    constexpr uint32_t getBase() const { return this->base_; }
    constexpr Attribute getAttrs() const { return static_cast<Attribute>(attrs_); }

    void resetFg() { this->fg_ = 0; }
    void resetBg() { this->bg_ = 0; }
    void resetBase() { this->base_ = 0; }
    void resetAttrs() { this->attrs_ = 0; }
    void reset()
    {
      this->fg_ = 0;
      this->bg_ = 0;
      this->base_ = 0;
      this->attrs_ = 0;
    }

  private:
    // representing all the colors
    uint32_t fg_ = 0;
    uint32_t bg_ = 0;
    uint32_t base_ = 0;

    // all the attributes
    uint16_t attrs_ = 0;
  };
  class Config {
  public:
    constexpr Config() = default;

    Alignment align() const { return this->align_; }
    Padding padd() const { return this->padd_; }
    size_t width() const { return this->width_; }
    String delimiter() const { return this->delimiter_; }

    Config& align(Alignment alignment)
    {
      this->align_ = alignment;
      return *this;
    }
    Config& padd(Padding padd)
    {
      this->padd_ = padd;
      return *this;
    }
    Config& width(size_t width)
    {
      this->width_ = width;
      return *this;
    }
    Config& delimiter(String delimiter)
    {
      this->delimiter_ = delimiter;
      return *this;
    }

  private:
    Alignment align_ = Alignment::Left;
    Padding padd_ = Padding(0);
    String delimiter_ = String("-");
    size_t width_ = 0;
  };

  Column() = default;
  Column(std::string content) : content(std::move(content)) {}

  std::string getContent() const { return this->content; }
  void setContent(std::string content) { this->content = std::move(content); }

  Config& config() { return this->config_; }
  const Config& config() const { return this->config_; }

  Style& style() { return this->style_; }
  const Style& style() const { return this->style_; }

  std::vector<String> toString() const
  {
    std::string styles = resolveStyles();
    std::string base = resolveBase();

    size_t width = config().width();
    Padding padd = config().padd();
    String delimiter = config().delimiter();

    std::vector<String> lines;
    lines.reserve(this->content.length() / width);

    // empty line for the padding
    String emptyLine;
    emptyLine += base + std::string(width, ' ');
    if (!base.empty())
      emptyLine += RESET_ESC;
    lines.insert(lines.end(), padd.top, emptyLine);

    auto words = detail::split(this->content);
    String line;
    line.reserve(width);

    // in case the line contains active escape sequences and they were NOT
    // reseted (with '\x1b[0m'), we need to register them, reset them
    // at the end of the line, and restore them in the next line.
    std::string activeEscs;
    for (size_t i = 0; i < words.size(); ++i)
    {
      String word = words[i];
      size_t len = word.dw();

      if (word == "\n")
      {
        formatLine(line, activeEscs, styles);
        lines.emplace_back(std::move(line));
        line.clear();
        continue;
      }

      // skip spaces at the start of a new line
      if (line.empty() && word == " ")
        continue;

      // ignore other space characters
      if (len == 1 && detail::isspace(word[0]) && word != " ")
        continue;

      // the word fits in the line
      if (line.dw() + len <= width)
      {
        line += word;
        continue;
      }

      // the word fits in the next line
      if (len <= width)
      {
        formatLine(line, activeEscs, styles);
        lines.emplace_back(std::move(line));
        line.clear();
        if (word != " ")
          line += word;
        continue;
      }

      // if we reach here that means the word's display width
      // exceeds the line width
      while (len > width)
      {
        size_t limit = width - delimiter.dw() - line.dw();

        String firstPart;
        firstPart.reserve(limit);

        size_t pos = 0;
        while (firstPart.dw() < limit)
        {
          String buff = detail::readUtf8Char(word.toStr(), pos);

          // if it will exceed the limit
          if (buff.dw() + firstPart.dw() > limit)
            break;
          firstPart += buff;
          pos += buff.len();
        }

        firstPart += delimiter;
        line += firstPart;

        formatLine(line, activeEscs, styles);
        lines.emplace_back(std::move(line));
        line.clear();

        word = word.toStr().substr(pos);
        len = word.dw();
      }

      line += word;
      continue;
    }

    if (!line.empty())
    {
      formatLine(line, activeEscs, styles);
      lines.emplace_back(std::move(line));
      line.clear();
    }

    return lines;
  }

private:
  Config config_;
  Style style_;
  std::string content;

  void resolveColor(std::string& styles, uint32_t color, bool back) const
  {
    using namespace detail;
    if (isColor(color))
    {
      Color c = getColor(color);
      styles += std::to_string(static_cast<uint8_t>(c) + (back ? 10 : 0)) + ';';
    }

    else if (isRgb(color))
    {
      Rgb rgb = getRgb(color);
      styles += back ? "48;2;" : "38;2;";
      styles += std::to_string(static_cast<uint8_t>(rgb.r)) + ';';
      styles += std::to_string(static_cast<uint8_t>(rgb.g)) + ';';
      styles += std::to_string(static_cast<uint8_t>(rgb.b)) + ';';
    }
  }
  std::string resolveStyles() const
  {
    std::string styles = "\x1b[";

    auto hasAttr = [](Attribute attr, Attribute flag) -> bool {
      return static_cast<uint16_t>(attr) & static_cast<uint16_t>(flag);
    };
    Attribute attr = style().getAttrs();

    if (hasAttr(attr, Attribute::Bold))
      styles += "1;";
    if (hasAttr(attr, Attribute::Dim))
      styles += "2;";
    if (hasAttr(attr, Attribute::Italic))
      styles += "3;";
    if (hasAttr(attr, Attribute::Underline))
      styles += "4;";
    if (hasAttr(attr, Attribute::Dunderline))
      styles += "21;";
    if (hasAttr(attr, Attribute::Blink))
      styles += "5;";
    if (hasAttr(attr, Attribute::Flink))
      styles += "6;";
    if (hasAttr(attr, Attribute::Reverse))
      styles += "7;";
    if (hasAttr(attr, Attribute::Concealed))
      styles += "8;";
    if (hasAttr(attr, Attribute::Crossed))
      styles += "9;";

    resolveColor(styles, style().getFg(), false);
    resolveColor(styles, style().getBg(), true);
    resolveColor(styles, style().getBase(), true);

    if (styles == "\x1b[")
      return ""; // empty
    if (styles.back() == ';')
      styles.back() = 'm';
    return styles;
  }
  std::string resolveBase() const
  {
    std::string base = "\x1b[";
    resolveColor(base, style().getBase(), true);

    if (base == "\x1b[")
      return ""; // empty
    if (base.back() == ';')
      base.back() = 'm';
    return base;
  }
  std::string activeEscSeqs(String& line, const std::string& styles) const
  {
    using namespace detail;
    std::string active;

    // helper function
    auto endsWith = [](const std::string& str, const std::string& with) {
      return str.compare(str.length() - with.length(), with.length(), with);
    };

    size_t len = line.len();
    for (size_t i = 0; i < len; ++i)
    {
      if (line[i] != '\x1b')
        continue;

      std::string current;
      current += line[i++];

      // break at the first ascii letter
      while (i < len && isAscii(line[i]) && !isAlpha(line[i]))
        current += line[i++];

      // if it's 'm' register it
      if (i < len && line[i] == 'm')
      {
        current += line[i++];
        if (current == "\x1b[0m" ||
            (current.length() >= 3 && endsWith(current, ";0m")))
        {
          // insert the styles to avoid reseting them
          if (!styles.empty()) line.insert(i, styles);
          active.clear();
          continue;
        }

        active += current;
      }
    }

    return active;
  }
  void formatLine(String& line, std::string& activeEscs,
                  const std::string& styles) const
  {
    bool shouldReset = false;
    if (!activeEscs.empty())
      line.insert(0, activeEscs);

    activeEscs = activeEscSeqs(line, styles);
    if (!activeEscs.empty())
      shouldReset = true;

    alignLine(line);

    line.insert(0, styles);
    if (!styles.empty())
      shouldReset = true;

    if (shouldReset)
      line += RESET_ESC;
  }
  void alignLine(String& line) const
  {
    size_t width = config().width();
    Alignment align = config().align();

    size_t freeSpace = width - line.dw();
    if (freeSpace == 0)
      return;

    switch (align)
    {
    case Alignment::Left:
      line += std::string(freeSpace, ' ');
      break;
    case Alignment::Center: {
      size_t left = freeSpace / 2;
      line.insert(0, std::string(left, ' '));
      line += std::string(freeSpace - left, ' ');
      break;
    }
    case Alignment::Right:
      line.insert(0, std::string(freeSpace, ' '));
      break;
    }
  }
};

inline Column::Style operator|(Attribute lhs, Attribute rhs)
{
  return Column::Style(lhs) | rhs;
}
} // namespace tabular
