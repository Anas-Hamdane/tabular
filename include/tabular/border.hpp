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

#include <array>
#include <string>
#include <tabular/row.hpp>

namespace tabular {
  enum class BorderStyle : size_t {
    empty = 0,
    standard,
    space,
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

    bool disabled_styles;

    mutable BorderGlyphs cached_glyphs;
    mutable bool valid_cache;

    static std::string solve_color(Color color, const bool is_background) {
      const int code = static_cast<int>(color) + (is_background ? 10 : 0);
      return ansi::CSI + std::to_string(code) + ansi::suffix;
    }

    static std::string solve_color(const RGB rgb, const bool is_background) {
      std::string result;
      result.reserve(20);

      result.append(ansi::CSI);

      result.append(is_background ? "48;2;" : "38;2;");

      result.append(std::to_string(rgb.r) + ";" +
                    std::to_string(rgb.g) + ";" +
                    std::to_string(rgb.b) + "m");
      return result;
    }

    void invalidate_cache() const {
      if (this->valid_cache)
        this->valid_cache = false;
    }

    class Getters {
      Border& border;

  public:
      explicit Getters(Border& border) : border(border) {}

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
      explicit Parts(Border& border) : border(border) {}

      /*
       * Note: It is the user's responsibility to ensure that each border glyph is a single
       * terminal column wide. If a string with a visual width > 1 is provided, the table
       * layout may break.
       *
       * Using str.length() is unreliable for multibyte characters,
       * and calling custom display width functions on every
       * assignment would introduce unnecessary performance overhead.
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
      explicit Setters(Border& border) : border(border) {}

      Setters& disabled_styles(const bool is_disabled) {
        border.disabled_styles = is_disabled;
        border.invalidate_cache();
        return *this;
      }

      Setters& style(const BorderStyle style) {
        border.style = style;
        border.invalidate_cache();
        return *this;
      }
    };

    class Coloring {
      Border& border;

  public:
      explicit Coloring(Border& border) : border(border) {}

      /* ------------------ full ------------------------ */
      Coloring& full(const Color color) {
        horizontal(color);
        vertical(color);

        return *this;
      }

      Coloring& full(const RGB rgb) {
        horizontal(rgb);
        vertical(rgb);

        return *this;
      }

      /* ------------------ horizontal ------------------------ */
      Coloring& horizontal(const Color color) {
        border.colors.horizontal = tabular::Border::solve_color(color, false);

        top_connector(color);
        bottom_connector(color);
        intersection(color);

        corners(color);

        return *this;
      }

      Coloring& horizontal(const RGB rgb) {
        border.colors.horizontal = tabular::Border::solve_color(rgb, false);

        top_connector(rgb);
        bottom_connector(rgb);
        intersection(rgb);

        corners(rgb);

        return *this;
      }

      Coloring& bottom_connector(const Color color) {
        border.colors.bottom_connector = tabular::Border::solve_color(color, false);

        border.invalidate_cache();
        return *this;
      }

      Coloring& bottom_connector(const RGB rgb) {
        border.colors.bottom_connector = tabular::Border::solve_color(rgb, false);

        border.invalidate_cache();
        return *this;
      }

      Coloring& top_connector(const Color color) {
        border.colors.top_connector = tabular::Border::solve_color(color, false);

        border.invalidate_cache();
        return *this;
      }

      Coloring& top_connector(const RGB rgb) {
        border.colors.top_connector = tabular::Border::solve_color(rgb, false);

        border.invalidate_cache();
        return *this;
      }

      /* ------------------ vertical ------------------------ */
      Coloring& vertical(const Color color) {
        border.colors.vertical = tabular::Border::solve_color(color, false);

        left_connector(color);
        right_connector(color);

        corners(color);

        border.invalidate_cache();
        return *this;
      }

      Coloring& vertical(const RGB rgb) {
        border.colors.vertical = tabular::Border::solve_color(rgb, false);

        left_connector(rgb);
        right_connector(rgb);

        corners(rgb);

        border.invalidate_cache();
        return *this;
      }

      Coloring& left_connector(const Color color) {
        border.colors.left_connector = tabular::Border::solve_color(color, false);

        border.invalidate_cache();
        return *this;
      }

