#include "tabular/Column.h"
#include <stdexcept>

namespace tabular {
  static bool isspace(char c)
  {
      return c == ' ' || c == '\n' || c == '\t' ||
            c == '\v' || c == '\f' || c == '\r';
  }
  // split a string into an array of words, treating space characters
  // as separated words
  static std::vector<std::string> split(const std::string content)
  {
    std::vector<std::string> result;

    size_t start = 0;
    size_t len = content.length();

    for (size_t i = 0; i < len; ++i)
    {
      char c = content[i];

      if (isspace(c))
      {
        if (i > start)
        {
          result.emplace_back(content.begin() + start, content.begin() + i);
        }

        result.emplace_back(1, c);
        start = i + 1;
      }
    }

    if (start < len)
    {
      result.emplace_back(content.begin() + start, content.end());
    }

    return result;
  }
  // read the ne utf8 character into a std::string of bytes, starting from
  // the specified position `pos`
  static std::string readUtf8Char(const std::string& str, size_t pos)
  {
    if (pos >= str.length()) return "";

    unsigned char first_byte = static_cast<unsigned char>(str[pos]);

    // if it's a continuation byte then it's invalid
    if ((first_byte & 0xC0) == 0x80) return "";

    // find the length of the sequence from the start byte
    size_t len;
    if ((first_byte & 0x80) == 0) len = 1;
    else if ((first_byte & 0xE0) == 0xC0) len = 2;
    else if ((first_byte & 0xF0) == 0xE0) len = 3;
    else if ((first_byte & 0xF8) == 0xF0) len = 4;
    else return "";

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

  Column::Column(std::string content)
    : content(std::move(content)) {}
  Column::Column() = default;

  std::string Column::getContent() const { return this->content; }

  Config& Column::getConfig() { return this->config; }
  const Config& Column::getConfig() const { return this->config; }

  void Column::setContent(std::string content) { this->content = std::move(content); }
  void Column::setConfig(Config config) { this->config = std::move(config); }

  // the output represent a vector of lines
  std::vector<String> Column::toString() const
  {
    if (this->config.getWidth() == -1)
      throw std::logic_error("Internal error: uninitialized column width");

    // make sure it is a copy
    std::string content = this->content;

    // handling attributes, fg, bg stuff
    std::string base = getBaseFormat();
    std::string styles = base;
    {
      std::vector<uint8_t> codes;
      auto temp = getAttrsCodes();
      codes.insert(codes.end(), temp.begin(), temp.end());

      temp = getFGCode();
      codes.insert(codes.end(), temp.begin(), temp.end());

      temp = getBGCode();
      codes.insert(codes.end(), temp.begin(), temp.end());

      if (!codes.empty())
      {
        styles += "\033[";
        for (size_t i = 0; i < codes.size(); ++i)
        {
          if (i != 0)
            styles += ';';

          styles += std::to_string(codes[i]);
        }
        styles += 'm';
      }
    }
    std::string reset = styles.empty() ? "" : "\033[0m";

    size_t width = getConfig().getWidth();
    Padding padd = getConfig().getPadd();
    std::vector<std::string> words = split(content);

    // the result
    std::vector<String> lines;
    lines.reserve(this->content.size() / width);

    // the base must be applied to the padding
    String empty(std::string(width, ' '));
    if (!base.empty())
    {
      empty.insert(0, getBaseFormat());
      empty += "\033[0m";
    }

    // top padding
    lines.insert(lines.end(), padd.top, empty);

    // the main logic
    size_t pos = 0;
    String next("");
    while (pos < words.size())
    {
      String line = processLine(words, pos, next);

      // the rest are empty spaces
      if (line.getVisibleWidth() < width)
      {
        size_t spaces = width - line.getVisibleWidth();
        Alignment align = getConfig().getAlign();
        if (align == Alignment::Center)
        {
          size_t left = spaces / 2;
          line.insert(0, std::string(spaces / 2, ' '));
          line += std::string(spaces - left, ' ');
        }
        else if (align == Alignment::Right)
        {
          line.insert(0, std::string(spaces, ' '));
        }
        else
        {
          line += std::string(spaces, ' ');
        }
      }

      line.insert(0, styles);
      line += reset;

      lines.push_back(std::move(line));
    }

    // bottom padding
    lines.insert(lines.end(), padd.bottom, empty);
    return lines;
  }

  // TODO: make const auto& overloads to avoid unecessary copying
  std::vector<uint8_t> Column::getAttrsCodes() const
  {
    const auto attrs = getConfig().getAttrs();
    std::vector<uint8_t> codes;
    codes.reserve(attrs.size());

    for (const auto& attr : attrs)
      codes.push_back(static_cast<uint8_t>(attr));

    return codes;
  }
  std::vector<uint8_t> Column::getFGCode() const
  {
    const auto fg = getConfig().getFG();

    // the default constructor
    if (fg.getIndex() == 0 && fg.getBuiltin() == Color::Builtin::Normal)
      return {};

    return fg.getCode();
  }
  std::vector<uint8_t> Column::getBGCode() const
  {
    const auto bg = getConfig().getBG();

    // the default constructor
    if (bg.getIndex() == 0 && bg.getBuiltin() == Color::Builtin::Normal)
      return {};

    return bg.getBackCode();
  }
  std::string Column::getBaseFormat() const
  {
    const auto base = getConfig().getBase();

    if (base.getIndex() == 0 && base.getBuiltin() == Color::Builtin::Normal)
      return "";

    std::string format = "\033[";
    const auto codes = base.getBackCode();

    for (size_t i = 0; i < codes.size(); ++i)
    {
      if (i != 0)
        format += ';';

      format += std::to_string(codes[i]);
    }

    format += 'm';
    return format;
  }
  String Column::processLine(std::vector<std::string>& words, size_t& pos, String& next) const
  {
    size_t width = getConfig().getWidth();

    String line;
    line.reserve(width);

    size_t i = pos;
    while (i < words.size())
    {
      String word(words[i]);
      if (!next.empty())
      {
        word = next; // process this word instead
        next = "";
      }
      size_t len = word.getVisibleWidth();

      if (word == "\n")
      {
        next = "";
        pos = i;
        return line;
      }

      // ignore spaces at the start of a new line
      if (line.empty() && word == " ")
      {
        ++i; continue;
      }

      // other spacing characters rather than '\n' and ' '
      // are ignored
      if (len == 1 && isspace(word[0]) && word != " " && word != "\n")
      {
        ++i; continue;
      }

      // the word fits in the line
      if (len + line.getVisibleWidth() <= width)
      {
        line += word;
        ++i; continue;
      }

      // the word doesn't fit in the line
      else // if (len + line.getVisibleWidth() > width)
      {
        size_t freeSpace = width - line.getVisibleWidth();

        // push the current line and append the word
        // on the next line
        if (freeSpace <= (width * 0.25))
        {
          pos = i;
          next = word;
          return line;
        }

        // we need to force cut the word
        else // if (len + freeSpace > (width * 0.25))
        {
          String delimiter = getConfig().getDelimiter();
          size_t limit = freeSpace - delimiter.getVisibleWidth();

          String firstPart;
          size_t j = 0;
          while (firstPart.getVisibleWidth() < limit)
          {
            String buff(readUtf8Char(word.getContent(), j));

            // if it will exceed the free space
            if (buff.getVisibleWidth() + firstPart.getVisibleWidth() > limit)
              break;

            firstPart += buff;
            j += buff.len();
          }

          line += firstPart;
          line += delimiter;

          String secondPart(word.getContent().substr(j));
          pos = i;
          next = secondPart;
          return line;
        }
      }
    }

    pos = i;
    return line;
  }
}
