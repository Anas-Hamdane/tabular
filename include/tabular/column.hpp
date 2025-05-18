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

#include <algorithm>
#include <unordered_set>

#include <tabular/definitions.hpp>
#include <tabular/global_vars.hpp>
#include <tabular/style.hpp>

namespace tabular {
    class Column {
        FontStylesVector font_styles;
        StringVector splitted_content;
        StringList words;
        Alignment alignment;
        unsigned int width;
        unsigned int top_padding;
        unsigned int bottom_padding;

        class Config {
            Column& column;

            class Fonts {
                Column& column;

            public:
                Fonts(Column& column) : column(column) {}

                Fonts& add(FontStylesVector styles) {
                    column.font_styles.insert(column.font_styles.begin(), styles.begin(), styles.end());

                    return *this;
                }

                // removing every element of font_styles exist in this->font_styles
                Fonts& remove(FontStylesVector styles) {

                    // better performance
                    std::unordered_set<FontStyle> to_remove(styles.begin(), styles.end());

                    column.font_styles.erase(
                        std::remove_if(
                            column.font_styles.begin(),
                            column.font_styles.end(),

                            [&to_remove](FontStyle font_style) {
                                return to_remove.find(font_style) != to_remove.end();
                            }),

                        column.font_styles.end());

                    return *this;
                }

            };

        public:
            Config(Column& column) : column(column) {}

            Fonts fonts() { return Fonts(column); }

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

        class Getters {
            Column& column;

        public:
            Getters(Column& column) : column(column) {}

            Alignment alignment() { return column.alignment; }

            unsigned int width() { return column.width; }

            unsigned int top_padding() { return column.top_padding; }

            unsigned int bottom_padding() { return column.bottom_padding; }

            StringVector splitted_content() { return column.splitted_content; }

            FontStylesVector font_styles() { return column.font_styles; }

            StringList words() { return column.words; }
        };

        class Setters {
            Column& column;

        public:
            Setters(Column& column) : column(column) {}

            Setters& splitted_content(StringVector splittedContent) {
                column.splitted_content = splittedContent;
                return *this;
            }

            Setters& words(StringList words) {
                column.words = words;
                return *this;
            }

            Setters& width(int width) {
                if (width <= 0)
                    width = 0;

                column.width = static_cast<unsigned int>(width);

                return *this;
            }
        };

    public:
        std::string content;

        Column(std::string content)
            : content(content), alignment(Alignment::left), width(0), top_padding(0), bottom_padding(0){};

        Config config() { return Config(*this); }

        Getters get() { return Getters(*this); }

        Setters set() { return Setters(*this); }
    };
} // namespace tabular

#endif // TABULAR_COLUMN_HPP