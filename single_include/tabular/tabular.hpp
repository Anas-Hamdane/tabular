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

#include <array>
#include <climits>
#include <cstdint>
#include <list>
#include <string.h>
#include <string>
#include <unordered_set>
#include <vector>

#define TABULAR_VERSION_MAJOR 1
#define TABULAR_VERSION_MINOR 0
#define TABULAR_VERSION_PATCH 1
#define TABULAR_VERSION "1.0.1"

#if defined(_WIN32) || defined(_WIN64)
#define WINDOWS 1
#include <io.h> // for _isatty
#include <windows.h>
#undef RGB

#elif defined(__unix__) || defined(__unix) || defined(__APPLE__) || defined(__linux__)
#define UNIX_LIKE 1
#include <sys/ioctl.h>
#include <unistd.h>

#else
#error unsupported platform
#endif

namespace tabular {
  enum class Alignment {
    left,
    center,
    right
  };

  enum class BorderStyle : size_t {
    empty = 0,
    standard,
    space,
    ansi
  };

  enum class Font {
    bold = 1,
    dim,
    italic,
    underline,
    blink, // rarely supported
    reverse = 7,
    concealed,
    crossed
  };

  enum class Color {
    black = 30,
    red,
    green,
    yellow,
    blue,
    magenta,
    cyan,
    white,
    normal = 39,
  };

  enum class Attribute {
    bold = 1,
    dim = 2,
    italic = 3,

    underline = 4,
    double_underline = 21, // not popular

    blink = 5,      // rarely supported
    fast_blink = 6, // rarely supported
    reverse = 7,
    concealed = 8,
    crossed = 9,
  };

  enum class STD {
    Out,
    Error
  };

  struct BorderGlyphs {
    std::string vertical;
    std::string horizontal;

    std::string bottom_right_corner;
    std::string top_right_corner;
    std::string top_left_corner;
    std::string bottom_left_corner;

    std::string intersection; // +

    std::string left_connector;
    std::string right_connector;
    std::string bottom_connector;
    std::string top_connector;
  };

  struct RGB {
    uint8_t r, g, b;

    RGB(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}
  };

  struct ColumnLines {
    std::string line;
    size_t display_width;

    ColumnLines(std::string line, size_t width) : line(std::move(line)), display_width(width) {}
  };

  namespace ansi {
    // table ANSI printing
    constexpr const char* TABLE_MODE = "\x1b(0";
    constexpr const char* RESET_TABLE_MODE = "\x1B(B";

    // Control sequence introducer (for font styles and colors)
    constexpr const char* CSI = "\x1b[";
    constexpr const char* RESET = "\x1b[0m";

    // end of an ansi sequence
    constexpr const char suffix = 'm';

    constexpr const char* FG_COLOR_RESET = "\x1b[39m";
    constexpr const char* BG_COLOR_RESET = "\x1b[49m";
  } // namespace ansi

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

    std::vector<ColumnLines> lines;

    std::list<std::string> words;

    Alignment alignment;

    unsigned int width;

    unsigned int top_padding;
    unsigned int bottom_padding;

