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

#ifndef TABULAR_BORDER_HPP
#define TABULAR_BORDER_HPP

#include <map>
#include <string>

#include <tabular/colors.hpp>
#include <tabular/definitions.hpp>
#include <tabular/rgb.hpp>

namespace tabular {
  enum class BorderStyle {
    standard,
    empty,
    ANSI
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

  class Border {
private:
    BorderGlyphs glyphs;

    BorderStyle style;

    BorderGlyphs colors;
    BorderGlyphs background_colors;

    bool is_multi_byte;

    class Getters {
      Border& border;

  public:
      Getters(Border& border) : border(border) {}

      BorderStyle style() { return border.style; }

      BorderGlyphs glyphs() {
        static const std::map<BorderStyle, BorderGlyphs> styleTemplates{
            {BorderStyle::empty, {" ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "}},
            {BorderStyle::standard, {"|", "-", "+", "+", "+", "+", "+", "+", "+", "+", "+"}},
            {BorderStyle::ANSI, {
                                    TABLE_MODE "x" RESET_TABLE_MODE, // vertical
                                    TABLE_MODE "q" RESET_TABLE_MODE, // horizontal
                                    TABLE_MODE "j" RESET_TABLE_MODE, // bottom_right_corner
                                    TABLE_MODE "k" RESET_TABLE_MODE, // top_right_corner
                                    TABLE_MODE "l" RESET_TABLE_MODE, // top_left_corner
                                    TABLE_MODE "m" RESET_TABLE_MODE, // bottom_left_corner
                                    TABLE_MODE "n" RESET_TABLE_MODE, // intersection
                                    TABLE_MODE "t" RESET_TABLE_MODE, // left_connector
                                    TABLE_MODE "u" RESET_TABLE_MODE, // right_connector
                                    TABLE_MODE "v" RESET_TABLE_MODE, // bottom_connector
                                    TABLE_MODE "w" RESET_TABLE_MODE  // top_connector
                                }}};

        const BorderGlyphs& def = styleTemplates.at(border.style);
        BorderGlyphs result;

        auto pick = [](const std::string& val, const std::string& fallback) -> std::string {
          return val.empty() ? fallback : val;
        };

        auto wrap = [](const std::string& fg, const std::string& bg, const std::string& val) -> std::string {
          if (fg.empty() && bg.empty()) return val;
          return fg + bg + val + RESET;
        };

        result.vertical = wrap(border.colors.vertical,
                               border.background_colors.vertical,
                               pick(border.glyphs.vertical, def.vertical));

        result.horizontal = wrap(border.colors.horizontal,
                                 border.background_colors.horizontal,
                                 pick(border.glyphs.horizontal, def.horizontal));

        result.bottom_right_corner = wrap(border.colors.bottom_right_corner,
                                          border.background_colors.bottom_right_corner,
                                          pick(border.glyphs.bottom_right_corner, def.bottom_right_corner));

        result.top_right_corner = wrap(border.colors.top_right_corner,
                                       border.background_colors.top_right_corner,
                                       pick(border.glyphs.top_right_corner, def.top_right_corner));

        result.top_left_corner = wrap(border.colors.top_left_corner,
                                      border.background_colors.top_left_corner,
                                      pick(border.glyphs.top_left_corner, def.top_left_corner));

        result.bottom_left_corner = wrap(border.colors.bottom_left_corner,
                                         border.background_colors.bottom_left_corner,
                                         pick(border.glyphs.bottom_left_corner, def.bottom_left_corner));

        result.intersection = wrap(border.colors.intersection,
                                   border.background_colors.intersection,
                                   pick(border.glyphs.intersection, def.intersection));

        result.left_connector = wrap(border.colors.left_connector,
                                     border.background_colors.left_connector,
                                     pick(border.glyphs.left_connector, def.left_connector));

        result.right_connector = wrap(border.colors.right_connector,
                                      border.background_colors.right_connector,
                                      pick(border.glyphs.right_connector, def.right_connector));

        result.bottom_connector = wrap(border.colors.bottom_connector,
                                       border.background_colors.bottom_connector,
                                       pick(border.glyphs.bottom_connector, def.bottom_connector));

        result.top_connector = wrap(border.colors.top_connector,
                                    border.background_colors.top_connector,
                                    pick(border.glyphs.top_connector, def.top_connector));

        return result;
      }

