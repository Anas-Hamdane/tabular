#pragma once

#include "color.h"
#include "global.h"
#include "string_utils.h"

#include <vector>

namespace tabular {
namespace detail {
struct Str {
  std::string str;
  size_t dw;
};

// for strings
inline bool isSpace(const char c)
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
} // namespace detail
// clang-format off
class Column {
public:
  class Style {
  public:
    explicit Style(Column& parent) : parent(parent)
    {
    }

    Style& fg(Color color)
    {
      this->fg_ = static_cast<uint32_t>(color);
      this->parent.makeDirty();
      return *this;
    }
    Style& bg(Color color)
    {
      this->bg_ = static_cast<uint32_t>(color);
      this->parent.makeDirty();
      return *this;
    }
    Style& base(Color color)
    {
      this->base_ = static_cast<uint32_t>(color);
      this->parent.makeDirty();
      return *this;
    }

    Style& fg(const Rgb rgb)
    {
      this->fg_ = rgb.toHex() | (1u << 24);
      this->parent.makeDirty();
      return *this;
    }
    Style& bg(const Rgb rgb)
    {
      this->bg_ = rgb.toHex() | (1u << 24);
      this->parent.makeDirty();
      return *this;
    }
    Style& base(const Rgb rgb)
    {
      this->base_ = rgb.toHex() | (1u << 24);
      this->parent.makeDirty();
      return *this;
    }

    Style& attrs(Attribute attr)
    {
      this->attrs_ |= static_cast<uint16_t>(attr);
      this->parent.makeDirty();
      return *this;
    }
    Style& attrs(const Style& attr)
    {
      this->attrs_ |= attr.attrs_;
      this->parent.makeDirty();
      return *this;
    }

    bool hasFg() const { return this->fg_ != 0; }
    bool hasBg() const { return this->bg_ != 0; }
    bool hasBase() const { return this->base_ != 0; }
    bool hasAttrs() const { return this->attrs_ != 0; }

    uint32_t fg() const { return this->fg_; }
    uint32_t bg() const { return this->bg_; }
    uint32_t base() const { return this->base_; }
    Attribute attrs() const { return static_cast<Attribute>(attrs_); }

    void resetFg()
    {
      this->fg_ = 0;
      this->parent.makeDirty();
    }
    void resetBg()
    {
      this->bg_ = 0;
      this->parent.makeDirty();
    }
    void resetBase()
    {
      this->base_ = 0;
      this->parent.makeDirty();
    }
    void resetAttrs()
    {
      this->attrs_ = 0;
      this->parent.makeDirty();
    }
    void reset()
    {
      this->fg_ = 0;
      this->bg_ = 0;
      this->base_ = 0;
      this->attrs_ = 0;
      this->parent.makeDirty();
    }

  private:
    Column& parent;

    // representing all the colors
    uint32_t fg_ = 0;
    uint32_t bg_ = 0;
    uint32_t base_ = 0;

    // all the attributes
    uint16_t attrs_ = 0;
  };
  class Config {
  public:
    explicit Config(Column& parent) : parent(parent)
    {
    }

    Alignment align() const { return this->align_; }
    Padding padd() const { return this->padd_; }
    size_t width() const { return this->width_; }
    std::string delimiter() const { return this->delimiter_; }

    Config& align(const Alignment alignment)
    {
      this->align_ = alignment;
      this->parent.makeDirty();
      return *this;
    }
    Config& padd(const Padding padd)
    {
      this->padd_ = padd;
      this->parent.makeDirty();
      return *this;
    }
    Config& width(const size_t width)
    {
      this->width_ = width;
      this->parent.makeDirty();
      return *this;
    }
    Config& delimiter(std::string delimiter)
    {
      this->delimiter_ = std::move(delimiter);
      this->parent.makeDirty();
      return *this;
    }

    void reset()
    {
      this->align_ = Alignment::Left;
      this->padd_ = Padding();
      this->delimiter_ = "-";
      this->width_ = 0;
      this->parent.makeDirty();
    }

  private:
    Column& parent;

    Alignment align_ = Alignment::Left;
    Padding padd_ = Padding();
    std::string delimiter_ = "-";
    size_t width_ = 0;
  };

public:
  Column() = default;

  Column(std::string content)
    : content_(std::move(content)), dirty_(true)
  {
  }

  void content(std::string content)
  {
    this->content_ = std::move(content);
    makeDirty();
  }

  Config& config() { return this->config_; }
  Style& style() { return this->style_; }

  const Config& config() const { return this->config_; }
  const Style& style() const { return this->style_; }

  const std::string& content() const { return this->content_; }
  std::string& content()
  {
    makeDirty();
    return this->content_;
  }

  const std::vector<std::string>& lines() const
  {
    if (dirty_)
    {
      lines_ = genLines();
      makeClean();
    }
    return lines_;
  }
  std::string genEmptyLine() const
  {
    const std::string base = resolveBase();
    const size_t width = config().width();

    std::string empty = base;
    empty.reserve(width);

    empty += std::string(width, ' ');
    if (!base.empty()) empty += RESET_ESC;

    return empty;
  }

