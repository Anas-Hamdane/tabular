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

    public:
        std::string content;

        Column(std::string content)
            : content(content), alignment(Alignment::left), width(0), top_padding(0), bottom_padding(0) {};

        void set_column_align(Alignment alignment) { this->alignment = alignment; }

        Alignment get_column_align() { return alignment; }

        Column& set_width(int width) {
            if (width <= 0)
                width = 0;

            this->width = static_cast<unsigned int>(width);

            return *this;
        }

        unsigned int get_width() { return this->width; }

        Column& set_column_padding(int padding) {
            if (padding <= 0)
                padding = 0;

            this->top_padding = padding;
            this->bottom_padding = padding;

            return *this;
        }

        Column& set_column_top_padding(int padding) {
            if (padding <= 0)
                this->top_padding = 0;
            else
                this->top_padding = static_cast<unsigned int>(padding);

            return *this;
        }

        Column& set_column_bottom_padding(int padding) {
            if (padding <= 0)
                this->bottom_padding = 0;
            else
                this->bottom_padding = static_cast<unsigned int>(padding);

            return *this;
        }

        Column& col_font_styles(FontStylesVector font_styles) {
            this->font_styles.insert(this->font_styles.begin(), font_styles.begin(), font_styles.end());

            return *this;
        }

        // removing every element of font_styles exist in this->font_styles
        Column& remove_font_styles(FontStylesVector font_styles) {

            // better performance
            std::unordered_set<FontStyle> to_remove(font_styles.begin(), font_styles.end());

            this->font_styles.erase(
                std::remove_if(
                    this->font_styles.begin(),
                    this->font_styles.end(),

                    [&to_remove](FontStyle font_style) {
                        return to_remove.find(font_style) != to_remove.end();
                    }),

                this->font_styles.end());

            return *this;
        }

        unsigned int get_top_padding() { return top_padding; }

        unsigned int get_bottom_padding() { return bottom_padding; }

        void set_splitted_content(StringVector splittedContent) { this->splitted_content = splittedContent; }

        StringVector get_splitted_content() { return splitted_content; }

        void set_words(StringList words) { this->words = words; }

        StringList get_words() { return words; }

        FontStylesVector get_font_styles() { return font_styles; }
    };
} // namespace tabular

#endif // TABULAR_COLUMN_HPP