    bool multi_byte_characters;
    bool disabled_styles;

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
    };

    class Style {
      Column& column;

  public:
      Style(Column& column) : column(column) {}

      Style& text_attribute(const Attribute& attribute) {
        if (column.text_attribute.empty())
          column.text_attribute.append(ansi::CSI);

        column.text_attribute += std::to_string(static_cast<int>(attribute)) + ";";

        return *this;
      }

      Style& text_attribute(const std::vector<Attribute>& attribute) {
        if (attribute.empty()) return *this;

        if (column.text_attribute.empty())
          column.text_attribute.append(ansi::CSI);

        // Apply all styles at the beginning
        for (const auto& style : attribute)
          column.text_attribute += std::to_string(static_cast<int>(style)) + ";";

        return *this;
      }

      Style& content_color(const Color& color) {
        column.content_color = std::string(ansi::CSI) + std::to_string(static_cast<int>(color)) + "m";

        return *this;
      }

      Style& content_color(const RGB& rgb) {
        column.content_color = std::string(ansi::CSI) + "38;2;" + std::to_string(rgb.r) + ";" +
                               std::to_string(rgb.g) + ";" +
                               std::to_string(rgb.b) + "m";

        return *this;
      }

      Style& content_background_color(const Color& color) {
        // one color could be applied
        column.content_background_color = std::string(ansi::CSI) + std::to_string(static_cast<int>(color) + 10) + "m";

        return *this;
      }

      Style& content_background_color(const RGB& rgb) {
        // one color could be applied
        column.content_background_color = std::string(ansi::CSI) + "48;2;" + std::to_string(rgb.r) + ";" +
                                          std::to_string(rgb.g) + ";" +
                                          std::to_string(rgb.b) + "m";

        return *this;
      }

      Style& column_background_color(const Color& color) {
        column.column_background_color = std::string(ansi::CSI) + std::to_string(static_cast<int>(color) + 10) + "m";

        return *this;
      }

      Style& column_background_color(const RGB& rgb) {
        column.column_background_color = std::string(ansi::CSI) + "48;2;" + std::to_string(rgb.r) + ";" +
                                         std::to_string(rgb.g) + ";" +
                                         std::to_string(rgb.b) + "m";

        return *this;
      }
    };

    class Setters {
      Column& column;

  public:
      Setters(Column& column) : column(column) {}

      Setters& lines(const std::vector<ColumnLines>& lines) {
        column.lines = lines;
        return *this;
      }

      Setters& words(const std::list<std::string>& words) {
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
    };

    class Getters {
      const Column& column;

  public:
      Getters(const Column& column) : column(column) {}

      Alignment alignment() const { return column.alignment; }

      unsigned int width() const { return column.width; }

      unsigned int top_padding() const { return column.top_padding; }

      unsigned int bottom_padding() const { return column.bottom_padding; }

      std::vector<ColumnLines> lines() const { return column.lines; }

      std::list<std::string> words() const { return column.words; }

      std::string text_attributes() const { return column.text_attribute; }

      std::string content_color() const { return column.content_color; }

      std::string content_background_color() const { return column.content_background_color; }

      std::string column_background_color() const { return column.column_background_color; }

      bool multi_byte_characters() const { return column.multi_byte_characters; }

      bool disabled_styles() const { return column.disabled_styles; }
    };

public:
    std::string content;

    Column(std::string content)
        : content(std::move(content)),
          alignment(Alignment::left),
          width(0),
          top_padding(0),
          bottom_padding(0),
          multi_byte_characters(false),
          disabled_styles(false) {}

    Config config() { return Config(*this); }

    Style style() { return Style(*this); }

    Setters set() { return Setters(*this); }

    Getters get() const { return Getters(*this); }
  };

  class Row {

public:
    std::vector<Column> columns;

    Row(std::vector<Column>&& columns)
        : columns(std::move(columns)) {}

    Column& operator[](int index) {
      return this->columns.at(index);
    }
  };

  class Border {
    BorderGlyphs glyphs;
    BorderStyle style;

    BorderGlyphs colors;
    BorderGlyphs background_colors;

    bool is_multi_byte;
    bool disabled_styles;

    mutable BorderGlyphs cached_glyphs;
    mutable bool valid_cache;

    static std::string solve_color(Color color, bool is_background) {
      int code = static_cast<int>(color) + (is_background ? 10 : 0);
      return ansi::CSI + std::to_string(code) + ansi::suffix;
    }

    static std::string solve_color(RGB rgb, bool is_background) {
      std::string result;
      result.reserve(20);

      result.append(ansi::CSI);

      result.append(is_background ? "48;2;" : "38;2;");

      result.append(std::to_string(rgb.r) + ";" +
                    std::to_string(rgb.g) + ";" +
                    std::to_string(rgb.b) + "m");
      return result;
    }

    void invalidate_cache() {
      if (this->valid_cache)
        this->valid_cache = false;
    }

    class Getters {
      Border& border;

  public:
      Getters(Border& border) : border(border) {}

      static const BorderGlyphs& style_templates(BorderStyle style) {

        // clang-format off
        static const std::array<BorderGlyphs, 4> templates = {{
          {"", "", "", "", "", "", "", "", "", "", ""},
          {"|", "-", "+", "+", "+", "+", "+", "+", "+", "+", "+"},
          {" ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "},
          {
            std::string(ansi::TABLE_MODE) + "x" + ansi::RESET_TABLE_MODE, // vertical
            std::string(ansi::TABLE_MODE) + "q" + ansi::RESET_TABLE_MODE, // horizontal
            std::string(ansi::TABLE_MODE) + "j" + ansi::RESET_TABLE_MODE, // bottom_right_corner
            std::string(ansi::TABLE_MODE) + "k" + ansi::RESET_TABLE_MODE, // top_right_corner
            std::string(ansi::TABLE_MODE) + "l" + ansi::RESET_TABLE_MODE, // top_left_corner
            std::string(ansi::TABLE_MODE) + "m" + ansi::RESET_TABLE_MODE, // bottom_left_corner
            std::string(ansi::TABLE_MODE) + "n" + ansi::RESET_TABLE_MODE, // intersection
            std::string(ansi::TABLE_MODE) + "t" + ansi::RESET_TABLE_MODE, // left_connector
            std::string(ansi::TABLE_MODE) + "u" + ansi::RESET_TABLE_MODE, // right_connector
            std::string(ansi::TABLE_MODE) + "v" + ansi::RESET_TABLE_MODE, // bottom_connector
            std::string(ansi::TABLE_MODE) + "w" + ansi::RESET_TABLE_MODE  // top_connector
          }}};
        // clang-format on

        return templates[static_cast<size_t>(style)];
      }

      BorderStyle style() const { return border.style; }

      BorderGlyphs processed_glyphs() const {
        if (border.valid_cache)
          return border.cached_glyphs;

        const BorderGlyphs& def = style_templates(border.style);
        BorderGlyphs result;

        auto pick = [](const std::string& val, const std::string& fallback) -> std::string {
          return val.empty() ? fallback : val;
        };

        auto wrap = [](const std::string& fg, const std::string& bg,
                       const std::string& val, const Border& border) -> std::string {
          if ((fg.empty() && bg.empty()) || border.disabled_styles) return val;
          return fg + bg + val + ansi::RESET;
        };

        result.vertical = wrap(border.colors.vertical,
                               border.background_colors.vertical,
                               pick(border.glyphs.vertical, def.vertical),
                               border);

        result.horizontal = wrap(border.colors.horizontal,
                                 border.background_colors.horizontal,
                                 pick(border.glyphs.horizontal, def.horizontal),
                                 border);

        result.bottom_right_corner = wrap(border.colors.bottom_right_corner,
                                          border.background_colors.bottom_right_corner,
                                          pick(border.glyphs.bottom_right_corner, def.bottom_right_corner),
                                          border);

        result.top_right_corner = wrap(border.colors.top_right_corner,
                                       border.background_colors.top_right_corner,
                                       pick(border.glyphs.top_right_corner, def.top_right_corner),
                                       border);

        result.top_left_corner = wrap(border.colors.top_left_corner,
                                      border.background_colors.top_left_corner,
                                      pick(border.glyphs.top_left_corner, def.top_left_corner),
                                      border);

        result.bottom_left_corner = wrap(border.colors.bottom_left_corner,
                                         border.background_colors.bottom_left_corner,
                                         pick(border.glyphs.bottom_left_corner, def.bottom_left_corner),
                                         border);

        result.intersection = wrap(border.colors.intersection,
                                   border.background_colors.intersection,
                                   pick(border.glyphs.intersection, def.intersection),
                                   border);

        result.left_connector = wrap(border.colors.left_connector,
                                     border.background_colors.left_connector,
                                     pick(border.glyphs.left_connector, def.left_connector),
                                     border);

        result.right_connector = wrap(border.colors.right_connector,
                                      border.background_colors.right_connector,
                                      pick(border.glyphs.right_connector, def.right_connector),
                                      border);

        result.bottom_connector = wrap(border.colors.bottom_connector,
                                       border.background_colors.bottom_connector,
                                       pick(border.glyphs.bottom_connector, def.bottom_connector),
                                       border);

        result.top_connector = wrap(border.colors.top_connector,
                                    border.background_colors.top_connector,
                                    pick(border.glyphs.top_connector, def.top_connector),
                                    border);

        border.cached_glyphs = std::move(result);
        border.valid_cache = true;
        return border.cached_glyphs;
      }

      BorderGlyphs glyphs() const {
        const BorderGlyphs& def = style_templates(border.style);
        BorderGlyphs result;

        auto pick = [](const std::string& val, const std::string& fallback) -> std::string {
          return val.empty() ? fallback : val;
        };

        result.vertical = pick(border.glyphs.vertical, def.vertical);
        result.horizontal = pick(border.glyphs.horizontal, def.horizontal);
        result.bottom_right_corner = pick(border.glyphs.bottom_right_corner, def.bottom_right_corner);
        result.top_right_corner = pick(border.glyphs.top_right_corner, def.top_right_corner);
        result.top_left_corner = pick(border.glyphs.top_left_corner, def.top_left_corner);
        result.bottom_left_corner = pick(border.glyphs.bottom_left_corner, def.bottom_left_corner);
        result.intersection = pick(border.glyphs.intersection, def.intersection);
        result.left_connector = pick(border.glyphs.left_connector, def.left_connector);
        result.right_connector = pick(border.glyphs.right_connector, def.right_connector);
        result.bottom_connector = pick(border.glyphs.bottom_connector, def.bottom_connector);
        result.top_connector = pick(border.glyphs.top_connector, def.top_connector);

        return result;
      }

      std::string horizontal() const { return border.glyphs.horizontal; }

      std::string vertical() const { return border.glyphs.vertical; }

      std::string top_left_corner() const { return border.glyphs.top_left_corner; }

      std::string top_right_corner() const { return border.glyphs.top_right_corner; }

      std::string bottom_left_corner() const { return border.glyphs.bottom_left_corner; }

      std::string bottom_right_corner() const { return border.glyphs.bottom_right_corner; }

      std::string intersection() const { return border.glyphs.intersection; }

      std::string left_connector() const { return border.glyphs.left_connector; }

      std::string right_connector() const { return border.glyphs.right_connector; }

      std::string top_connector() const { return border.glyphs.top_connector; }

      std::string bottom_connector() const { return border.glyphs.bottom_connector; }

      bool disabled_styles() const { return border.disabled_styles; }
    };

    class Parts {
      Border& border;

      template <typename T>
      Parts& set_glyph(std::string BorderGlyphs::* member, T&& value) {
        (border.glyphs.*member) = std::forward<T>(value);
        border.invalidate_cache();
        return *this;
      }

  public:
      Parts(Border& border) : border(border) {}

      /*
       * Note: It is the user's responsibility to ensure that each border glyph is a single
       * terminal column wide. If a string with a visual width > 1 is provided, the table
       * layout may break.
       *
       * Using str.length() is unreliable for multi-byte characters,
       * and calling utils::mbswidth() on every assignment would
       * introduce unnecessary performance overhead.
       *
       * — Anas Hamdane, 2025-06-14
       */

      Parts& horizontal(std::string horizontal) {
        return set_glyph(&BorderGlyphs::horizontal, std::move(horizontal));
      }

      Parts& vertical(std::string vertical) {
        return set_glyph(&BorderGlyphs::vertical, std::move(vertical));
      }

      Parts& top_left_corner(std::string corner) {
        return set_glyph(&BorderGlyphs::top_left_corner, std::move(corner));
      }

      Parts& top_right_corner(std::string corner) {
        return set_glyph(&BorderGlyphs::top_right_corner, std::move(corner));
      }

      Parts& bottom_left_corner(std::string corner) {
        return set_glyph(&BorderGlyphs::bottom_left_corner, std::move(corner));
      }

      Parts& bottom_right_corner(std::string corner) {
        return set_glyph(&BorderGlyphs::bottom_right_corner, std::move(corner));
      }

      Parts& intersection(std::string intersection) {
        return set_glyph(&BorderGlyphs::intersection, std::move(intersection));
      }

      Parts& left_connector(std::string connector) {
        return set_glyph(&BorderGlyphs::left_connector, std::move(connector));
      }

      Parts& right_connector(std::string connector) {
        return set_glyph(&BorderGlyphs::right_connector, std::move(connector));
      }

      Parts& top_connector(std::string connector) {
        return set_glyph(&BorderGlyphs::top_connector, std::move(connector));
      }

      Parts& bottom_connector(std::string connector) {
        return set_glyph(&BorderGlyphs::bottom_connector, std::move(connector));
      }
    };

    class Setters {
      Border& border;

  public:
      Setters(Border& border) : border(border) {}

      Setters& disabled_styles(bool is_disabled) {
        border.disabled_styles = is_disabled;
        border.invalidate_cache();
        return *this;
      }

      Setters& style(BorderStyle style) {
        border.style = style;
        border.invalidate_cache();
        return *this;
      }
    };

    class Coloring {
      Border& border;

  public:
      Coloring(Border& border) : border(border) {}

      /* ------------------ full ------------------------ */
      Coloring& full(Color color) {
        horizontal(color);
        vertical(color);

        return *this;
      }

      Coloring& full(RGB rgb) {
        horizontal(rgb);
        vertical(rgb);

        return *this;
      }

      /* ------------------ horizontal ------------------------ */
      Coloring& horizontal(Color color) {
        border.colors.horizontal = border.solve_color(color, false);

        top_connector(color);
        bottom_connector(color);
        intersection(color);

        corners(color);

        return *this;
      }

      Coloring& horizontal(RGB rgb) {
        border.colors.horizontal = border.solve_color(rgb, false);

        top_connector(rgb);
        bottom_connector(rgb);
        intersection(rgb);

        corners(rgb);

        return *this;
      }

      Coloring& bottom_connector(Color color) {
        border.colors.bottom_connector = border.solve_color(color, false);

        border.invalidate_cache();
        return *this;
      }

      Coloring& bottom_connector(RGB rgb) {
        border.colors.bottom_connector = border.solve_color(rgb, false);

        border.invalidate_cache();
        return *this;
      }

      Coloring& top_connector(Color color) {
        border.colors.top_connector = border.solve_color(color, false);

        border.invalidate_cache();
        return *this;
      }

      Coloring& top_connector(RGB rgb) {
        border.colors.top_connector = border.solve_color(rgb, false);

        border.invalidate_cache();
        return *this;
      }

      /* ------------------ vertical ------------------------ */
      Coloring& vertical(Color color) {
        border.colors.vertical = border.solve_color(color, false);

        left_connector(color);
        right_connector(color);

        corners(color);

        border.invalidate_cache();
        return *this;
      }

      Coloring& vertical(RGB rgb) {
        border.colors.vertical = border.solve_color(rgb, false);

        left_connector(rgb);
        right_connector(rgb);

        corners(rgb);

        border.invalidate_cache();
        return *this;
      }

      Coloring& left_connector(Color color) {
        border.colors.left_connector = border.solve_color(color, false);

        border.invalidate_cache();
        return *this;
      }

      Coloring& left_connector(RGB rgb) {
        border.colors.left_connector = border.solve_color(rgb, false);

        border.invalidate_cache();
        return *this;
      }

      Coloring& right_connector(Color color) {
        border.colors.right_connector = border.solve_color(color, false);

        border.invalidate_cache();
        return *this;
      }

      Coloring& right_connector(RGB rgb) {
        border.colors.right_connector = border.solve_color(rgb, false);

        border.invalidate_cache();
        return *this;
      }

      /* ------------------ corners ------------------------ */
      Coloring& corners(Color color) {
        top_left_corner(color);
        top_right_corner(color);

        bottom_left_corner(color);
        bottom_right_corner(color);

        return *this;
      }

      Coloring& corners(RGB rgb) {
        top_left_corner(rgb);
        top_right_corner(rgb);

        bottom_left_corner(rgb);
        bottom_right_corner(rgb);

        return *this;
      }

      Coloring& top_left_corner(Color color) {
        border.colors.top_left_corner = border.solve_color(color, false);

        border.invalidate_cache();
        return *this;
      }

      Coloring& top_left_corner(RGB rgb) {
        border.colors.top_left_corner = border.solve_color(rgb, false);

        border.invalidate_cache();
        return *this;
      }

      Coloring& top_right_corner(Color color) {
        border.colors.top_right_corner = border.solve_color(color, false);

        border.invalidate_cache();
        return *this;
      }

      Coloring& top_right_corner(RGB rgb) {
        border.colors.top_right_corner = border.solve_color(rgb, false);

        border.invalidate_cache();
        return *this;
      }

      Coloring& bottom_left_corner(Color color) {
        border.colors.bottom_left_corner = border.solve_color(color, false);

        border.invalidate_cache();
        return *this;
      }

      Coloring& bottom_left_corner(RGB rgb) {
        border.colors.bottom_left_corner = border.solve_color(rgb, false);

        border.invalidate_cache();
        return *this;
      }

      Coloring& bottom_right_corner(Color color) {
        border.colors.bottom_right_corner = border.solve_color(color, false);

        border.invalidate_cache();
        return *this;
      }

      Coloring& bottom_right_corner(RGB rgb) {
        border.colors.bottom_right_corner = border.solve_color(rgb, false);

        border.invalidate_cache();
        return *this;
      }

      /* ------------------ intersection point ------------------------ */
      Coloring& intersection(Color color) {
        border.colors.intersection = border.solve_color(color, false);

        border.invalidate_cache();
        return *this;
      }

      Coloring& intersection(RGB rgb) {
        border.colors.intersection = border.solve_color(rgb, false);

        border.invalidate_cache();
        return *this;
      }
    };

    class BackgroundColoring {
      Border& border;

      void invalidate_cache() {
        if (border.valid_cache)
          border.valid_cache = false;
      }

  public:
      BackgroundColoring(Border& border) : border(border) {}

      /* ------------------ full ------------------------ */
      BackgroundColoring& full(Color color) {
        horizontal(color);
        vertical(color);

        return *this;
      }

      BackgroundColoring& full(RGB rgb) {
        horizontal(rgb);
        vertical(rgb);

        return *this;
      }

      /* ------------------ horizontal ------------------------ */
      BackgroundColoring& horizontal(Color color) {
        border.background_colors.horizontal = border.solve_color(color, true);

        top_connector(color);
        bottom_connector(color);
        intersection(color);

        corners(color);

        return *this;
      }

      BackgroundColoring& horizontal(RGB rgb) {
        border.background_colors.horizontal = border.solve_color(rgb, true);

        top_connector(rgb);
        bottom_connector(rgb);
        intersection(rgb);

        corners(rgb);

        return *this;
      }

      BackgroundColoring& bottom_connector(Color color) {
        border.background_colors.bottom_connector = border.solve_color(color, true);

        border.invalidate_cache();
        return *this;
      }

      BackgroundColoring& bottom_connector(RGB rgb) {
        border.background_colors.bottom_connector = border.solve_color(rgb, true);

        border.invalidate_cache();
        return *this;
      }

      BackgroundColoring& top_connector(Color color) {
        border.background_colors.top_connector = border.solve_color(color, true);

        border.invalidate_cache();
        return *this;
      }

      BackgroundColoring& top_connector(RGB rgb) {
        border.background_colors.top_connector = border.solve_color(rgb, true);

        border.invalidate_cache();
        return *this;
      }

      /* ------------------ vertical ------------------------ */
      BackgroundColoring& vertical(Color color) {
        border.background_colors.vertical = border.solve_color(color, true);

        left_connector(color);
        right_connector(color);

        corners(color);

        border.invalidate_cache();
        return *this;
      }

      BackgroundColoring& vertical(RGB rgb) {
        border.background_colors.vertical = border.solve_color(rgb, true);

        left_connector(rgb);
        right_connector(rgb);

        corners(rgb);

        border.invalidate_cache();
        return *this;
      }

      BackgroundColoring& left_connector(Color color) {
        border.background_colors.left_connector = border.solve_color(color, true);

        border.invalidate_cache();
        return *this;
      }

      BackgroundColoring& left_connector(RGB rgb) {
        border.background_colors.left_connector = border.solve_color(rgb, true);

        border.invalidate_cache();
        return *this;
      }

      BackgroundColoring& right_connector(Color color) {
        border.background_colors.right_connector = border.solve_color(color, true);

        border.invalidate_cache();
        return *this;
      }

      BackgroundColoring& right_connector(RGB rgb) {
        border.background_colors.right_connector = border.solve_color(rgb, true);

        border.invalidate_cache();
        return *this;
      }

      /* ------------------ corners ------------------------ */
      BackgroundColoring& corners(Color color) {
        top_left_corner(color);
        top_right_corner(color);

        bottom_left_corner(color);
        bottom_right_corner(color);

        return *this;
      }

      BackgroundColoring& corners(RGB rgb) {
        top_left_corner(rgb);
        top_right_corner(rgb);

        bottom_left_corner(rgb);
        bottom_right_corner(rgb);

        return *this;
      }

      BackgroundColoring& top_left_corner(Color color) {
        border.background_colors.top_left_corner = border.solve_color(color, true);

        border.invalidate_cache();
        return *this;
      }

      BackgroundColoring& top_left_corner(RGB rgb) {
        border.background_colors.top_left_corner = border.solve_color(rgb, true);

        border.invalidate_cache();
        return *this;
      }

      BackgroundColoring& top_right_corner(Color color) {
        border.background_colors.top_right_corner = border.solve_color(color, true);

        border.invalidate_cache();
        return *this;
      }

      BackgroundColoring& top_right_corner(RGB rgb) {
        border.background_colors.top_right_corner = border.solve_color(rgb, true);

        border.invalidate_cache();
        return *this;
      }

      BackgroundColoring& bottom_left_corner(Color color) {
        border.background_colors.bottom_left_corner = border.solve_color(color, true);

        border.invalidate_cache();
        return *this;
      }

      BackgroundColoring& bottom_left_corner(RGB rgb) {
        border.background_colors.bottom_left_corner = border.solve_color(rgb, true);

        border.invalidate_cache();
        return *this;
      }

      BackgroundColoring& bottom_right_corner(Color color) {
        border.background_colors.bottom_right_corner = border.solve_color(color, true);

        border.invalidate_cache();
        return *this;
      }

      BackgroundColoring& bottom_right_corner(RGB rgb) {
        border.background_colors.bottom_right_corner = border.solve_color(rgb, true);

        border.invalidate_cache();
        return *this;
      }

      /* ------------------ intersection point ------------------------ */
      BackgroundColoring& intersection(Color color) {
        border.background_colors.intersection = border.solve_color(color, true);

        border.invalidate_cache();
        return *this;
      }

      BackgroundColoring& intersection(RGB rgb) {
        border.background_colors.intersection = border.solve_color(rgb, true);

        border.invalidate_cache();
        return *this;
      }
    };

public:
    Border() : style(BorderStyle::standard), disabled_styles(false), valid_cache(false) {}

    Getters get() { return Getters(*this); }

    Setters set() { return Setters(*this); }

    Parts parts() { return Parts(*this); }

    Coloring color() { return Coloring(*this); }

    BackgroundColoring background_color() { return BackgroundColoring(*this); }
  };

  class Table {
    Border table_border;

    unsigned int width;

    // used when the output stream is not
    // a valid TTY
    unsigned int non_tty_width;

    // columns number to track Regularity
    unsigned int columns_number;

    uint8_t width_percent;
    uint8_t back_limit_percent;

    bool separated_rows;
    bool disabled_styles;
    bool regular;

    class Setters {
      Table& table;

  public:
      Setters(Table& table) : table(table) {}

      Setters& width(int width) {
        if (width > 0)
          table.width = static_cast<unsigned int>(width);
        else
          table.width = 0;

        return *this;
      }

      // Sets the table width as a percentage of the total width of the terminal.
      // Values outside the 1–100 range are ignored.
      Setters& width_percent(int percent) {
        if (percent > 0 && percent <= 100)
          table.width_percent = static_cast<uint8_t>(percent);

        return *this;
      }

      // Sets the wrap threshold as a percentage of the total width.
      // Values outside the 1–100 range are ignored.
      Setters& back_limit_percent(int percent) {
        if (percent > 0 && percent <= 100)
          table.back_limit_percent = static_cast<uint8_t>(percent);

        return *this;
      }

      // Sets the table width for invalid output streams.
      Setters& non_tty_width(int width) {
        if (width > 0)
          table.non_tty_width = static_cast<unsigned int>(width);

        return *this;
      }

      // multi-byte strings support
      Setters& multi_byte_characters(bool is_multi_byte) {
        for (Row& row : table.rows)
          for (Column& column : row.columns)
            column.set().multi_byte_characters(is_multi_byte);

        return *this;
      }

      // border between rows
      Setters& separated_rows(bool is_separated) {
        table.separated_rows = is_separated;
        return *this;
      }

      // disable the whole table styles.
      // useful when dealing with non-tty streams
      Setters& disabled_styles(bool is_disabled) {
        table.disabled_styles = is_disabled;
        return *this;
      }
    };

    class Getters {
      const Table& table;

  public:
      Getters(const Table& table) : table(table) {}

      unsigned int width() const { return table.width; }

      uint8_t width_percent() const { return table.width_percent; }

      uint8_t back_limit_percent() const { return table.back_limit_percent; }

      unsigned int non_tty_width() const { return table.non_tty_width; }

      bool disabled_styles() const { return table.disabled_styles; }

      bool regular() const { return table.regular; }

      bool separated_rows() const { return table.separated_rows; }
    };

public:
    std::vector<Row> rows;

    Table()
        : width(0),
          width_percent(60),
          non_tty_width(60),
          columns_number(0),
          back_limit_percent(30),
          separated_rows(true),
          disabled_styles(false),
          regular(true) {}

    Border& border() { return table_border; }

    Setters set() { return Setters(*this); }

    Getters get() const { return Getters(*this); }

    Table& add_row(const std::vector<std::string>& contents) {
      std::vector<Column> columns;
      columns.reserve(contents.size());

      for (const std::string& content : contents)
        columns.emplace_back(content);

      // tracking Regularity
      if (regular) {
        if (this->rows.empty())
          this->columns_number = columns.size();

        else if (columns_number != columns.size())
          regular = false;
      }

      this->rows.emplace_back(std::move(columns));
      return *this;
    }

    Row& operator[](int index) {
      return this->rows.at(index);
    }
  };

  namespace detail {
    namespace codec {
      struct interval {
        uint32_t first;
        uint32_t last;
      };

      // clang-format off
    static constexpr interval wide[] = {
      {0x1100, 0x115F}, {0x231A, 0x231B}, {0x2329, 0x232A}, 
      {0x23E9, 0x23EC}, {0x23F0, 0x23F0}, {0x23F3, 0x23F3}, 
      {0x25FD, 0x25FE}, {0x2614, 0x2615}, {0x2630, 0x2637}, 
      {0x2648, 0x2653}, {0x267F, 0x267F}, {0x268A, 0x268F}, 
      {0x2693, 0x2693}, {0x26A1, 0x26A1}, {0x26AA, 0x26AB}, 
      {0x26BD, 0x26BE}, {0x26C4, 0x26C5}, {0x26CE, 0x26CE}, 
      {0x26D4, 0x26D4}, {0x26EA, 0x26EA}, {0x26F2, 0x26F3}, 
      {0x26F5, 0x26F5}, {0x26FA, 0x26FA}, {0x26FD, 0x26FD}, 
      {0x2705, 0x2705}, {0x270A, 0x270B}, {0x2728, 0x2728}, 
      {0x274C, 0x274C}, {0x274E, 0x274E}, {0x2753, 0x2755}, 
      {0x2757, 0x2757}, {0x2795, 0x2797}, {0x27B0, 0x27B0}, 
      {0x27BF, 0x27BF}, {0x2B1B, 0x2B1C}, {0x2B50, 0x2B50}, 
      {0x2B55, 0x2B55}, {0x2E80, 0x2E99}, {0x2E9B, 0x2EF3}, 
      {0x2F00, 0x2FD5}, {0x2FF0, 0x3029}, {0x302E, 0x303E}, 
      {0x3041, 0x3096}, {0x309B, 0x30FF}, {0x3105, 0x312F}, 
      {0x3131, 0x318E}, {0x3190, 0x31E5}, {0x31EF, 0x321E}, 
      {0x3220, 0x3247}, {0x3250, 0xA48C}, {0xA490, 0xA4C6}, 
      {0xA960, 0xA97C}, {0xAC00, 0xD7A3}, {0xF900, 0xFA6D}, 
      {0xFA70, 0xFAD9}, {0xFE10, 0xFE19}, {0xFE30, 0xFE52}, 
      {0xFE54, 0xFE66}, {0xFE68, 0xFE6B}, {0xFF01, 0xFF60}, 
      {0xFFE0, 0xFFE6}, {0x16FE0, 0x16FE3}, {0x16FF0, 0x16FF1}, 
      {0x17000, 0x187F7}, {0x18800, 0x18CD5}, {0x18CFF, 0x18D08}, 
      {0x1AFF0, 0x1AFF3}, {0x1AFF5, 0x1AFFB}, {0x1AFFD, 0x1AFFE}, 
      {0x1B000, 0x1B122}, {0x1B132, 0x1B132}, {0x1B150, 0x1B152}, 
      {0x1B155, 0x1B155}, {0x1B164, 0x1B167}, {0x1B170, 0x1B2FB}, 
      {0x1D300, 0x1D356}, {0x1D360, 0x1D376}, {0x1F004, 0x1F004}, 
      {0x1F0CF, 0x1F0CF}, {0x1F18E, 0x1F18E}, {0x1F191, 0x1F19A}, 
      {0x1F200, 0x1F202}, {0x1F210, 0x1F23B}, {0x1F240, 0x1F248}, 
      {0x1F250, 0x1F251}, {0x1F260, 0x1F265}, {0x1F300, 0x1F320}, 
      {0x1F32D, 0x1F335}, {0x1F337, 0x1F37C}, {0x1F37E, 0x1F393}, 
      {0x1F3A0, 0x1F3CA}, {0x1F3CF, 0x1F3D3}, {0x1F3E0, 0x1F3F0}, 
      {0x1F3F4, 0x1F3F4}, {0x1F3F8, 0x1F43E}, {0x1F440, 0x1F440}, 
      {0x1F442, 0x1F4FC}, {0x1F4FF, 0x1F53D}, {0x1F54B, 0x1F54E}, 
      {0x1F550, 0x1F567}, {0x1F57A, 0x1F57A}, {0x1F595, 0x1F596}, 
      {0x1F5A4, 0x1F5A4}, {0x1F5FB, 0x1F64F}, {0x1F680, 0x1F6C5}, 
      {0x1F6CC, 0x1F6CC}, {0x1F6D0, 0x1F6D2}, {0x1F6D5, 0x1F6D7}, 
      {0x1F6DC, 0x1F6DF}, {0x1F6EB, 0x1F6EC}, {0x1F6F4, 0x1F6FC}, 
      {0x1F7E0, 0x1F7EB}, {0x1F7F0, 0x1F7F0}, {0x1F90C, 0x1F93A}, 
      {0x1F93C, 0x1F945}, {0x1F947, 0x1F9FF}, {0x1FA70, 0x1FA7C}, 
      {0x1FA80, 0x1FA89}, {0x1FA8F, 0x1FAC6}, {0x1FACE, 0x1FADC}, 
      {0x1FADF, 0x1FAE9}, {0x1FAF0, 0x1FAF8}, {0x20000, 0x2A6DF}, 
      {0x2A700, 0x2B739}, {0x2B740, 0x2B81D}, {0x2B820, 0x2CEA1}, 
      {0x2CEB0, 0x2EBE0}, {0x2EBF0, 0x2EE5D}, {0x2F800, 0x2FA1D}, 
      {0x30000, 0x3134A}, {0x31350, 0x323AF}
    };
      // clang-format on

      static constexpr size_t wide_size = sizeof(wide) / sizeof(interval);

      static constexpr unsigned char utf8_len[256] = {
          1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
          1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
          1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
          1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
          1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
          1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
          1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
          1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
          2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
          2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
          3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
          4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

      // binary search in `wide` array of `interval`s
      inline bool wide_bisearch(uint32_t cp) noexcept {
        if (cp < wide[0].first || cp > wide[wide_size - 1].last) {
          return false;
        }

        size_t lo = 0, hi = wide_size - 1;

        while (lo <= hi) {
          const size_t mid = lo + ((hi - lo) >> 1);
          const interval& range = wide[mid];

          if (cp < range.first)
            hi = mid - 1;

          else if (cp > range.last)
            lo = mid + 1;

          else
            return true;
        }

        return false;
      }

      /* Decode UTF-8 to code point */
      inline bool utf8_decode(const char* s, uint32_t& codepoint, int& consumed) {
        const unsigned char* u = reinterpret_cast<const unsigned char*>(s);
        unsigned char c = u[0];
        consumed = utf8_len[c];
        if (consumed == 0) return false;

        switch (consumed) {
          case 1:
            codepoint = c;
            return true;
          case 2:
            if ((u[1] & 0xC0) != 0x80) return false;
            codepoint = ((c & 0x1F) << 6) | (u[1] & 0x3F);
            return codepoint >= 0x80;
          case 3:
            if ((u[1] & 0xC0) != 0x80 || (u[2] & 0xC0) != 0x80) return false;
            codepoint = ((c & 0x0F) << 12) | ((u[1] & 0x3F) << 6) | (u[2] & 0x3F);
            return codepoint >= 0x800 && (codepoint < 0xD800 || codepoint > 0xDFFF);
          case 4:
            if ((u[1] & 0xC0) != 0x80 || (u[2] & 0xC0) != 0x80 || (u[3] & 0xC0) != 0x80) return false;
            codepoint = ((c & 0x07) << 18) | ((u[1] & 0x3F) << 12) |
                        ((u[2] & 0x3F) << 6) | (u[3] & 0x3F);
            return codepoint >= 0x10000 && codepoint <= 0x10FFFF;
        }
        return false;
      }

      inline size_t wcwidth(uint32_t cp) noexcept {
        if (cp == 0) return 0;

        // custom check for tabular project
        if (cp == 0x1B) return 1;

        return wide_bisearch(cp) ? 2 : 1;
      }

      inline size_t utf8dw(const char* s) noexcept {
        size_t width = 0;
        const unsigned char* p = reinterpret_cast<const unsigned char*>(s);

        while (*p) {
          const unsigned char c = static_cast<unsigned char>(*p);

          // ASCII chars
          if (c < 0x80) {
            width += (c >= 0x20) ? 1 : (c == 0x1B ? 1 : (c == 0 ? 0 : 1));
            ++p;
            continue;
          }

          // Multi-byte UTF-8 sequence
          const unsigned char len = utf8_len[*p];
          if (len == 0) {
            // Invalid UTF-8 byte, treated as width 1
            ++width;
            ++p;
            continue;
          }

          uint32_t cp;
          int consumed;
          if (utf8_decode(reinterpret_cast<const char*>(p), cp, consumed)) {
            width += wcwidth(cp);
            p += consumed;
          }

          // Invalid UTF-8 byte, treated as width 1
          else {
            ++width;
            ++p;
          }
        }

        return width;
      }

      /*
       * Read a the next UTF-8 character (which may be 1-4 bytes long)
       * starting at the specified position `pos` and copies it into
       * a provided buffer `buffer`.
       *
       * @param str: The UTF-8 encoded string to extract from.
       * @param buffer: Output buffer to store the character sequence.
       * @param pos: The position where to start extraction.
       * @param len: Reference parameter for the length
       *             of th extracted `buffer` in bytes.
       *
       * @return true: when a valid UTF-8 sequence was extracted, otherwise
       *               it returns false.
       *
       */
      inline bool next_utf8_sequence(const std::string& str, char buffer[5], size_t pos,
                                     size_t& len) {
        const unsigned char first_byte = static_cast<unsigned char>(str[pos]);

        // the first byte MUST indicate the start of a UTF-8 sequence
        if ((first_byte & 0xC0) == 0x80 || pos >= str.length()) {
          len = 0;
          buffer[0] = '\0';
          return false;
        }

        len = utf8_len[first_byte];

        // Invalid UTF-8
        if (len == 0) {
          len = 0;
          buffer[0] = str[pos];
          buffer[1] = '\0';
          return false;
        }

        if (pos + len > str.length())
          len = str.length() - pos;

        // Copy bytes
        switch (len) {
          case 4:
            buffer[3] = str[pos + 3];
          case 3:
            buffer[2] = str[pos + 2];
          case 2:
            buffer[1] = str[pos + 1];
          case 1:
            buffer[0] = str[pos];
            break;
          default:
            // in case
            buffer[0] = str[pos];
            len = 1;
        }

        buffer[len] = '\0';

        return true;
      }

    } // namespace codec

    namespace string_utils {
      inline size_t display_width(const std::string& str, bool is_multi_byte) {
        return is_multi_byte ? codec::utf8dw(str.c_str()) : str.length();
      }

      // split by words AND save both '\n' and ' ' as words too
      // works at least for UTF-8 encoding strings
      inline std::list<std::string> split_text(const std::string& str) {
        std::list<std::string> result;

        size_t start = 0;
        const size_t len = str.size();

        for (size_t i = 0; i < len; ++i) {
          char ch = str[i];

          if (ch == ' ' || ch == '\n') {
            if (i > start)
              result.emplace_back(str.begin() + start, str.begin() + i);

            result.emplace_back(1, ch); // Push " " or "\n"
            start = i + 1;
          }
        }

        if (start < len)
          result.emplace_back(str.begin() + start, str.end());

        return result;
      }
    } // namespace string_utils

    namespace utils {
      // to align PPDirectives
      // clang-format off
      inline unsigned short get_terminal_width(FILE* stream) {
        // in case of stdout we could check the environnement variable
        if (stream == stdout) {
          const char* columns_env = std::getenv("COLUMNS");
          if (columns_env != nullptr) {
            try {
              int width_int = std::stoi(columns_env);
              if (width_int > 0 && width_int <= USHRT_MAX)
                return static_cast<unsigned short>(width_int);
            } catch (...) {}
          }
        }

        unsigned short width = 0;

        #if defined(UNIX_LIKE)
          int fd = fileno(stream);
          if (!isatty(fd)) return 0; // Note a terminal

          struct winsize ws;
          if (ioctl(fd, TIOCGWINSZ, &ws) == 0)
            width = ws.ws_col;

        #elif defined(WINDOWS)
          int fd = _fileno(stream);
          if (!_isatty(fd)) return 0; // Not a terminal

          HANDLE hConsole = reinterpret_cast<HANDLE>(_get_osfhandle(fd)); 
          CONSOLE_SCREEN_BUFFER_INFO csbi;

          if (hConsole != INVALID_HANDLE_VALUE && GetConsoleScreenBufferInfo(hConsole, &csbi))
            width = csbi.srWindow.Right - csbi.srWindow.Left + 1;

        #endif
        return width;
      }

      /*
       * return value:
       *  1: when everything is fine (success)
       *  2: can't enable VTP for windows 
       *  3: `stream` is not a valid tty
       */
      inline int enable_ansi_support(FILE* stream) {
        /*
         * for windows we should enable VTP mode
         * to ensure that ansi escape sequences
         * will be displayed correctly.
         */
        #if defined(WINDOWS)
          int fd = _fileno(stream);
          if (!_isatty(fd))
            return 3;
      
          HANDLE hOut = reinterpret_cast<HANDLE>(_get_osfhandle(fd));
          if (hOut == INVALID_HANDLE_VALUE)
            return 2;
      
          DWORD dwMode = 0;
          if (!GetConsoleMode(hOut, &dwMode))
            return 2;
      
          dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
          if (!SetConsoleMode(hOut, dwMode))
            return 2;
      
          return 1;
      
        /*
         * In Unix-like systems most consoles support ansi escape
         * sequences by default.
         */
        #else
          if (!isatty(fileno(stream)))
            return 3;
      
          return 1;
        #endif
      }
      // clang-format on
    } // namespace utils

    namespace printer {
      // returns the processed line
      inline void append_column_line(std::string& line, size_t& line_width,
                                     size_t max_width, Column& column,
                                     std::vector<ColumnLines>& result,
                                     bool disabled_styles) {
        std::string formatted_line;
        Alignment align = column.get().alignment();

        std::string styles;

        if (!disabled_styles) {
          // `suffix` to complete the text styles statement.
          styles.append(column.get().text_attributes());

          if (!styles.empty()) styles.push_back(ansi::suffix);

          styles.append(column.get().content_color());
          styles.append(column.get().content_background_color());
        }

        // Adjusting Alignment
        int start;
        if (align == Alignment::center)
          start = (max_width - line_width) / 2;
        else if (align == Alignment::right)
          start = (max_width - line_width);
        else
          start = 0;

        // calculating line size
        size_t line_len = start + styles.size() +
                          line.size() + (styles.empty() ? 0 : strlen(ansi::RESET));
        formatted_line.reserve(line_len);

        // auto horizontal padding of 1
        formatted_line = " ";

        // alignment
        formatted_line.append(start, ' ');

        // counting spaces
        line_width += start + 1;

        // appending the line with styles
        formatted_line.append(styles + line);
        if (!styles.empty())
          formatted_line.append(ansi::RESET);

        result.emplace_back(formatted_line, line_width);

        line.clear();
        line_width = 0;
      }

      inline void process_word(const std::string& word, size_t& word_width,
                               std::string& line, size_t& line_width,
                               size_t max_width, size_t back_limit,
                               std::vector<ColumnLines>& result, Column& column,
                               bool disabled_styles) {
        bool multi_byte_characters = column.get().multi_byte_characters();

        if (line.empty() && word == " ")
          return;

        // add existing content if we reach new line
        if (word == "\n") {
          append_column_line(line, line_width, max_width, column, result, disabled_styles);
          return;
        }

        // normal case: append the word
        if ((line_width + word_width) <= max_width) {
          line += word;
          line_width += word_width;

          return;
        }

        /*
         * if we reach here, that means (line_width + word_width) > width
         * we should append the line after processing the last word.
         */

        // space left
        size_t remaining_space = max_width - line_width;

        // append the line, and leave the current word for the next line,
        // since the remaining_space doesn't exceed the back_limit
        if (remaining_space <= back_limit) {
          if (!line.empty() && line.back() == ' ') {
            line.pop_back(); // pop the last space if exist
            line_width--;
          }

          append_column_line(line, line_width, max_width, column, result, disabled_styles);

          process_word(word, word_width, line, line_width,
                       max_width, back_limit, result, column, disabled_styles);

          return;
        }

        /*
         * In this case we will be splitting the word into two parts.
         * the first parts should fill int this line, with a hyphen
         * ONLY if multi_byte_characters = false, otherwise we will not,
         * since other languages like Japanese doesn't need (actually they shouldn't have)
         * a hyphen at the end of a splitted word.
         *
         * The hardest part is handling multi-byte strings splitting.
         */

        // word parts
        std::string first_part;
        std::string remainder;

        size_t first_part_width = 0;

        // no multi-byte strings, basic case
        if (!multi_byte_characters) {
          // -1 for the hyphen
          first_part = word.substr(0, remaining_space - 1) + '-';
          remainder = word.substr(remaining_space - 1);

          first_part_width = first_part.length();
        }

        else {
          // initialize variables
          size_t pos = 0, len = 0, w = 0;

          char buffer[5];
          while (pos < word.length()) {
            // getting the next utf-8 sequence
            codec::next_utf8_sequence(word, buffer, pos, len);

            // don't exceed the remaining space
            if (pos + len > remaining_space)
              break;

            // otherwise increase the width w
            w += codec::utf8dw(buffer);

            // to iterate for the next utf-8 sequence
            pos += len;
          }

          // we found the right position to split
          if (pos < word.length()) {
            first_part = word.substr(0, pos);
            remainder = word.substr(pos);
          }

          // in case something goes wrong fallback into
          // using remaining_space, nothing will break
          // but the layout may have some issues.
          else {
            first_part = word.substr(0, remaining_space);
            remainder = word.substr(remaining_space);
          }

          // in all cases w is positive
          first_part_width = w;
        }

        // append the first part and his width
        line += first_part;
        line_width += first_part_width;

        append_column_line(line, line_width, max_width, column, result, disabled_styles);

        size_t remainder_width = word_width - first_part_width + ((multi_byte_characters) ? 0 : 1);
        process_word(remainder, remainder_width, line, line_width,
                     max_width, back_limit, result, column, disabled_styles);
      }

      inline void format_column(Column& column, uint8_t back_limit_percent,
                                bool disabled_styles, bool& multi_byte_characters_flag) {
        const std::string& content = column.content;
        const bool multi_byte_characters = column.get().multi_byte_characters();

        if (multi_byte_characters && !multi_byte_characters_flag)
          multi_byte_characters_flag = true;

        if (content.empty()) {
          column.set().lines(std::vector<ColumnLines>());
          return;
        }

        unsigned int top_padding = column.get().top_padding();
        unsigned int bottom_padding = column.get().bottom_padding();

        // to avoid empty columns
        if (column.get().width() <= 2)
          column.set().width(3);

        // MAX line width POSSIBLE, - 2 for two sides spaces
        const size_t max_width = (column.get().width() - 2);

        // the return result
        std::vector<ColumnLines> result;
        result.reserve(content.length() / max_width);

        // split the content into words to easily manipulate it
        auto words = string_utils::split_text(content);

        // TOP padding
        result.insert(result.end(), top_padding, ColumnLines("", 0));

        // space we shouldn't exceed if we decided to leave the current word
        // for the next line, meaning if the remaining_space > back_limit
        // we should FORCE CUT the word, else we can just append this line
        // and leave that word for the next line
        const size_t back_limit = (max_width * back_limit_percent) / 100;

        std::string line;
        size_t line_width = 0;
        for (auto it = words.begin(); it != words.end(); ++it) {
          const std::string& word = *it;

          // we need split
          size_t word_width = string_utils::display_width(word, multi_byte_characters);

          process_word(word, word_width, line, line_width,
                       max_width, back_limit, result, column, disabled_styles);
        }

        // any remaining words
        if (!line.empty())
          append_column_line(line, line_width, max_width, column, result, disabled_styles);

        // BOTTOM padding
        result.insert(result.end(), bottom_padding, ColumnLines("", 0));

        column.set().lines(result);
      }

      // return the size of the tallest splitted_content vector
      inline size_t tallest_cell(const Row& row) {
        size_t result = 0;
        for (const Column& column : row.columns) {
          size_t splitted_content_size = column.get().lines().size();
          result = std::max(result, splitted_content_size);
        }

        return result;
      }

      inline void adjust_width(Table& table) {
        unsigned int table_width = table.get().width();

        if (table_width <= 0)
          return;

        for (size_t i = 0; i < table.rows.size(); i++) {
          Row& row = table.rows[i];

          size_t columns_num = row.columns.size();
          if (columns_num == 0)
            continue;

          // minus table splits
          int table_usable_width = table_width - (columns_num + 1);
          int columns_width = 0;

          // when inconsistent widths are given we will force
          // overriding their original width
          bool forced = false;

          for (Column& column : row.columns) {
            unsigned int col_width = column.get().width();
            if (col_width != 0) {
              table_usable_width -= col_width;
              columns_width += col_width;
              columns_num--;
            }
          }

          // At this point, all column widths for the current row are known.
          if (columns_num == 0 && i == 0) {
            // First row: initialize table width to fit all columns plus separators.
            // make sure to update table_width too ;)
            table_width = columns_width + row.columns.size() + 1;
            table.set().width(table_width);
            continue;
          }

          // One of two issues:
          // Not the first row and has a full width initialization
          // We've consumed all usable width and there are still more columns
          else if ((columns_num == 0 && table_usable_width != 0) ||
                   (columns_num != 0 && table_usable_width <= 0)) {

            // Restore full width and update columns count for recalculating layout.
            table_usable_width += columns_width;
            columns_num = row.columns.size();

            // force overriding the original width
            forced = true;
          }

          if (columns_num == 0 && !forced)
            continue;

          int indiv_column_width = 0;
          int rest = 0;
          if (columns_num != 0) {
            indiv_column_width = table_usable_width / columns_num;
            rest = table_usable_width % columns_num;
          }

          for (Column& column : row.columns) {
            unsigned int width = column.get().width();

            if (width == 0 || forced) {
              width = indiv_column_width + (rest > 0 ? 1 : 0);
              if (rest > 0) rest--;
              column.set().width(width);
            }
          }
        }
      }

      inline std::unordered_set<size_t> separators_positions(const Row& row) {
        std::unordered_set<size_t> result;
        result.reserve(row.columns.size() - 1);

        size_t track = 1;
        for (const Column& column : row.columns) {
          track += column.get().width();
          result.insert(track);
          track++;
        }

        return result;
      }

      inline void resolve_border(const std::string*& left_corner, const std::string*& right_corner,
                                 const std::string*& intersection, const std::string*& top_connector,
                                 const std::string*& bottom_connector, const BorderGlyphs& glyphs,
                                 bool first, bool last) {

        if (first) {
          left_corner = &glyphs.top_left_corner;
          right_corner = &glyphs.top_right_corner;
          intersection = &glyphs.top_connector;
          top_connector = &glyphs.top_connector;
          bottom_connector = &glyphs.top_connector;
        }

        else if (last) {
          left_corner = &glyphs.bottom_left_corner;
          right_corner = &glyphs.bottom_right_corner;
          intersection = &glyphs.bottom_connector;
          top_connector = &glyphs.bottom_connector;
          bottom_connector = &glyphs.bottom_connector;
        }

        else {
          left_corner = &glyphs.left_connector;
          right_corner = &glyphs.right_connector;
          intersection = &glyphs.intersection;
          top_connector = &glyphs.top_connector;
          bottom_connector = &glyphs.bottom_connector;
        }
      }

      inline std::string print_border(std::vector<Row>::const_iterator& it,
                                      const BorderGlyphs& glyphs,
                                      bool first, bool last,
                                      size_t width) {
        // result
        std::string border;
        border.reserve(width);

        const Row& reference = *it;

        std::unordered_set<size_t> separators;
        // first row separators
        if (first)
          separators = separators_positions(reference);

        // next row separators
        else if (!last)
          separators = separators_positions(*(it + 1));

        if (!first)
          border.push_back('\n');

        const std::string& horizontal = glyphs.horizontal;

        const std::string* left_corner;
        const std::string* right_corner;
        const std::string* intersection;
        const std::string* top_connector;
        const std::string* bottom_connector;

        resolve_border(left_corner, right_corner,
                       intersection, top_connector,
                       bottom_connector, glyphs, first, last);

        border.append(*left_corner);

        const size_t columns_number = reference.columns.size();
        size_t tracker = 0;

        for (size_t j = 0; j < columns_number; ++j) {
          const Column& column = reference.columns[j];
          const unsigned int column_width = column.get().width();

          for (unsigned int k = 0; k < column_width; ++k) {
            ++tracker;
            border += separators.count(tracker) ? *top_connector : horizontal;
          }
          ++tracker;

          if (j + 1 != columns_number)
            border += separators.count(tracker) ? *intersection : *bottom_connector;
        }

        border += *right_corner;
        return border;
      }

      inline std::string print_row(const Row& row, const BorderGlyphs& glyphs, size_t width) {
        std::string result;

        const std::string& vertical = glyphs.vertical;
        const std::string& reset = ansi::RESET;
        const size_t tallest_cell_size = tallest_cell(row);

        result.reserve(width * tallest_cell_size);

        for (size_t i = 0; i < tallest_cell_size; ++i) {
          result += '\n' + vertical;

          for (const Column& column : row.columns) {
            const auto& lines = column.get().lines();
            const size_t lines_size = lines.size();
            const std::string column_background_color =
                column.get().disabled_styles() ? "" : column.get().column_background_color();

            size_t rest = column.get().width();

            if (lines_size > i) {
              const std::string& current_line = lines[i].line;
              const size_t current_line_size = lines[i].display_width;
              const bool multi_byte_characters = column.get().multi_byte_characters();

              // appending the column
              result += column_background_color + current_line;

              rest -= current_line_size;
            }

            result += column_background_color;
            result.append(rest, ' ');

            if (!column_background_color.empty())
              result += reset;

            result += vertical;
          }
        }

        return result;
      }

      inline void handle_output(const std::string& formatted_table,
                                bool multi_byte_characters_flag, STD stream) {
        // clang-format off
        #if defined(WINDOWS)
          HANDLE handle;
          if (std == STD::Out)
            handle = GetStdHandle(STD_OUTPUT_HANDLE);
          else
            handle = GetStdHandle(STD_ERROR_HANDLE);

          DWORD mode;
          DWORD written;
          bool is_console_output = GetConsoleMode(handle, &mode);

          if (!is_console_output) {
            // Output is redirected: write raw UTF-8 bytes
            WriteFile(handle, formatted_table.c_str(),
                formatted_table.length(), &written, nullptr);
            return;
          }

          if (multi_byte_characters_flag) {
            // Convert UTF-8 to UTF-16 for proper console rendering
            int wlen = MultiByteToWideChar(CP_UTF8, 0,
                formatted_table.c_str(), -1, nullptr, 0);
            if (wlen <= 0) {
              WriteFile(handle, formatted_table.c_str(),
                  formatted_table.length(), &written, nullptr);
              return;
            }

            std::wstring wstr(wlen - 1, L'\0'); // exclude null terminator
            int result = MultiByteToWideChar(CP_UTF8, 0, formatted_table.c_str(),
                -1, &wstr[0], wlen);
            if (result <= 0) {
              WriteFile(handle, formatted_table.c_str(), formatted_table.length(),
                  &written, nullptr);
              return;
            }

            WriteConsoleW(handle, wstr.c_str(), static_cast<DWORD>(wstr.length()),
                &written, nullptr);
          } else {
            // ASCII or simple UTF-8 output
            WriteConsoleA(handle, formatted_table.c_str(),
                static_cast<DWORD>(formatted_table.length()), &written, nullptr);
          }
        #else
          FILE* out_stream = stream == STD::Out ? stdout : stderr;
          fwrite(formatted_table.c_str(), 1, formatted_table.length(), out_stream);
          fflush(out_stream);
        #endif
        // clang-format on
      }

      inline std::string format_table(Table& table, bool disabled_styles,
                                      bool& multi_byte_characters_flag, FILE* stream) {
        // result
        std::string formatted_table;

        if (table.rows.empty())
          return "";

        if (table.get().width() == 0) {
          unsigned short terminal_width = detail::utils::get_terminal_width(stream);

          // setting the width via the percent
          table.set().width((terminal_width * table.get().width_percent()) / 100);
        }

        // return code
        int rc = detail::utils::enable_ansi_support(stream);

        /*
         * In case enabling ansi escape sequences fail, or the `stream`
         * is not a valid TTY disable the styles to prevent printing raw
         * ansi escape sequences
         */
        if (rc != 1) {
          // if something goes wrong disable the styles
          // to prevent printing raw ansi escape sequences
          disabled_styles = true;

          // in case it is not a TTY (for example the output stream is a file)
          // we will be applying the non_tui_width to prevent infinity loops
          // due to the large value of table.width because non-tty streams
          // most of the times, don't have a limited columns number.
          if (rc == 3)
            table.set().width(table.get().non_tty_width());
        }

        detail::printer::adjust_width(table);

        uint8_t back_limit_percent = table.get().back_limit_percent();

        for (Row& row : table.rows) {
          for (Column& column : row.columns) {
            detail::printer::format_column(column, back_limit_percent,
                                           disabled_styles, multi_byte_characters_flag);
          }
        }

        const BorderStyle old_style = table.border().get().style();

        if (disabled_styles && old_style == BorderStyle::ansi)
          table.border().set().style(BorderStyle::standard);

        // border parts
        BorderGlyphs glyphs = table.border().get().processed_glyphs();

        if (table.border().get().style() != old_style)
          table.border().set().style(old_style);

        /* Starting printing */
        const auto& rows = table.rows;

        bool is_first = true, is_last = (rows.size() == 1) ? true : false;
        bool separated_rows = table.get().separated_rows();
        size_t width = table.get().width();

        auto it = rows.begin();
        formatted_table += detail::printer::print_border(it, glyphs, is_first, is_last, width);

        is_first = false;
        for (it = rows.begin(); it != rows.end(); ++it) {
          const Row& row = *it;

          formatted_table += detail::printer::print_row(row, glyphs, width);

          if ((it + 1) == rows.end())
            is_last = true;

          if (separated_rows)
            formatted_table += detail::printer::print_border(it, glyphs,
                                                             is_first, is_last, width);
        }

        // appending last new line
        formatted_table.push_back('\n');

        return formatted_table;
      }
    } // namespace printer
  } // namespace detail

  inline void print(Table& table, STD stream = STD::Out) {
    bool multi_byte_characters_flag = false;

    FILE* file_stream = stream == STD::Out ? stdout : stderr;

    std::string formatted_table =
        detail::printer::format_table(table, table.get().disabled_styles(),
                                      multi_byte_characters_flag, file_stream);

    detail::printer::handle_output(formatted_table, multi_byte_characters_flag, stream);
  }

  inline void print(Table& table, FILE* file) {
    if (!file)
      return;

    bool multi_byte_characters_flag = false;

    std::string formatted_table =
        detail::printer::format_table(table, table.get().disabled_styles(),
                                      multi_byte_characters_flag, file);

    // clang-format off
    #if defined(WINDOWS)
      int fd = _fileno(file);
      HANDLE handle = reinterpret_cast<HANDLE>(_get_osfhandle(fd));
      DWORD written;

      if (handle != INVALID_HANDLE_VALUE) {
        WriteFile(handle, formatted_table.c_str(), formatted_table.length(), &written, nullptr);
        return;
      }
    #elif defined (UNIX_LIKE)
      fwrite(formatted_table.c_str(), 1, formatted_table.length(), file);
      fflush(file);
      return;
    #endif
    // clang-format on

    // fallback
    fprintf(file, "%s", formatted_table.c_str());
  }
} // namespace tabular
