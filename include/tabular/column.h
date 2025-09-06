#pragma once

#include "global.h"
#include "string.h"
#include <utility>
#include <vector>

namespace tabular {

namespace detail {
// for strings
inline bool isspace(const char c)
{
  return c == ' ' || c == '\n' || c == '\t' || c == '\v' || c == '\f' || c == '\r';
}
inline std::string readUtf8Char(const std::string& str, const size_t pos)
{
  if (pos >= str.length()) return "";

  const auto first_byte = static_cast<unsigned char>(str[pos]);

  // if it's a continuation byte then it's invalid
  if ((first_byte & 0xC0) == 0x80) return "";

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
  if (pos + len > str.length()) return "";

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
constexpr bool isColorSet(const uint32_t colorValue)
{
  return colorValue != 0;
}
constexpr bool isColor(const uint32_t colorValue)
{
  return isColorSet(colorValue) && (colorValue & (1u << 24)) == 0;
}
constexpr bool isRgb(const uint32_t colorValue)
{
  return isColorSet(colorValue) && (colorValue & (1u << 24)) != 0;
}

constexpr Color getColor(const uint32_t colorValue)
{
  return static_cast<Color>(colorValue & 0xFFFFFF);
}
constexpr Rgb getRgb(const uint32_t colorValue)
{
  return {colorValue & 0xFFFFFF};
}
} // namespace detail

class Column {
public:
  class Style {
  public:
    constexpr Style() = default;
    constexpr explicit Style(Attribute attr) : attrs_(static_cast<uint16_t>(attr)) {}

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
      this->delimiter_ = std::move(delimiter);
      return *this;
    }

  private:
    Alignment align_ = Alignment::Left;
    Padding padd_ = Padding();
    String delimiter_ = String("-");
    size_t width_ = 0;
  };

  Column() = default;
  explicit Column(std::string content) : content(std::move(content)) {}

  std::string getContent() const { return this->content; }
  void setContent(std::string content) { this->content = std::move(content); }

  Config& config() { return this->config_; }
  const Config& config() const { return this->config_; }

  Style& style() { return this->style_; }
  const Style& style() const { return this->style_; }

  std::vector<String> toString() const
  {
    const std::string base = resolveBase();
    const std::string styles = resolveStyles();

    const size_t width = config().width();
    const Padding padd = config().padd();
    const Alignment align = config().align();
    const String delimiter = config().delimiter();

    // split into words
    const std::vector<String> words = split();

    // wrap the words into lines that don't exceed
    // `width - padd.left - padd.right` to leave space for the padding
    std::vector<String> lines =
        wrap(words, width - padd.left - padd.right, styles, delimiter);

    // format the lines handling padding, alignment and the base styles
    lines = format(lines, width, padd, align, base);
    return lines;
  }

private:
  Config config_;
  Style style_;
  std::string content;

