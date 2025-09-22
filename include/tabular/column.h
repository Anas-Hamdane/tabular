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
} // namespace detail
// clang-format off
class Column {
public:
  class Style {
  public:
    explicit Style(Column& parent) : parent_(parent)
    {
    }

    Style& fg(Color color)
    {
      fg_ = static_cast<uint32_t>(color);
      parent_.makeDirty();
      return *this;
    }
    Style& bg(Color color)
    {
      bg_ = static_cast<uint32_t>(color);
      parent_.makeDirty();
      return *this;
    }
    Style& base(Color color)
    {
      base_ = static_cast<uint32_t>(color);
      parent_.makeDirty();
      return *this;
    }

    Style& fg(const Rgb rgb)
    {
      fg_ = rgb.toHex() | (1u << 24);
      parent_.makeDirty();
      return *this;
    }
    Style& bg(const Rgb rgb)
    {
      bg_ = rgb.toHex() | (1u << 24);
      parent_.makeDirty();
      return *this;
    }
    Style& base(const Rgb rgb)
    {
      base_ = rgb.toHex() | (1u << 24);
      parent_.makeDirty();
      return *this;
    }

    Style& attrs(Attr attr)
    {
      attrs_ |= static_cast<uint16_t>(attr);
      parent_.makeDirty();
      return *this;
    }
    Style& attrs(const Style& attr)
    {
      attrs_ |= attr.attrs_;
      parent_.makeDirty();
      return *this;
    }

    bool hasFg() const { return fg_ != 0; }
    bool hasBg() const { return bg_ != 0; }
    bool hasBase() const { return base_ != 0; }
    bool hasAttrs() const { return attrs_ != 0; }

    uint32_t fg() const { return fg_; }
    uint32_t bg() const { return bg_; }
    uint32_t base() const { return base_; }
    Attr attrs() const { return static_cast<Attr>(attrs_); }

    void resetFg()
    {
      fg_ = 0;
      parent_.makeDirty();
    }
    void resetBg()
    {
      bg_ = 0;
      parent_.makeDirty();
    }
    void resetBase()
    {
      base_ = 0;
      parent_.makeDirty();
    }
    void resetAttrs()
    {
      attrs_ = 0;
      parent_.makeDirty();
    }
    void reset()
    {
      fg_ = 0;
      bg_ = 0;
      base_ = 0;
      attrs_ = 0;
      parent_.makeDirty();
    }

  private:
    Column& parent_;

    // representing all the colors
    uint32_t fg_ = 0;
    uint32_t bg_ = 0;
    uint32_t base_ = 0;

    // all the attributes
    uint16_t attrs_ = 0;
  };
  class Config {
  public:
    explicit Config(Column& parent) : parent_(parent)
    {
    }

    Align align() const { return align_; }
    Padd padd() const { return padd_; }
    size_t width() const { return width_; }
    std::string delimiter() const { return delimiter_; }
    bool skipEmptyLineIndent() const { return skipEmptyLineIndent_; }

    Config& align(const Align alignment)
    {
      align_ = alignment;
      parent_.makeDirty();
      return *this;
    }
    Config& padd(const Padd padd)
    {
      padd_ = padd;
      parent_.makeDirty();
      return *this;
    }
    Config& width(const size_t width)
    {
      width_ = width;
      parent_.makeDirty();
      return *this;
    }
    Config& delimiter(std::string delimiter)
    {
      delimiter_ = std::move(delimiter);
      parent_.makeDirty();
      return *this;
    }
    Config& skipEmptyLineIndent(const bool skip)
    {
      skipEmptyLineIndent_ = skip;
      parent_.makeDirty();
      return *this;
    }

    void reset()
    {
      align_ = Align::Left;
      padd_ = Padd();
      delimiter_ = "-";
      width_ = 0;
      skipEmptyLineIndent_ = true;
      parent_.makeDirty();
    }

  private:
    Column& parent_;

    Align align_ = Align::Left;
    Padd padd_ = Padd();
    std::string delimiter_ = "-";
    size_t width_ = 0;
    bool skipEmptyLineIndent_ = true;
  };

public:
  Column() = default;