      std::string horizontal() const {
        return border.glyphs.horizontal;
      }

      std::string vertical() const {
        return border.glyphs.vertical;
      }

      std::string top_left_corner() const {
        return border.glyphs.top_left_corner;
      }

      std::string top_right_corner() const {
        return border.glyphs.top_right_corner;
      }

      std::string bottom_left_corner() const {
        return border.glyphs.bottom_left_corner;
      }

      std::string bottom_right_corner() const {
        return border.glyphs.bottom_right_corner;
      }

      std::string intersection() const {
        return border.glyphs.intersection;
      }

      std::string left_connector() const {
        return border.glyphs.left_connector;
      }

      std::string right_connector() const {
        return border.glyphs.right_connector;
      }

      std::string top_connector() const {
        return border.glyphs.top_connector;
      }

      std::string bottom_connector() const {
        return border.glyphs.bottom_connector;
      }
    };

    class Setters {
      Border& border;

  public:
      Setters(Border& border) : border(border) {}

      /*
       * Note: It is the user's responsibility to ensure that each border glyph is a single
       * terminal column wide. If a string with a visual width > 1 is provided, the table
       * layout may break.
       *
       * Using str.length() is unreliable for multi-byte characters, and calling utils::mbswidth()
       * on every assignment would introduce unnecessary performance overhead. Supporting accurate
       * width checks would also require UTF-8 decoding and wide character handling, which is
       * beyond the scope of this setter.
       *
       * Since this library is aimed at developers, it assumes users understand the implications
       * of using multi-byte or wide characters in border definitions.
       *
       * — Anas Hamdane, 2025-06-09
       */

      Setters& style(BorderStyle style) {
        border.style = style;
        return *this;
      }

      Setters& horizontal(std::string horizontal) {
        border.glyphs.horizontal = horizontal;
        return *this;
      }

      Setters& vertical(std::string vertical) {
        border.glyphs.vertical = vertical;
        return *this;
      }

      Setters& corner(std::string corner) {
        border.glyphs.bottom_right_corner = corner;
        border.glyphs.top_right_corner = corner;

        border.glyphs.bottom_left_corner = corner;
        border.glyphs.top_left_corner = corner;
        return *this;
      }

      Setters& top_left_corner(std::string corner) {
        border.glyphs.top_left_corner = corner;
        return *this;
      }

      Setters& top_right_corner(std::string corner) {
        border.glyphs.top_right_corner = corner;
        return *this;
      }

      Setters& bottom_left_corner(std::string corner) {
        border.glyphs.bottom_left_corner = corner;
        return *this;
      }

      Setters& bottom_right_corner(std::string corner) {
        border.glyphs.bottom_right_corner = corner;
        return *this;
      }

      Setters intersection(std::string intersection) {
        border.glyphs.intersection = intersection;
        return *this;
      }

      Setters& left_connector(std::string connector) {
        border.glyphs.left_connector = connector;
        return *this;
      }

      Setters& right_connector(std::string connector) {
        border.glyphs.right_connector = connector;
        return *this;
      }

      Setters& top_connector(std::string connector) {
        border.glyphs.top_connector = connector;
        return *this;
      }

      Setters& bottom_connector(std::string connector) {
        border.glyphs.bottom_connector = connector;
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
        if (!border.colors.horizontal.empty())
          border.colors.horizontal.clear();

        border.colors.horizontal = CSI + std::to_string(static_cast<int>(color)) + "m";

        top_connector(color);
        bottom_connector(color);

        corners(color);

        return *this;
      }