  static void handleColor(std::string& styles, uint32_t color, bool back)
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
      styles += std::to_string(rgb.r) + ';';
      styles += std::to_string(rgb.g) + ';';
      styles += std::to_string(rgb.b) + ';';
    }
  }
  static void handleAttrs(std::string& styles, Attribute attr)
  {
    auto hasAttr = [](Attribute attr, Attribute flag) -> bool {
      return static_cast<uint16_t>(attr) & static_cast<uint16_t>(flag);
    };

    if (hasAttr(attr, Attribute::Bold)) styles += "1;";
    if (hasAttr(attr, Attribute::Dim)) styles += "2;";
    if (hasAttr(attr, Attribute::Italic)) styles += "3;";
    if (hasAttr(attr, Attribute::Underline)) styles += "4;";
    if (hasAttr(attr, Attribute::Dunderline)) styles += "21;";
    if (hasAttr(attr, Attribute::Blink)) styles += "5;";
    if (hasAttr(attr, Attribute::Flink)) styles += "6;";
    if (hasAttr(attr, Attribute::Reverse)) styles += "7;";
    if (hasAttr(attr, Attribute::Concealed)) styles += "8;";
    if (hasAttr(attr, Attribute::Crossed)) styles += "9;";
  }
  std::string resolveStyles() const
  {
    std::string styles = "\x1b[";

    if (style().hasAttrs()) handleAttrs(styles, style().getAttrs());

    if (style().hasFg()) handleColor(styles, style().getFg(), false);

    if (style().hasBg()) handleColor(styles, style().getBg(), true);

    if (styles == "\x1b[") return ""; // empty

    if (styles.back() == ';') styles.back() = 'm';
    return styles;
  }
  std::string resolveBase() const
  {
    std::string base = "\x1b[";
    handleColor(base, style().getBase(), true);

    if (base == "\x1b[") return ""; // empty

    if (base.back() == ';') base.back() = 'm';
    return base;
  }
  std::vector<String> split() const
  {
    const size_t len = content.length();

    std::vector<String> words;
    words.reserve(len / WORD_LENGTH_AVERAGE); // average

    String buffer;
    buffer.reserve(WORD_LENGTH_AVERAGE);

    size_t i = 0;
    while (i < len)
    {
      if (content[i] == '\x1b')
      {
        if (!buffer.empty()) words.emplace_back(std::move(buffer));
        buffer.clear();

        while (i < len && detail::isAscii(content[i]) &&
               !detail::isAlpha(content[i]))
          buffer += content[i++];

        if (i < len) buffer += content[i++];
        words.emplace_back(std::move(buffer));
        buffer.clear();
        continue;
      }

      if (detail::isspace(content[i]))
      {
        if (!buffer.empty()) words.emplace_back(std::move(buffer));
        buffer.clear();

        words.emplace_back(content[i++]);
        continue;
      }

      buffer += content[i++];
    }

    if (!buffer.empty()) words.emplace_back(std::move(buffer));
    return words;
  }

  std::vector<String> wrap(const std::vector<String>& words, size_t width,
                           const std::string& styles, const String& delimiter) const
  {
    std::vector<String> lines;
    lines.reserve(this->content.length() / width);

    String buffer;
    buffer.reserve(width);
    if (!styles.empty()) buffer += styles;

    // in case the line contains active escape sequences, and they were NOT
    // reseted (with '\x1b[0m'), we need to register them, reset them
    // at the end of the line, and restore them in the next line.
    std::string activeEscs;

    // helper lambdas to avoid repeating code
    auto appendResetIfNeeded = [&](String& buf) {
      if (!(activeEscs.empty() && styles.empty()) && !buf.endsWith(RESET_ESC))
        buf += RESET_ESC;
    };
    auto startNewLine = [&](std::vector<String>& lns, String& buf) {
      appendResetIfNeeded(buf);
      lns.emplace_back(std::move(buf));
      buf.clear();
      if (!styles.empty()) buf += styles;
      if (!activeEscs.empty()) buf += activeEscs;
    };
    auto wordFits = [&](const String* word) {
      return word && word->dw() + buffer.dw() <= width;
    };

    for (size_t i = 0; i < words.size(); ++i)
    {
      String word = words[i];

      // SKIP empty strings
      if (word.len() == 0) continue;

      // SKIP spaces at the start of a new line
      if (buffer.empty() && word == " ") continue;

      // IGNORE other space characters
      if (word.len() == 1 && detail::isspace(word[0]) && word != " ") continue;

      // HANDLE escape sequences
      if (word[0] == '\x1b')
      {
        if (word.back() != 'm') continue;

        const String* nextWord = i + 1 < words.size() ? &words[i + 1] : nullptr;
        if (word == "\x1b[0m")
        {
          activeEscs.clear();
          buffer += word;

          if (nextWord && wordFits(nextWord)) buffer += styles;
          continue;
        }

        activeEscs += word.toStr();

        if (nextWord && wordFits(nextWord)) buffer += word;
        continue;
      }

      // HANDLE new lines
      if (word == "\n")
      {
        startNewLine(lines, buffer);
        continue;
      }

      // the word fits in the line
      if (wordFits(&word))
      {
        buffer += word;
        continue;
      }

      // the word fits in the next line
      if (word.dw() <= width)
      {
        startNewLine(lines, buffer);

        // add the next word and avoid spaces
        if (word != " ") buffer += word;
        continue;
      }

      /*
       * if we reach here that means the word's display width
       * exceeds the line width
       */

      // no free space, append the current line and process the others
      if (buffer.dw() >= width + delimiter.dw()) startNewLine(lines, buffer);

      while (word.dw() > width)
      {
        size_t limit = width - delimiter.dw() - buffer.dw();

        String firstPart;
        firstPart.reserve(limit);

        size_t pos = 0;
        while (firstPart.dw() < limit)
        {
          String utf8char = detail::readUtf8Char(word.toStr(), pos);

          // if it will exceed the limit don't append
          if (utf8char.dw() + firstPart.dw() > limit) break;

          // otherwise append this part
          firstPart += utf8char;
          pos += utf8char.len();
        }

        firstPart += delimiter;
        buffer += firstPart;

        startNewLine(lines, buffer);
        word = word.toStr().substr(pos);
      }

      buffer += word;
    }

    if (!buffer.empty())
    {
      appendResetIfNeeded(buffer);
      lines.emplace_back(std::move(buffer));
      buffer.clear();
    }

    return lines;
  }
  static std::vector<String> format(std::vector<String>& lines, size_t width,
                                    Padding padd, Alignment align,
                                    const std::string& base)
  {
    std::vector<String> formated;
    formated.reserve(lines.size() + padd.top + padd.bottom);

    bool styled = !base.empty();

    String emptyLine = base;
    emptyLine += std::string(width, ' ');
    if (styled) emptyLine += RESET_ESC;

    formated.insert(formated.end(), padd.top, emptyLine);

    // temporary buffer
    String buffer;
    buffer.reserve(width);

    for (auto& line : lines)
    {
      // append the base styles
      if (styled) buffer += base;

      // calculate the total line width
      size_t lineWidth = line.dw() + padd.left + padd.right;

      // handle the alignment
      size_t freeSpace = width - lineWidth;
      switch (align)
      {
      case Alignment::Left:
        buffer += std::string(padd.left, ' ');
        buffer += line;

        if (line.endsWith("\x1b[0m")) buffer += base;

        buffer += std::string(padd.right, ' ');
        buffer += std::string(freeSpace, ' ');

        break;
      case Alignment::Center: {
        size_t leftSpace = freeSpace / 2;
        buffer += std::string(leftSpace, ' ');

        buffer += std::string(padd.left, ' ');
        buffer += line;

        if (line.endsWith("\x1b[0m")) buffer += base;

        buffer += std::string(padd.right, ' ');

        buffer += std::string(freeSpace - leftSpace, ' ');
        break;
      }
      case Alignment::Right:
        buffer += std::string(freeSpace, ' ');

        buffer += std::string(padd.left, ' ');
        buffer += line;

        if (line.endsWith("\x1b[0m")) buffer += base;

        buffer += std::string(padd.right, ' ');
        break;
      }

      // reset the base styles
      if (styled) buffer += RESET_ESC;

      formated.emplace_back(std::move(buffer));
      buffer.clear();
    }

    formated.insert(formated.end(), padd.bottom, emptyLine);
    return formated;
  }
};

inline Column::Style operator|(Attribute lhs, Attribute rhs)
{
  return Column::Style(lhs) | rhs;
}
} // namespace tabular