      Coloring& left_connector(const RGB rgb) {
        border.colors.left_connector = tabular::Border::solve_color(rgb, false);

        border.invalidate_cache();
        return *this;
      }

      Coloring& right_connector(const Color color) {
        border.colors.right_connector = tabular::Border::solve_color(color, false);

        border.invalidate_cache();
        return *this;
      }

      Coloring& right_connector(const RGB rgb) {
        border.colors.right_connector = tabular::Border::solve_color(rgb, false);

        border.invalidate_cache();
        return *this;
      }

      /* ------------------ corners ------------------------ */
      Coloring& corners(const Color color) {
        top_left_corner(color);
        top_right_corner(color);

        bottom_left_corner(color);
        bottom_right_corner(color);

        return *this;
      }

      Coloring& corners(const RGB rgb) {
        top_left_corner(rgb);
        top_right_corner(rgb);

        bottom_left_corner(rgb);
        bottom_right_corner(rgb);

        return *this;
      }

      Coloring& top_left_corner(const Color color) {
        border.colors.top_left_corner = tabular::Border::solve_color(color, false);

        border.invalidate_cache();
        return *this;
      }

      Coloring& top_left_corner(const RGB rgb) {
        border.colors.top_left_corner = tabular::Border::solve_color(rgb, false);

        border.invalidate_cache();
        return *this;
      }

      Coloring& top_right_corner(const Color color) {
        border.colors.top_right_corner = tabular::Border::solve_color(color, false);

        border.invalidate_cache();
        return *this;
      }

      Coloring& top_right_corner(const RGB rgb) {
        border.colors.top_right_corner = tabular::Border::solve_color(rgb, false);

        border.invalidate_cache();
        return *this;
      }

      Coloring& bottom_left_corner(const Color color) {
        border.colors.bottom_left_corner = tabular::Border::solve_color(color, false);

        border.invalidate_cache();
        return *this;
      }

      Coloring& bottom_left_corner(const RGB rgb) {
        border.colors.bottom_left_corner = tabular::Border::solve_color(rgb, false);

        border.invalidate_cache();
        return *this;
      }

      Coloring& bottom_right_corner(const Color color) {
        border.colors.bottom_right_corner = tabular::Border::solve_color(color, false);

        border.invalidate_cache();
        return *this;
      }

      Coloring& bottom_right_corner(const RGB rgb) {
        border.colors.bottom_right_corner = tabular::Border::solve_color(rgb, false);

        border.invalidate_cache();
        return *this;
      }

      /* ------------------ intersection point ------------------------ */
      Coloring& intersection(const Color color) {
        border.colors.intersection = tabular::Border::solve_color(color, false);

        border.invalidate_cache();
        return *this;
      }

      Coloring& intersection(const RGB rgb) {
        border.colors.intersection = tabular::Border::solve_color(rgb, false);

        border.invalidate_cache();
        return *this;
      }
    };

    class BackgroundColoring {
      Border& border;

  public:
      explicit BackgroundColoring(Border& border) : border(border) {}

      /* ------------------ full ------------------------ */
      BackgroundColoring& full(const Color color) {
        horizontal(color);
        vertical(color);

        return *this;
      }

      BackgroundColoring& full(const RGB rgb) {
        horizontal(rgb);
        vertical(rgb);

        return *this;
      }

      /* ------------------ horizontal ------------------------ */
      BackgroundColoring& horizontal(const Color color) {
        border.background_colors.horizontal = tabular::Border::solve_color(color, true);

        top_connector(color);
        bottom_connector(color);
        intersection(color);

        corners(color);

        return *this;
      }

      BackgroundColoring& horizontal(const RGB rgb) {
        border.background_colors.horizontal = tabular::Border::solve_color(rgb, true);

        top_connector(rgb);
        bottom_connector(rgb);
        intersection(rgb);

        corners(rgb);

        return *this;
      }

      BackgroundColoring& bottom_connector(const Color color) {
        border.background_colors.bottom_connector = tabular::Border::solve_color(color, true);

        border.invalidate_cache();
        return *this;
      }

      BackgroundColoring& bottom_connector(const RGB rgb) {
        border.background_colors.bottom_connector = tabular::Border::solve_color(rgb, true);

        border.invalidate_cache();
        return *this;
      }