      Coloring& horizontal(RGB rgb) {
        if (!border.colors.horizontal.empty())
          border.colors.horizontal.clear();

        border.colors.horizontal = CSI "38;2;" + std::to_string(rgb.r) + ";" +
                                   std::to_string(rgb.g) + ";" +
                                   std::to_string(rgb.b) + "m";

        top_connector(rgb);
        bottom_connector(rgb);

        corners(rgb);

        return *this;
      }

      Coloring& bottom_connector(Color color) {
        if (!border.colors.bottom_connector.empty())
          border.colors.bottom_connector.clear();

        border.colors.bottom_connector = CSI + std::to_string(static_cast<int>(color)) + "m";

        return *this;
      }

      Coloring& bottom_connector(RGB rgb) {
        if (!border.colors.bottom_connector.empty())
          border.colors.bottom_connector.clear();

        border.colors.bottom_connector = CSI "38;2;" + std::to_string(rgb.r) + ";" +
                                         std::to_string(rgb.g) + ";" +
                                         std::to_string(rgb.b) + "m";

        return *this;
      }

      Coloring& top_connector(Color color) {
        if (!border.colors.top_connector.empty())
          border.colors.top_connector.clear();

        border.colors.top_connector = CSI + std::to_string(static_cast<int>(color)) + "m";

        return *this;
      }

      Coloring& top_connector(RGB rgb) {
        if (!border.colors.top_connector.empty())
          border.colors.top_connector.clear();

        border.colors.top_connector = CSI "38;2;" + std::to_string(rgb.r) + ";" +
                                      std::to_string(rgb.g) + ";" +
                                      std::to_string(rgb.b) + "m";

        return *this;
      }

      /* ------------------ vertical ------------------------ */
      Coloring& vertical(Color color) {
        if (!border.colors.vertical.empty())
          border.colors.vertical.clear();

        border.colors.vertical = CSI + std::to_string(static_cast<int>(color)) + "m";

        left_connector(color);
        right_connector(color);

        corners(color);

        return *this;
      }

      Coloring& vertical(RGB rgb) {
        if (!border.colors.vertical.empty())
          border.colors.vertical.clear();

        border.colors.vertical = CSI "38;2;" + std::to_string(rgb.r) + ";" +
                                 std::to_string(rgb.g) + ";" +
                                 std::to_string(rgb.b) + "m";

        left_connector(rgb);
        right_connector(rgb);

        corners(rgb);

        return *this;
      }

      Coloring& left_connector(Color color) {
        if (!border.colors.left_connector.empty())
          border.colors.left_connector.clear();

        border.colors.left_connector = CSI + std::to_string(static_cast<int>(color)) + "m";

        return *this;
      }

      Coloring& left_connector(RGB rgb) {
        if (!border.colors.left_connector.empty())
          border.colors.left_connector.clear();

        border.colors.left_connector = CSI "38;2;" + std::to_string(rgb.r) + ";" +
                                       std::to_string(rgb.g) + ";" +
                                       std::to_string(rgb.b) + "m";

        return *this;
      }

      Coloring& right_connector(Color color) {
        if (!border.colors.right_connector.empty())
          border.colors.right_connector.clear();

        border.colors.right_connector = CSI + std::to_string(static_cast<int>(color)) + "m";

        return *this;
      }

      Coloring& right_connector(RGB rgb) {
        if (!border.colors.right_connector.empty())
          border.colors.right_connector.clear();

        border.colors.right_connector = CSI "38;2;" + std::to_string(rgb.r) + ";" +
                                        std::to_string(rgb.g) + ";" +
                                        std::to_string(rgb.b) + "m";

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
        if (!border.colors.top_left_corner.empty())
          border.colors.top_left_corner.clear();

        border.colors.top_left_corner = CSI + std::to_string(static_cast<int>(color)) + "m";

        return *this;
      }

      Coloring& top_left_corner(RGB rgb) {
        if (!border.colors.top_left_corner.empty())
          border.colors.top_left_corner.clear();

        border.colors.top_left_corner = CSI "38;2;" + std::to_string(rgb.r) + ";" +
                                        std::to_string(rgb.g) + ";" +
                                        std::to_string(rgb.b) + "m";

        return *this;
      }

