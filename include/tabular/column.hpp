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

#include <tabular/enums.hpp>
#include <tabular/glVars.hpp>

namespace tabular {
    class Column {
        std::vector<FontStyle> fontStyles;
        StringVector splittedContent;
        StringList words;
        Alignment alignment;
        unsigned int width;
        unsigned int topPadding;
        unsigned int bottomPadding;

    public:
        std::string content;

        Column(std::string content)
            : content(content), alignment(Alignment::left), width(0), topPadding(0), bottomPadding(0) {};

        void setColumnAlign(Alignment alignment) { this->alignment = alignment; }

        Alignment getColumnAlign() { return alignment; }

        void setWidth(int width) {
            if (width <= 0)
                width = 0;

            this->width = static_cast<unsigned int>(width);
        }

        unsigned int getWidth() { return this->width; }

        void setColumnPadding(int padding) {
            if (padding <= 0)
                padding = 0;

            this->topPadding = padding;
            this->bottomPadding = padding;
        }

        void setColumnTopPadding(int padding) {
            if (padding <= 0)
                this->topPadding = 0;
            else
                this->topPadding = static_cast<unsigned int>(padding);
        }
        
        void setColumnBottomPadding(int padding) {
            if (padding <= 0)
                this->bottomPadding = 0;
            else
                this->bottomPadding = static_cast<unsigned int>(padding);
        }

        unsigned int getTopPadding() { return topPadding; }
        
        unsigned int getBottomPadding() { return bottomPadding; }

        void setSplittedContent(StringVector splittedContent) { this->splittedContent = splittedContent; }

        StringVector getSplittedContent() { return splittedContent; }

        void setWords(StringList words) { this->words = words; }

        StringList getWords() { return words; }
    };
} // namespace tabular

#endif // TABULAR_COLUMN_HPP