      BackgroundColoring& top_connector(const Color color) {
        border.background_colors.top_connector = tabular::Border::solve_color(color, true);

        border.invalidate_cache();
        return *this;
      }

      BackgroundColoring& top_connector(const RGB rgb) {
        border.background_colors.top_connector = tabular::Border::solve_color(rgb, true);

        border.invalidate_cache();
        return *this;
      }

      /* ------------------ vertical ------------------------ */
      BackgroundColoring& vertical(const Color color) {
        border.background_colors.vertical = tabular::Border::solve_color(color, true);

        left_connector(color);
        right_connector(color);

        corners(color);

        border.invalidate_cache();
        return *this;
      }

      BackgroundColoring& vertical(const RGB rgb) {
        border.background_colors.vertical = tabular::Border::solve_color(rgb, true);

        left_connector(rgb);
        right_connector(rgb);

        corners(rgb);

        border.invalidate_cache();
        return *this;
      }

      BackgroundColoring& left_connector(const Color color) {
        border.background_colors.left_connector = tabular::Border::solve_color(color, true);

        border.invalidate_cache();
        return *this;
      }

      BackgroundColoring& left_connector(const RGB rgb) {
        border.background_colors.left_connector = tabular::Border::solve_color(rgb, true);

        border.invalidate_cache();
        return *this;
      }

      BackgroundColoring& right_connector(const Color color) {
        border.background_colors.right_connector = tabular::Border::solve_color(color, true);

        border.invalidate_cache();
        return *this;
      }

      BackgroundColoring& right_connector(const RGB rgb) {
        border.background_colors.right_connector = tabular::Border::solve_color(rgb, true);

        border.invalidate_cache();
        return *this;
      }

      /* ------------------ corners ------------------------ */
      BackgroundColoring& corners(const Color color) {
        top_left_corner(color);
        top_right_corner(color);

        bottom_left_corner(color);
        bottom_right_corner(color);

        return *this;
      }

      BackgroundColoring& corners(const RGB rgb) {
        top_left_corner(rgb);
        top_right_corner(rgb);

        bottom_left_corner(rgb);
        bottom_right_corner(rgb);

        return *this;
      }

      BackgroundColoring& top_left_corner(const Color color) {
        border.background_colors.top_left_corner = tabular::Border::solve_color(color, true);

        border.invalidate_cache();
        return *this;
      }

      BackgroundColoring& top_left_corner(const RGB rgb) {
        border.background_colors.top_left_corner = tabular::Border::solve_color(rgb, true);

        border.invalidate_cache();
        return *this;
      }

      BackgroundColoring& top_right_corner(const Color color) {
        border.background_colors.top_right_corner = tabular::Border::solve_color(color, true);

        border.invalidate_cache();
        return *this;
      }

      BackgroundColoring& top_right_corner(const RGB rgb) {
        border.background_colors.top_right_corner = tabular::Border::solve_color(rgb, true);

        border.invalidate_cache();
        return *this;
      }

      BackgroundColoring& bottom_left_corner(const Color color) {
        border.background_colors.bottom_left_corner = tabular::Border::solve_color(color, true);

        border.invalidate_cache();
        return *this;
      }

      BackgroundColoring& bottom_left_corner(const RGB rgb) {
        border.background_colors.bottom_left_corner = tabular::Border::solve_color(rgb, true);

        border.invalidate_cache();
        return *this;
      }

      BackgroundColoring& bottom_right_corner(const Color color) {
        border.background_colors.bottom_right_corner = tabular::Border::solve_color(color, true);

        border.invalidate_cache();
        return *this;
      }

      BackgroundColoring& bottom_right_corner(const RGB rgb) {
        border.background_colors.bottom_right_corner = tabular::Border::solve_color(rgb, true);

        border.invalidate_cache();
        return *this;
      }

      /* ------------------ intersection point ------------------------ */
      BackgroundColoring& intersection(const Color color) {
        border.background_colors.intersection = tabular::Border::solve_color(color, true);

        border.invalidate_cache();
        return *this;
      }

      BackgroundColoring& intersection(const RGB rgb) {
        border.background_colors.intersection = tabular::Border::solve_color(rgb, true);

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
} // namespace tabular

#endif // TABULAR_BORDER_HPP