      Coloring& top_right_corner(Color color) {
        if (!border.colors.top_right_corner.empty())
          border.colors.top_right_corner.clear();

        border.colors.top_right_corner = CSI + std::to_string(static_cast<int>(color)) + "m";

        return *this;
      }

      Coloring& top_right_corner(RGB rgb) {
        if (!border.colors.top_right_corner.empty())
          border.colors.top_right_corner.clear();

        border.colors.top_right_corner = CSI "38;2;" + std::to_string(rgb.r) + ";" +
                                         std::to_string(rgb.g) + ";" +
                                         std::to_string(rgb.b) + "m";

        return *this;
      }

      Coloring& bottom_left_corner(Color color) {
        if (!border.colors.bottom_left_corner.empty())
          border.colors.bottom_left_corner.clear();

        border.colors.bottom_left_corner = CSI + std::to_string(static_cast<int>(color)) + "m";

        return *this;
      }

      Coloring& bottom_left_corner(RGB rgb) {
        if (!border.colors.bottom_left_corner.empty())
          border.colors.bottom_left_corner.clear();

        border.colors.bottom_left_corner = CSI "38;2;" + std::to_string(rgb.r) + ";" +
                                           std::to_string(rgb.g) + ";" +
                                           std::to_string(rgb.b) + "m";

        return *this;
      }

      Coloring& bottom_right_corner(Color color) {
        if (!border.colors.bottom_right_corner.empty())
          border.colors.bottom_right_corner.clear();

        border.colors.bottom_right_corner = CSI + std::to_string(static_cast<int>(color)) + "m";

        return *this;
      }

      Coloring& bottom_right_corner(RGB rgb) {
        if (!border.colors.bottom_right_corner.empty())
          border.colors.bottom_right_corner.clear();

        border.colors.bottom_right_corner = CSI "38;2;" + std::to_string(rgb.r) + ";" +
                                            std::to_string(rgb.g) + ";" +
                                            std::to_string(rgb.b) + "m";

        return *this;
      }

      /* ------------------ intersection point ------------------------ */
      Coloring& intersection(Color color) {
        if (!border.colors.intersection.empty())
          border.colors.intersection.clear();

        border.colors.intersection = CSI + std::to_string(static_cast<int>(color)) + "m";

        return *this;
      }

      Coloring& intersection(RGB rgb) {
        if (!border.colors.intersection.empty())
          border.colors.intersection.clear();

        border.colors.intersection = CSI "38;2;" + std::to_string(rgb.r) + ";" +
                                     std::to_string(rgb.g) + ";" +
                                     std::to_string(rgb.b) + "m";

        return *this;
      }
    };

    class BackgroundColoring {
      Border& border;

  public:
      BackgroundColoring(Border& border) : border(border) {}

      /* ------------------ full ------------------------ */
      BackgroundColoring& full(BackgroundColor background_color) {
        horizontal(background_color);
        vertical(background_color);

        return *this;
      }

      BackgroundColoring& full(RGB rgb) {
        horizontal(rgb);
        vertical(rgb);

        return *this;
      }

      /* ------------------ horizontal ------------------------ */
      BackgroundColoring& horizontal(BackgroundColor background_color) {
        if (!border.background_colors.horizontal.empty())
          border.background_colors.horizontal.clear();

        border.background_colors.horizontal = CSI + std::to_string(static_cast<int>(background_color)) + "m";

        top_connector(background_color);
        bottom_connector(background_color);

        corners(background_color);
        return *this;
      }

      BackgroundColoring& horizontal(RGB rgb) {
        if (!border.background_colors.horizontal.empty())
          border.background_colors.horizontal.clear();

        border.background_colors.horizontal = CSI "48;2;" + std::to_string(rgb.r) + ";" +
                                              std::to_string(rgb.g) + ";" +
                                              std::to_string(rgb.b) + "m";

        top_connector(rgb);
        bottom_connector(rgb);

        corners(rgb);

        return *this;
      }