  Column(std::string content)
    : content_(std::move(content)), dirty_(true)
  {
  }

  void content(std::string content)
  {
    content_ = std::move(content);
    makeDirty();
  }

  Config& config() { return config_; }
  Style& style() { return style_; }

  const Config& config() const { return config_; }
  const Style& style() const { return style_; }

  const std::string& content() const { return content_; }
  std::string& content()
  {
    makeDirty();
    return content_;
  }

  explicit operator const std::string&() const
  {
    return content_;
  }
  explicit operator std::string&()
  {
    makeDirty();
    return content_;
  }

  char& operator[](int index)
  {
    makeDirty();
    return content_.at(index);
  }
  const char& operator[](int index) const
  {
    return content_.at(index);
  }

  void clr()
  {
    content_.clear();
    config_.reset();
    style_.reset();
    lines_.clear();
    makeClean();
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

private:
  Config config_{*this};
  Style style_{*this};
  std::string content_;

  // cache
  mutable std::vector<std::string> lines_;
  mutable bool dirty_ = false;

  void makeDirty() const { dirty_ = true; }
  void makeClean() const { dirty_ = false; }

  std::vector<std::string> genLines() const
  {
    std::string delimiter = config().delimiter();
    size_t delimiterDw = string_utils::dw(delimiter);
    Padd padd = config().padd();

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
    const auto words = split(content_);

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
  static void handleAttrs(std::string& styles, Attr attr)
  {
    auto hasAttr = [](Attr attr, Attr flag) -> bool {
      return static_cast<uint16_t>(attr) & static_cast<uint16_t>(flag);
    };

    if (hasAttr(attr, Attr::Bold)) styles += "1;";
    if (hasAttr(attr, Attr::Dim)) styles += "2;";
    if (hasAttr(attr, Attr::Italic)) styles += "3;";
    if (hasAttr(attr, Attr::Underline)) styles += "4;";
    if (hasAttr(attr, Attr::Dunderline)) styles += "21;";
    if (hasAttr(attr, Attr::Blink)) styles += "5;";
    if (hasAttr(attr, Attr::Flink)) styles += "6;";
    if (hasAttr(attr, Attr::Reverse)) styles += "7;";
    if (hasAttr(attr, Attr::Concealed)) styles += "8;";
    if (hasAttr(attr, Attr::Crossed)) styles += "9;";
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

      if (isSpace(str[i]))
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
    const bool skipBlanks = config_.skipEmptyLineIndent();

    std::vector<detail::Str> lines;
    lines.reserve(content_.length() / width + 1);

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
      if (skipBlanks && buffer.empty() && word == " ") continue;

      // HANDLE new lines
      if (word == "\n")
      {
        startNewLine();
        continue;
      }

      // IGNORE other space characters
      if (word.length() == 1 && isSpace(word[0]) && word != " ") continue;

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
          std::string utf8char = readUtf8Char(word, pos);
          size_t utf8charDw = dw(utf8char);

          // if it will exceed the limit don't append
          if (utf8charDw + firstPartDw > limit) break;

          // otherwise append this part
          firstPart += utf8char;
          firstPartDw += utf8charDw;

          if (utf8char.empty()) pos++;
          else pos += utf8char.length();
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
                                  const Padd padd) const
  {
    using namespace string_utils;

    const size_t width = config().width();
    const std::string base = resolveBase();
    const Align align = config().align();

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
      case Align::Left:
        leftSpace = 0;
        rightSpace = freeSpace;
        break;
      case Align::Center: {
        leftSpace = freeSpace / 2;
        rightSpace = freeSpace - leftSpace;
        break;
      }
      case Align::Right:
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

inline Attr operator|(Attr lhs, Attr rhs) noexcept
{
  return static_cast<Attr>(static_cast<uint16_t>(lhs) |
                                static_cast<uint16_t>(rhs));
}

inline Attr& operator|=(Attr& lhs, const Attr rhs) noexcept
{
  lhs = lhs | rhs;
  return lhs;
}
} // namespace tabular
