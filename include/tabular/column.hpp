/*
      __        ___.         .__
    _/  |______ \_ |__  __ __|  | _____ _______
    \   __\__  \ | __ \|  |  \  | \__  \\_  __ \
     |  |  / __ \| \_\ \  |  /  |__/ __ \|  | \/
     |__| (____  /___  /____/|____(____  /__|
               \/    \/                \/

    *  Author: Anas Hamdane
    *  Github: https://github.com/Anas-Hamdane

*/

#ifndef TABULAR_COLUMN_HPP
#define TABULAR_COLUMN_HPP

#include <list>
#include <string>
#include <vector>

#include <tabular/alignment.hpp>
#include <tabular/colors.hpp>
#include <tabular/global.hpp>
#include <tabular/rgb.hpp>
#include <tabular/styles.hpp>

namespace tabular {
  class Column {
    // styles applied to the column content
    // If empty, start with ANSI CSI + style + ";",
    // else just append style + ";".
    // later add "m".
    std::string text_attribute;

    // column content coloring
    std::string content_color;
    std::string content_background_color;

    // column coloring
    std::string column_background_color;

    std::vector<std::string> lines;
    std::list<std::string> words;

    Alignment alignment;

    unsigned int width;

    unsigned int top_padding;
    unsigned int bottom_padding;

    bool multi_byte_characters;
    bool disabled_styles;

    size_t special_characters;

    class Config {
      Column& column;

  public:
      Config(Column& column) : column(column) {}

      Config& alignment(Alignment align) {
        column.alignment = align;

        return *this;
      }

      Config& width(int w) {
        if (w <= 0)
          w = 0;

        column.width = static_cast<unsigned int>(w);

        return *this;
      }

      Config& padding(int padd) {
        if (padd <= 0)
          padd = 0;

        column.top_padding = padd;
        column.bottom_padding = padd;

        return *this;
      }

      Config& top_padding(int padd) {
        if (padd <= 0)
          column.top_padding = 0;
        else
          column.top_padding = static_cast<unsigned int>(padd);

        return *this;
      }

      Config& bottom_padding(int padd) {
        if (padd <= 0)
          column.bottom_padding = 0;
        else
          column.bottom_padding = static_cast<unsigned int>(padd);

        return *this;
      }

      // add text style to the whole column
      Config& text_attribute(const Attribute& attribute) {
        if (column.text_attribute.empty())
          column.text_attribute.append(ansi::CSI);

        column.text_attribute += std::to_string(static_cast<int>(attribute)) + ";";

        return *this;
      }

      // add multiple text styles to the whole column
      Config& text_attribute(const std::vector<Attribute>& attribute) {
        if (attribute.empty()) return *this;

        if (column.text_attribute.empty())
          column.text_attribute.append(ansi::CSI);

        // Apply all styles at the beginning
        for (const auto& style : attribute)
          column.text_attribute += std::to_string(static_cast<int>(style)) + ";";

        return *this;
      }

      // add Background Color to the whole column
      Config& content_background_color(const Color& color) {
        // one color could be applied
        column.content_background_color = std::string(ansi::CSI) + std::to_string(static_cast<int>(color) + 10) + "m";

        return *this;
      }

      // add background RGB to the whole column
      Config& content_background_color(const RGB& rgb) {
        // one color could be applied
        column.content_background_color = std::string(ansi::CSI) + "48;2;" + std::to_string(rgb.r) + ";" +
                                          std::to_string(rgb.g) + ";" +
                                          std::to_string(rgb.b) + "m";

        return *this;
      }

      // add Color to the whole column
      Config& color(const Color& color) {
        column.content_color = std::string(ansi::CSI) + std::to_string(static_cast<int>(color)) + "m";

        return *this;
      }

      // add RGB to the whole column
      Config& color(const RGB& rgb) {
        column.content_color = std::string(ansi::CSI) + "38;2;" + std::to_string(rgb.r) + ";" +
                               std::to_string(rgb.g) + ";" +
                               std::to_string(rgb.b) + "m";

        return *this;
      }

      // column background coloring
      Config& column_background_color(const Color& color) {
        column.column_background_color = std::string(ansi::CSI) + std::to_string(static_cast<int>(color) + 10) + "m";

        return *this;
      }

      Config& column_background_color(const RGB& rgb) {
        column.column_background_color = std::string(ansi::CSI) + "48;2;" + std::to_string(rgb.r) + ";" +
                                         std::to_string(rgb.g) + ";" +
                                         std::to_string(rgb.b) + "m";

        return *this;
      }
    };

    class Getters {
      const Column& column;

  public:
      Getters(const Column& column) : column(column) {}

      Alignment alignment() const { return column.alignment; }

      unsigned int width() const { return column.width; }

      unsigned int top_padding() const { return column.top_padding; }

      unsigned int bottom_padding() const { return column.bottom_padding; }

      std::vector<std::string> lines() const { return column.lines; }

      std::list<std::string> words() const { return column.words; }

      std::string text_attributes() const { return column.text_attribute; }

      std::string content_color() const { return column.content_color; }

      std::string content_background_color() const { return column.content_background_color; }

      std::string column_background_color() const { return column.column_background_color; }

      bool multi_byte_characters() const { return column.multi_byte_characters; }

      bool disabled_styles() const { return column.disabled_styles; }

      size_t special_characters() const { return column.special_characters; }
    };

    class Setters {
      Column& column;

  public:
      Setters(Column& column) : column(column) {}

      Setters& lines(std::vector<std::string> splittedContent) {
        column.lines = splittedContent;
        return *this;
      }

      Setters& words(std::list<std::string> words) {
        column.words = words;
        return *this;
      }

      Setters& width(int width) {
        if (width <= 0)
          width = 0;

        column.width = static_cast<unsigned int>(width);
        return *this;
      }

      // multi byte characters support
      // (locale-independent but only utf8 encoding is supported)
      Setters& multi_byte_characters(bool is_multi_byte) {
        column.multi_byte_characters = is_multi_byte;
        return *this;
      }

      Setters& disabled_styles(bool is_disabled) {
        column.disabled_styles = is_disabled;
        return *this;
      }

      Setters& special_characters(size_t special_characters) {
        column.special_characters = special_characters;
        return *this;
      }
    };

public:
    std::string content;

    Column(std::string content)
        : content(std::move(content)),
          alignment(Alignment::left),
          width(0),
          top_padding(0),
          bottom_padding(0),
          special_characters(0),
          multi_byte_characters(false),
          disabled_styles(false) {}

    Config config() { return Config(*this); }

    Getters get() const { return Getters(*this); }

    Setters set() { return Setters(*this); }
  };
} // namespace tabular

#endif // TABULAR_COLUMN_HPP
