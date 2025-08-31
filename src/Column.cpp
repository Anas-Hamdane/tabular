#include "../include/Column.h"
#include <stdexcept>

namespace tabular {
  static std::vector<std::string> split(const std::string content)
  {
    std::vector<std::string> result;

    auto isspace = [](char c) {
      return c == ' ' || c == '\n' || c == '\t' ||
            c == '\v' || c == '\f' || c == '\r';
    };

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
        std::string styles = "\033[";
        for (size_t i = 0; i < codes.size(); ++i)
        {
          if (i != 0)
            styles += ';';

          styles += std::to_string(codes[i]);
        }
        styles += 'm';

        content.insert(0, styles);
        content += "\033[0m"; // reset them
      }
    }

    std::vector<std::string> words = split(content);

    // the result
    std::vector<String> lines;



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
    const auto codes = base.getCode();

    for (size_t i = 0; i < codes.size(); ++i)
    {
      if (i != 0)
        format += ';';

      format += std::to_string(codes[i]);
    }

    format += 'm';
    return format;
  }
}
