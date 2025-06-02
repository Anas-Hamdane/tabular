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
#include <vector>

#include <tabular/alignment.hpp>
#include <tabular/colors.hpp>
#include <tabular/definitions.hpp>
#include <tabular/rgb.hpp>
#include <tabular/styles.hpp>

#include <tabular/maps.hpp>

namespace tabular {
    class Column {
        // styles applied to the whole content OR a specific sub string
        std::string global_styles;
        std::vector<std::string> splitted_content;
        std::list<std::string> words;
        Alignment alignment;
        unsigned int width;
        unsigned int top_padding;
        unsigned int bottom_padding;
        unsigned int special_characters;

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
            Config& text_style(Style style) {
                // style statement
                std::string style_stt = CSI + std::to_string(static_cast<int>(style)) + "m";

                column.global_styles += style_stt;

                // later add special_characters of RESET in utils in the last element of splitted_content
                return *this;
            }

            // add multiple text styles to the whole column
            Config& text_style(const std::vector<Style>& styles) {
                if (styles.empty()) return *this;

                // Apply all styles at the beginning
                std::string styles_stt;
                for (const auto& style : styles) {
                    styles_stt += CSI + std::to_string(static_cast<int>(style)) + "m";
                }

                column.global_styles += styles_stt;

                return *this;
            }

            // add Background Color to the whole column
            Config& background_color(BackgroundColor color) {

                // Background Color statement
                std::string back_color_stt = CSI + std::to_string(static_cast<int>(color)) + "m";

                column.global_styles += back_color_stt;

                return *this;
            }

            // add Color to the whole column
            Config& color(Color color) {

                // Color statement
                std::string color_stt = CSI + std::to_string(static_cast<int>(color)) + "m";

                column.global_styles += color_stt;

                return *this;
            }

            // add RGB to the whole column
            Config& rgb(RGB rgb) {

                // RGB statement
                std::string rgb_stt = CSI "38;2;" + std::to_string(rgb.r) + ";" +
                                      std::to_string(rgb.g) + ";" +
                                      std::to_string(rgb.b) + "m";

                column.global_styles += rgb_stt;
                return *this;
            }

            // add background RGB to the whole column
            Config& background_rgb(RGB rgb) {

                // RGB statement
                std::string back_rgb_stt = CSI "48;2;" + std::to_string(rgb.r) + ";" +
                                           std::to_string(rgb.g) + ";" +
                                           std::to_string(rgb.b) + "m";

                column.global_styles += back_rgb_stt;

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

            unsigned int special_characters() { return column.special_characters; }

            std::vector<std::string> splitted_content() { return column.splitted_content; }

            std::list<std::string> words() { return column.words; }

            std::string global_styles() { return column.global_styles; }
        };

        class Setters {
            Column& column;

        public:
            Setters(Column& column) : column(column) {}

            Setters& splitted_content(std::vector<std::string> splittedContent) {
                column.splitted_content = splittedContent;
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
        };

    public:
        std::string content;

        Column(std::string content)
            : content(content), alignment(Alignment::left), width(0), top_padding(0), bottom_padding(0), special_characters(0) {};

        Config config() { return Config(*this); }

        Getters get() { return Getters(*this); }

        Setters set() { return Setters(*this); }
    };
} // namespace tabular

#endif // TABULAR_COLUMN_HPP
