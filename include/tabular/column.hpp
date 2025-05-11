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

#include <tabular/global_vars.hpp>
#include <tabular/definitions.hpp>

namespace tabular {
    class Column {
        std::vector<FontStyle> font_styles;
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

        void set_width(int width) {
            if (width <= 0)
                width = 0;

            this->width = static_cast<unsigned int>(width);
        }

        unsigned int get_width() { return this->width; }

        void set_column_padding(int padding) {
            if (padding <= 0)
                padding = 0;

            this->top_padding = padding;
            this->bottom_padding = padding;
        }

        void set_column_top_padding(int padding) {
            if (padding <= 0)
                this->top_padding = 0;
            else
                this->top_padding = static_cast<unsigned int>(padding);
        }
        
        void set_column_bottom_padding(int padding) {
            if (padding <= 0)
                this->bottom_padding = 0;
            else
                this->bottom_padding = static_cast<unsigned int>(padding);
        }

        unsigned int get_top_padding() { return top_padding; }
        
        unsigned int get_bottom_padding() { return bottom_padding; }

        void set_splitted_content(StringVector splittedContent) { this->splitted_content = splittedContent; }

        StringVector get_splitted_content() { return splitted_content; }

        void set_words(StringList words) { this->words = words; }

        StringList get_words() { return words; }
    };
} // namespace tabular

#endif // TABULAR_COLUMN_HPP