  explicit operator const std::string&() const
  {
    return this->content_;
  }
  explicit operator std::string&()
  {
    makeDirty();
    return this->content_;
  }

  char& operator[](int index)
  {
    makeDirty();
    return this->content_.at(index);
  }
  const char& operator[](int index) const
  {
    return this->content_.at(index);
  }

  void clr()
  {
    this->content_.clear();
    this->config_.reset();
    this->style_.reset();
    this->lines_.clear();
    makeClean();
  }

private:
  Config config_ = Config(*this);
  Style style_ = Style(*this);
  std::string content_;

  // cache
  mutable std::vector<std::string> lines_;
  mutable bool dirty_ = false;

  void makeDirty() const { this->dirty_ = true; }
  void makeClean() const { this->dirty_ = false; }

  std::vector<std::string> genLines() const
  {
    std::string delimiter = config().delimiter();
    size_t delimiterDw = string_utils::dw(delimiter);
    Padding padd = config().padd();

    size_t width = config().width();
    if (width < MIN_COLUMN_WIDTH) width = MIN_COLUMN_WIDTH;

    if (padd.right + padd.left >= width)
    {
      padd.right = 0;
      padd.left = 0;
    }

    width -= (padd.left + padd.right);

    if (delimiterDw >= width)
    {
      delimiter = "";
      delimiterDw = 0;
    }

    // split the content into words
    const auto words = split(this->content_);

    // wrap the words into lines
    const std::vector<detail::Str> lines =
        wrap(words, width, delimiter, delimiterDw);

    // format the lines handling padding, alignment and the base styles
    return format(lines, padd);
  }
  static void handleColor(std::string& styles, detail::ColorType colortype,
                          bool back)
  {
    if (colortype.isColor())
    {
      Color c = colortype.color();
      styles += std::to_string(static_cast<uint8_t>(c) + (back ? 10 : 0)) + ';';
    }

    else if (colortype.isRgb())
    {
      Rgb rgb = colortype.rgb();
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

    if (style().hasAttrs()) handleAttrs(styles, style().attrs());

    if (style().hasFg()) handleColor(styles, style().fg(), false);

    if (style().hasBg()) handleColor(styles, style().bg(), true);

    if (styles == "\x1b[") return ""; // empty

    if (styles.back() == ';') styles.back() = 'm';
    return styles;
  }
  std::string resolveBase() const
  {
    std::string base = "\x1b[";
    handleColor(base, style().base(), true);

    if (base == "\x1b[") return ""; // empty

    if (base.back() == ';') base.back() = 'm';
    return base;
  }

  std::vector<std::string> split(const std::string& str) const
  {
    using namespace string_utils;

    const size_t len = str.length();

    std::vector<std::string> words;
    words.reserve((len / WORD_LENGTH_AVERAGE) + 1); // average

    std::string buffer;
    buffer.reserve(WORD_LENGTH_AVERAGE);

    size_t i = 0;
    while (i < len)
    {
      if (str[i] == '\x1b')
      {
        if (!buffer.empty()) words.emplace_back(std::move(buffer));
        buffer.clear();

        while (i < len && isAscii(str[i]) && !isAlpha(str[i]))
          buffer += str[i++];

        if (i < len) buffer += str[i++];
        words.emplace_back(std::move(buffer));
        buffer.clear();
        continue;
      }

      if (detail::isSpace(str[i]))
      {
        if (!buffer.empty()) words.emplace_back(std::move(buffer));
        buffer.clear();

        words.emplace_back(1, str[i++]);
        continue;
      }

      buffer += str[i++];
    }

    if (!buffer.empty()) words.emplace_back(std::move(buffer));
    return words;
  }
  std::vector<detail::Str> wrap(const std::vector<std::string>& words,
                                const size_t width,
                                const std::string& delimiter,
                                const size_t delimiterDw) const
  {
    using namespace string_utils;

    const std::string styles = resolveStyles();

    std::vector<detail::Str> lines;
    lines.reserve(this->content_.length() / width + 1);

    std::string buffer;
    size_t bufferDw = 0; // the display width

    buffer.reserve(width);
    if (!styles.empty()) buffer += styles;

    // in case the line contains active escape sequences, and they were NOT
    // reset (with '\x1b[0m'), we need to register them, reset them
    // at the end of the line, and restore them in the next line.
    std::string activeEscs;

    // helper lambdas to avoid repeating code
    auto appendResetIfNeeded = [&]() {
      if (!(activeEscs.empty() && styles.empty()) && !endsWith(buffer, RESET_ESC))
        buffer += RESET_ESC;
    };
    auto startNewLine = [&]() {
      appendResetIfNeeded();
      lines.push_back({buffer, bufferDw});

      buffer.clear();
      bufferDw = 0;

      if (!styles.empty()) buffer += styles;
      if (!activeEscs.empty()) buffer += activeEscs;
    };

    for (size_t i = 0; i < words.size(); ++i)
    {
      std::string word = words[i];

      // SKIP empty strings
      if (word.empty()) continue;

      // SKIP spaces at the start of a new line
      if (buffer.empty() && word == " ") continue;

      // IGNORE other space characters
      if (word.length() == 1 && detail::isSpace(word[0]) && word != " ") continue;

      // HANDLE escape sequences
      if (word[0] == '\x1b')
      {
        if (word.back() != 'm') continue;

        const auto* nextWord = i + 1 < words.size() ? &words[i + 1] : nullptr;
        if (word == "\x1b[0m")
        {
          activeEscs.clear();
          buffer += word;

          if (nextWord && dw(*nextWord) + bufferDw <= width) buffer += styles;

          continue;
        }

        activeEscs += word;

        if (nextWord && dw(*nextWord) + bufferDw <= width) buffer += word;

        continue;
      }

      // HANDLE new lines
      if (word == "\n")
      {
        startNewLine();
        continue;
      }

      // word display width
      size_t wordDw = dw(word);

      // the word fits in the line
      if (wordDw + bufferDw <= width)
      {
        buffer += word;
        bufferDw += wordDw;
        continue;
      }

      // the word fits in the next line
      if (wordDw <= width)
      {
        startNewLine();

        // add the next word and avoid spaces
        if (word != " ")
        {
          buffer += word;
          bufferDw += wordDw;
        }

        continue;
      }

      /*
       * if we reach here that means the word's display width
       * exceeds the line width
       */

      // no free space, append the current line and process the others
      if (bufferDw >= width - delimiterDw) startNewLine();

      while (wordDw > width)
      {
        size_t limit = width - delimiterDw - bufferDw;

        std::string firstPart;
        size_t firstPartDw = 0;
        firstPart.reserve(limit);

        size_t pos = 0;
        while (firstPartDw < limit)
        {
          std::string utf8char = detail::readUtf8Char(word, pos);
          size_t utf8charDw = dw(utf8char);

          // if it will exceed the limit don't append
          if (utf8charDw + firstPartDw > limit) break;

          // otherwise append this part
          firstPart += utf8char;
          firstPartDw += utf8charDw;

          pos += utf8char.length();
        }

        // prepare the next word
        word = word.substr(pos);
        wordDw -= firstPartDw;

        // add the delimiter
        firstPart += delimiter;
        firstPartDw += delimiterDw;

        // add the stuff into the buffer
        buffer += firstPart;
        bufferDw += firstPartDw;

        // append a new line
        startNewLine();
      }

      buffer += word;
      bufferDw += wordDw;
    }

    if (!buffer.empty())
    {
      appendResetIfNeeded();
      lines.push_back({buffer, bufferDw});
    }

    return lines;
  }
  std::vector<std::string> format(const std::vector<detail::Str>& lines,
                                  const Padding padd) const
  {
    using namespace string_utils;

    const size_t width = config().width();
    const std::string base = resolveBase();
    const Alignment align = config().align();

    std::vector<std::string> formatted;
    formatted.reserve(lines.size() + padd.top + padd.bottom);

    const bool styled = !base.empty();
    const std::string empty = genEmptyLine();
    formatted.insert(formatted.end(), padd.top, empty);

    // temporary buffer
    std::string buffer;
    buffer.reserve(width);

    for (const auto& line : lines)
    {
      // append the base styles
      if (styled) buffer += base;

      // calculate the total line width
      const size_t lineWidth = line.dw + padd.left + padd.right;
      const size_t freeSpace = (width > lineWidth) ? width - lineWidth : 0;

      size_t leftSpace = 0, rightSpace = 0;
      switch (align)
      {
      case Alignment::Left:
        leftSpace = 0;
        rightSpace = freeSpace;
        break;
      case Alignment::Center: {
        leftSpace = freeSpace / 2;
        rightSpace = freeSpace - leftSpace;
        break;
      }
      case Alignment::Right:
        leftSpace = freeSpace;
        rightSpace = 0;
        break;
      }

      buffer.append(leftSpace + padd.left, ' ');
      buffer += line.str;

      if (endsWith(line.str, RESET_ESC)) buffer += base;

      buffer.append(rightSpace + padd.right, ' ');

      // reset the base styles
      if (styled) buffer += RESET_ESC;

      formatted.emplace_back(std::move(buffer));
      buffer.clear();
    }

    formatted.insert(formatted.end(), padd.bottom, empty);
    return formatted;
  }
};
// clang-format on

constexpr Attribute operator|(Attribute lhs, Attribute rhs) noexcept
{
  return static_cast<Attribute>(static_cast<uint16_t>(lhs) |
                                static_cast<uint16_t>(rhs));
}

constexpr Attribute& operator|=(Attribute& lhs, Attribute rhs) noexcept
{
  lhs = lhs | rhs;
  return lhs;
}
} // namespace tabular