      BackgroundColoring& bottom_connector(BackgroundColor background_color) {
        if (!border.background_colors.bottom_connector.empty())
          border.background_colors.bottom_connector.clear();

        border.background_colors.bottom_connector = CSI + std::to_string(static_cast<int>(background_color)) + "m";

        return *this;
      }

      BackgroundColoring& bottom_connector(RGB rgb) {
        if (!border.background_colors.bottom_connector.empty())
          border.background_colors.bottom_connector.clear();

        border.background_colors.bottom_connector = CSI "48;2;" + std::to_string(rgb.r) + ";" +
                                                    std::to_string(rgb.g) + ";" +
                                                    std::to_string(rgb.b) + "m";

        return *this;
      }

      BackgroundColoring& top_connector(BackgroundColor background_color) {
        if (!border.background_colors.top_connector.empty())
          border.background_colors.top_connector.clear();

        border.background_colors.top_connector = CSI + std::to_string(static_cast<int>(background_color)) + "m";

        return *this;
      }

      BackgroundColoring& top_connector(RGB rgb) {
        if (!border.background_colors.top_connector.empty())
          border.background_colors.top_connector.clear();

        border.background_colors.top_connector = CSI "48;2;" + std::to_string(rgb.r) + ";" +
                                                 std::to_string(rgb.g) + ";" +
                                                 std::to_string(rgb.b) + "m";

        return *this;
      }

      /* ------------------ vertical ------------------------ */
      BackgroundColoring& vertical(BackgroundColor background_color) {
        if (!border.background_colors.vertical.empty())
          border.background_colors.vertical.clear();

        border.background_colors.vertical = CSI + std::to_string(static_cast<int>(background_color)) + "m";

        left_connector(background_color);
        left_connector(background_color);

        corners(background_color);

        return *this;
      }

      BackgroundColoring& vertical(RGB rgb) {
        if (!border.background_colors.vertical.empty())
          border.background_colors.vertical.clear();

        border.background_colors.vertical = CSI "48;2;" + std::to_string(rgb.r) + ";" +
                                            std::to_string(rgb.g) + ";" +
                                            std::to_string(rgb.b) + "m";

        left_connector(rgb);
        left_connector(rgb);

        corners(rgb);

        return *this;
      }

      BackgroundColoring& left_connector(BackgroundColor background_color) {
        if (!border.background_colors.left_connector.empty())
          border.background_colors.left_connector.clear();

        border.background_colors.left_connector = CSI + std::to_string(static_cast<int>(background_color)) + "m";

        return *this;
      }

      BackgroundColoring& left_connector(RGB rgb) {
        if (!border.background_colors.left_connector.empty())
          border.background_colors.left_connector.clear();

        border.background_colors.left_connector = CSI "48;2;" + std::to_string(rgb.r) + ";" +
                                                  std::to_string(rgb.g) + ";" +
                                                  std::to_string(rgb.b) + "m";

        return *this;
      }

      BackgroundColoring& right_connector(BackgroundColor background_color) {
        if (!border.background_colors.right_connector.empty())
          border.background_colors.right_connector.clear();

        border.background_colors.right_connector = CSI + std::to_string(static_cast<int>(background_color)) + "m";

        return *this;
      }

      BackgroundColoring& right_connector(RGB rgb) {
        if (!border.background_colors.right_connector.empty())
          border.background_colors.right_connector.clear();

        border.background_colors.right_connector = CSI "48;2;" + std::to_string(rgb.r) + ";" +
                                                   std::to_string(rgb.g) + ";" +
                                                   std::to_string(rgb.b) + "m";

        return *this;
      }

      /* ------------------ corners ------------------------ */
      BackgroundColoring& corners(BackgroundColor background_color) {
        top_left_corner(background_color);
        top_right_corner(background_color);

        bottom_left_corner(background_color);
        bottom_right_corner(background_color);

        return *this;
      }

      BackgroundColoring& corners(RGB rgb) {
        top_left_corner(rgb);
        top_right_corner(rgb);

        bottom_left_corner(rgb);
        bottom_right_corner(rgb);

        return *this;
      }

