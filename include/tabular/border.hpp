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

#include <tabular/row.hpp>

namespace tabular {
  enum class BorderStyle {
    standard,
    space,
    empty,
    ansi
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
    BorderGlyphs glyphs;
    BorderStyle style;

    BorderGlyphs colors;
    BorderGlyphs background_colors;

    bool is_multi_byte;
    bool disabled_styles;

    mutable BorderGlyphs cached_glyphs;
    mutable bool valid_cache;

    static std::string solve_color(Color color, bool is_background) {
      std::string result;
      result.reserve(6);

      result.append(ansi::CSI + std::to_string(static_cast<int>(color) + (is_background ? 10 : 0)) + "m");
      return result;
    }

    static std::string solve_color(RGB rgb, bool is_background) {
      std::string result;
      result.reserve(20);

      result.append(ansi::CSI);
      result.append(is_background ? "48;" : "38;");
      result.append("2;" + std::to_string(rgb.r) + ";" +
                    std::to_string(rgb.g) + ";" +
                    std::to_string(rgb.b) + "m");
      return result;
    }

    class Getters {
      Border& border;

  public:
      Getters(Border& border) : border(border) {}

      static const BorderGlyphs& style_templates(BorderStyle style) {
        static const std::map<BorderStyle, BorderGlyphs> templates = {
            {BorderStyle::space, {" ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " "}},
            {BorderStyle::empty, {"", "", "", "", "", "", "", "", "", "", ""}},
            {BorderStyle::standard, {"|", "-", "+", "+", "+", "+", "+", "+", "+", "+", "+"}},
            {BorderStyle::ansi, {
                                    std::string("\033(0") + "x" + std::string("\033(B"), // vertical
                                    std::string("\033(0") + "q" + std::string("\033(B"), // horizontal
                                    std::string("\033(0") + "j" + std::string("\033(B"), // bottom_right_corner
                                    std::string("\033(0") + "k" + std::string("\033(B"), // top_right_corner
                                    std::string("\033(0") + "l" + std::string("\033(B"), // top_left_corner
                                    std::string("\033(0") + "m" + std::string("\033(B"), // bottom_left_corner
                                    std::string("\033(0") + "n" + std::string("\033(B"), // intersection
                                    std::string("\033(0") + "t" + std::string("\033(B"), // left_connector
                                    std::string("\033(0") + "u" + std::string("\033(B"), // right_connector
                                    std::string("\033(0") + "v" + std::string("\033(B"), // bottom_connector
                                    std::string("\033(0") + "w" + std::string("\033(B")  // top_connector
                                }}};
        return templates.at(style);
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

        auto wrap = [](const std::string& fg, const std::string& bg, const std::string& val, const Border& border) -> std::string {
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

    class Setters {
      Border& border;

      void invalidate_cache() {
        if (border.valid_cache)
          border.valid_cache = false;
      }

  public:
      Setters(Border& border) : border(border) {}

      /*
       * Note: It is the user's responsibility to ensure that each border glyph is a single
       * terminal column wide. If a string with a visual width > 1 is provided, the table
       * layout may break.
       *
       * Using str.length() is unreliable for multi-byte characters, and calling utils::mbswidth()
       * on every assignment would introduce unnecessary performance overhead.
       *
       * — Anas Hamdane, 2025-06-14
       */

      Setters& style(BorderStyle style) {
        border.style = style;
        invalidate_cache();
        return *this;
      }

      Setters& horizontal(std::string horizontal) {
        border.glyphs.horizontal = horizontal;
        invalidate_cache();
        return *this;
      }

      Setters& vertical(std::string vertical) {
        border.glyphs.vertical = vertical;
        invalidate_cache();
        return *this;
      }

      Setters& corner(std::string corner) {
        border.glyphs.bottom_right_corner = corner;
        border.glyphs.top_right_corner = corner;

        border.glyphs.bottom_left_corner = corner;
        border.glyphs.top_left_corner = corner;

        invalidate_cache();
        return *this;
      }

      Setters& top_left_corner(std::string corner) {
        border.glyphs.top_left_corner = corner;
        invalidate_cache();
        return *this;
      }

      Setters& top_right_corner(std::string corner) {
        border.glyphs.top_right_corner = corner;
        invalidate_cache();
        return *this;
      }

      Setters& bottom_left_corner(std::string corner) {
        border.glyphs.bottom_left_corner = corner;
        invalidate_cache();
        return *this;
      }

      Setters& bottom_right_corner(std::string corner) {
        border.glyphs.bottom_right_corner = corner;
        invalidate_cache();
        return *this;
      }

      Setters& intersection(std::string intersection) {
        border.glyphs.intersection = intersection;
        invalidate_cache();
        return *this;
      }

      Setters& left_connector(std::string connector) {
        border.glyphs.left_connector = connector;
        invalidate_cache();
        return *this;
      }

      Setters& right_connector(std::string connector) {
        border.glyphs.right_connector = connector;
        invalidate_cache();
        return *this;
      }

      Setters& top_connector(std::string connector) {
        border.glyphs.top_connector = connector;
        invalidate_cache();
        return *this;
      }

      Setters& bottom_connector(std::string connector) {
        border.glyphs.bottom_connector = connector;
        invalidate_cache();
        return *this;
      }

      Setters& disabled_styles(bool is_disabled) {
        border.disabled_styles = is_disabled;
        return *this;
      }
    };

    class Coloring {
      Border& border;

      void invalidate_cache() {
        if (border.valid_cache)
          border.valid_cache = false;
      }

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

        invalidate_cache();
        return *this;
      }

      Coloring& bottom_connector(RGB rgb) {
        border.colors.bottom_connector = border.solve_color(rgb, false);

        invalidate_cache();
        return *this;
      }

      Coloring& top_connector(Color color) {
        border.colors.top_connector = border.solve_color(color, false);

        invalidate_cache();
        return *this;
      }

      Coloring& top_connector(RGB rgb) {
        border.colors.top_connector = border.solve_color(rgb, false);

        invalidate_cache();
        return *this;
      }

      /* ------------------ vertical ------------------------ */
      Coloring& vertical(Color color) {
        border.colors.vertical = border.solve_color(color, false);

        left_connector(color);
        right_connector(color);

        corners(color);

        invalidate_cache();
        return *this;
      }

      Coloring& vertical(RGB rgb) {
        border.colors.vertical = border.solve_color(rgb, false);

        left_connector(rgb);
        right_connector(rgb);

        corners(rgb);

        invalidate_cache();
        return *this;
      }

      Coloring& left_connector(Color color) {
        border.colors.left_connector = border.solve_color(color, false);

        invalidate_cache();
        return *this;
      }

      Coloring& left_connector(RGB rgb) {
        border.colors.left_connector = border.solve_color(rgb, false);

        invalidate_cache();
        return *this;
      }

      Coloring& right_connector(Color color) {
        border.colors.right_connector = border.solve_color(color, false);

        invalidate_cache();
        return *this;
      }

      Coloring& right_connector(RGB rgb) {
        border.colors.right_connector = border.solve_color(rgb, false);

        invalidate_cache();
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

        invalidate_cache();
        return *this;
      }

      Coloring& top_left_corner(RGB rgb) {
        border.colors.top_left_corner = border.solve_color(rgb, false);

        invalidate_cache();
        return *this;
      }

      Coloring& top_right_corner(Color color) {
        border.colors.top_right_corner = border.solve_color(color, false);

        invalidate_cache();
        return *this;
      }

      Coloring& top_right_corner(RGB rgb) {
        border.colors.top_right_corner = border.solve_color(rgb, false);

        invalidate_cache();
        return *this;
      }

      Coloring& bottom_left_corner(Color color) {
        border.colors.bottom_left_corner = border.solve_color(color, false);

        invalidate_cache();
        return *this;
      }

      Coloring& bottom_left_corner(RGB rgb) {
        border.colors.bottom_left_corner = border.solve_color(rgb, false);

        invalidate_cache();
        return *this;
      }

      Coloring& bottom_right_corner(Color color) {
        border.colors.bottom_right_corner = border.solve_color(color, false);

        invalidate_cache();
        return *this;
      }

      Coloring& bottom_right_corner(RGB rgb) {
        border.colors.bottom_right_corner = border.solve_color(rgb, false);

        invalidate_cache();
        return *this;
      }

      /* ------------------ intersection point ------------------------ */
      Coloring& intersection(Color color) {
        border.colors.intersection = border.solve_color(color, false);

        invalidate_cache();
        return *this;
      }

      Coloring& intersection(RGB rgb) {
        border.colors.intersection = border.solve_color(rgb, false);

        invalidate_cache();
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

        invalidate_cache();
        return *this;
      }

      BackgroundColoring& bottom_connector(RGB rgb) {
        border.background_colors.bottom_connector = border.solve_color(rgb, true);

        invalidate_cache();
        return *this;
      }

      BackgroundColoring& top_connector(Color color) {
        border.background_colors.top_connector = border.solve_color(color, true);

        invalidate_cache();
        return *this;
      }

      BackgroundColoring& top_connector(RGB rgb) {
        border.background_colors.top_connector = border.solve_color(rgb, true);

        invalidate_cache();
        return *this;
      }

      /* ------------------ vertical ------------------------ */
      BackgroundColoring& vertical(Color color) {
        border.background_colors.vertical = border.solve_color(color, true);

        left_connector(color);
        right_connector(color);

        corners(color);

        invalidate_cache();
        return *this;
      }

      BackgroundColoring& vertical(RGB rgb) {
        border.background_colors.vertical = border.solve_color(rgb, true);

        left_connector(rgb);
        right_connector(rgb);

        corners(rgb);

        invalidate_cache();
        return *this;
      }

      BackgroundColoring& left_connector(Color color) {
        border.background_colors.left_connector = border.solve_color(color, true);

        invalidate_cache();
        return *this;
      }

      BackgroundColoring& left_connector(RGB rgb) {
        border.background_colors.left_connector = border.solve_color(rgb, true);

        invalidate_cache();
        return *this;
      }

      BackgroundColoring& right_connector(Color color) {
        border.background_colors.right_connector = border.solve_color(color, true);

        invalidate_cache();
        return *this;
      }

      BackgroundColoring& right_connector(RGB rgb) {
        border.background_colors.right_connector = border.solve_color(rgb, true);

        invalidate_cache();
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

        invalidate_cache();
        return *this;
      }

      BackgroundColoring& top_left_corner(RGB rgb) {
        border.background_colors.top_left_corner = border.solve_color(rgb, true);

        invalidate_cache();
        return *this;
      }

      BackgroundColoring& top_right_corner(Color color) {
        border.background_colors.top_right_corner = border.solve_color(color, true);

        invalidate_cache();
        return *this;
      }

      BackgroundColoring& top_right_corner(RGB rgb) {
        border.background_colors.top_right_corner = border.solve_color(rgb, true);

        invalidate_cache();
        return *this;
      }

      BackgroundColoring& bottom_left_corner(Color color) {
        border.background_colors.bottom_left_corner = border.solve_color(color, true);

        invalidate_cache();
        return *this;
      }

      BackgroundColoring& bottom_left_corner(RGB rgb) {
        border.background_colors.bottom_left_corner = border.solve_color(rgb, true);

        invalidate_cache();
        return *this;
      }

      BackgroundColoring& bottom_right_corner(Color color) {
        border.background_colors.bottom_right_corner = border.solve_color(color, true);

        invalidate_cache();
        return *this;
      }

      BackgroundColoring& bottom_right_corner(RGB rgb) {
        border.background_colors.bottom_right_corner = border.solve_color(rgb, true);

        invalidate_cache();
        return *this;
      }

      /* ------------------ intersection point ------------------------ */
      BackgroundColoring& intersection(Color color) {
        border.background_colors.intersection = border.solve_color(color, true);

        invalidate_cache();
        return *this;
      }

      BackgroundColoring& intersection(RGB rgb) {
        border.background_colors.intersection = border.solve_color(rgb, true);

        invalidate_cache();
        return *this;
      }
    };

public:
    Border() : style(BorderStyle::standard), disabled_styles(false), valid_cache(false) {}

    Getters get() { return Getters(*this); }

    Setters set() { return Setters(*this); }

    Coloring color() { return Coloring(*this); }

    BackgroundColoring background_color() { return BackgroundColoring(*this); }
  };
} // namespace tabular

#endif // TABULAR_BORDER_HPP
