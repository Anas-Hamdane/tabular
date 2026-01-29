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
    explicit Style(bool& dirty_) : dirty_(dirty_)
    {
    }

    Style& fg(Color color)
    {
      fg_ = static_cast<uint32_t>(color);
      dirty_ = true;
      return *this;
    }
    Style& bg(Color color)
    {
      bg_ = static_cast<uint32_t>(color);
      dirty_ = true;
      return *this;
    }
    Style& base(Color color)
    {
      base_ = static_cast<uint32_t>(color);
      dirty_ = true;
      return *this;
    }

    Style& fg(const Rgb rgb)
    {
      fg_ = rgb.toHex() | (1u << 24);
      dirty_ = true;
      return *this;
    }
    Style& bg(const Rgb rgb)
    {
      bg_ = rgb.toHex() | (1u << 24);
      dirty_ = true;
      return *this;
    }
    Style& base(const Rgb rgb)
    {
      base_ = rgb.toHex() | (1u << 24);
      dirty_ = true;
      return *this;
    }

    Style& attrs(Attr attr)
    {
      attrs_ |= static_cast<uint16_t>(attr);
      dirty_ = true;
      return *this;
    }
    Style& attrs(const Style& attr)
    {
      attrs_ |= attr.attrs_;
      dirty_ = true;
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
      dirty_ = true;
    }
    void resetBg()
    {
      bg_ = 0;
      dirty_ = true;
    }
    void resetBase()
    {
      base_ = 0;
      dirty_ = true;
    }
    void resetAttrs()
    {
      attrs_ = 0;
      dirty_ = true;
    }
    void reset()
    {
      fg_ = 0;
      bg_ = 0;
      base_ = 0;
      attrs_ = 0;
      dirty_ = true;
    }

  private:
    friend class Column;
    bool& dirty_;

    // representing all the colors
    uint32_t fg_ = 0;
    uint32_t bg_ = 0;
    uint32_t base_ = 0;

    // all the attributes
    uint16_t attrs_ = 0;
  };
  class Config {
  public:
    explicit Config(bool& dirty_) : dirty_(dirty_)
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
      dirty_ = true;
      return *this;
    }
    Config& padd(const Padd padd)
    {
      padd_ = padd;
      dirty_ = true;
      return *this;
    }
    Config& width(const size_t width)
    {
      width_ = width;
      dirty_ = true;
      return *this;
    }
    Config& delimiter(std::string delimiter)
    {
      delimiter_ = std::move(delimiter);
      dirty_ = true;
      return *this;
    }
    Config& skipEmptyLineIndent(const bool skip)
    {
      skipEmptyLineIndent_ = skip;
      dirty_ = true;
      return *this;
    }

    void reset()
    {
      align_ = Align::Left;
      padd_ = Padd();
      delimiter_ = "-";
      width_ = 0;
      skipEmptyLineIndent_ = true;
      dirty_ = true;
    }

  private:
    friend class Column;
    bool& dirty_;

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

  // copy constructor
  // NOTE: if we don't initialize config_ and style_ with dirty_, and instead copy
  // other.config_ and other.style_ we will get a heap-use-after-free error from the address sanitizer
  // because config_ will still have the old dirty_ flag that points to the old object
  // to avoid this we will initialize config_ and style_ with dirty_ of the current object
  // and then copy the data of other.config_ and other.style_ one by one
  // I didn't find a workaround solution except this :(
  Column(const Column& other)
    : lines_(other.lines_),
    dirty_(other.dirty_),
    config_(dirty_), // CRUCIAL
    style_(dirty_), // CRUCIAL
    content_(other.content_)
  {
    config_.align_ = other.config_.align_;
    config_.padd_ = other.config_.padd_;
    config_.delimiter_ = other.config_.delimiter_;
    config_.width_ = other.config_.width_;
    config_.skipEmptyLineIndent_ = other.config_.skipEmptyLineIndent_;

    style_.fg_ = other.style_.fg_;
    style_.bg_ = other.style_.bg_;
    style_.base_ = other.style_.base_;
    style_.attrs_ = other.style_.attrs_;
  }

  void content(std::string content)
  {
    content_ = std::move(content);
    dirty_ = true;
  }

  Config& config() { return config_; }
  Style& style() { return style_; }

  const Config& config() const { return config_; }
  const Style& style() const { return style_; }

  const std::string& content() const { return content_; }
  std::string& content()
  {
    dirty_ = true;
    return content_;
  }

  explicit operator const std::string&() const
  {
    return content_;
  }
  explicit operator std::string&()
  {
    dirty_ = true;
    return content_;
  }

  char& operator[](int index)
  {
    dirty_ = true;
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
    dirty_ = false;
  }

  const std::vector<std::string>& lines() const
  {
    if (dirty_)
    {
      lines_ = genLines();
      dirty_ = false;
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
  // cache
  mutable std::vector<std::string> lines_;
  mutable bool dirty_ = false;

  Config config_{dirty_};
  Style style_{dirty_};
  std::string content_;

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