      BackgroundColoring& top_left_corner(BackgroundColor background_color) {
        if (!border.background_colors.top_left_corner.empty())
          border.background_colors.top_left_corner.clear();

        border.background_colors.top_left_corner = CSI + std::to_string(static_cast<int>(background_color)) + "m";

        return *this;
      }

      BackgroundColoring& top_left_corner(RGB rgb) {
        if (!border.background_colors.top_left_corner.empty())
          border.background_colors.top_left_corner.clear();

        border.background_colors.top_left_corner = CSI "48;2;" + std::to_string(rgb.r) + ";" +
                                                   std::to_string(rgb.g) + ";" +
                                                   std::to_string(rgb.b) + "m";

        return *this;
      }

      BackgroundColoring& top_right_corner(BackgroundColor background_color) {
        if (!border.background_colors.top_right_corner.empty())
          border.background_colors.top_right_corner.clear();

        border.background_colors.top_right_corner = CSI + std::to_string(static_cast<int>(background_color)) + "m";

        return *this;
      }

      BackgroundColoring& top_right_corner(RGB rgb) {
        if (!border.background_colors.top_right_corner.empty())
          border.background_colors.top_right_corner.clear();

        border.background_colors.top_right_corner = CSI "48;2;" + std::to_string(rgb.r) + ";" +
                                                    std::to_string(rgb.g) + ";" +
                                                    std::to_string(rgb.b) + "m";

        return *this;
      }

      BackgroundColoring& bottom_left_corner(BackgroundColor background_color) {
        if (!border.background_colors.bottom_left_corner.empty())
          border.background_colors.bottom_left_corner.clear();

        border.background_colors.bottom_left_corner = CSI + std::to_string(static_cast<int>(background_color)) + "m";

        return *this;
      }

      BackgroundColoring& bottom_left_corner(RGB rgb) {
        if (!border.background_colors.bottom_left_corner.empty())
          border.background_colors.bottom_left_corner.clear();

        border.background_colors.bottom_left_corner = CSI "48;2;" + std::to_string(rgb.r) + ";" +
                                                      std::to_string(rgb.g) + ";" +
                                                      std::to_string(rgb.b) + "m";

        return *this;
      }

      BackgroundColoring& bottom_right_corner(BackgroundColor background_color) {
        if (!border.background_colors.bottom_right_corner.empty())
          border.background_colors.bottom_right_corner.clear();

        border.background_colors.bottom_right_corner = CSI + std::to_string(static_cast<int>(background_color)) + "m";

        return *this;
      }

      BackgroundColoring& bottom_right_corner(RGB rgb) {
        if (!border.background_colors.bottom_right_corner.empty())
          border.background_colors.bottom_right_corner.clear();

        border.background_colors.bottom_right_corner = CSI "48;2;" + std::to_string(rgb.r) + ";" +
                                                       std::to_string(rgb.g) + ";" +
                                                       std::to_string(rgb.b) + "m";

        return *this;
      }

      /* ------------------ intersection point ------------------------ */
      BackgroundColoring& intersection(BackgroundColor background_color) {
        if (!border.background_colors.intersection.empty())
          border.background_colors.intersection.clear();

        border.background_colors.intersection = CSI + std::to_string(static_cast<int>(background_color)) + "m";

        return *this;
      }

      BackgroundColoring& intersection(RGB rgb) {
        if (!border.background_colors.intersection.empty())
          border.background_colors.intersection.clear();

        border.background_colors.intersection = CSI "48;2;" + std::to_string(rgb.r) + ";" +
                                                std::to_string(rgb.g) + ";" +
                                                std::to_string(rgb.b) + "m";

        return *this;
      }
    };

public:
    Border() : style(BorderStyle::standard) {}

    Getters get() { return Getters(*this); }

    Setters set() { return Setters(*this); }

    Coloring color() { return Coloring(*this); }

    BackgroundColoring background_color() { return BackgroundColoring(*this); }
  };
} // namespace tabular

#endif // TABULAR_BORDER_HPP
