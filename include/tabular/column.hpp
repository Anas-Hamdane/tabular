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

#include "enums.hpp"
#include "glVars.hpp"

#ifndef TABULAR_COLUMN_HPP
#define TABULAR_COLUMN_HPP

namespace tabular {
    class Column {
        std::vector<FontStyle> fontStyles;
        StringVector splittedContent;
        StringList words;
        Alignment alignment;
        unsigned int width;

    public:
        std::string content;

        Column(std::string content)
            : content(content), alignment(Alignment::left), width(0) {};

        void setColumnAlign(Alignment alignment) { this->alignment = alignment; }

        Alignment getColumnAlign() { return alignment; }

        void setWidth(unsigned int width) { this->width = width; }

        unsigned int getWidth() { return this->width; }

        void setSplittedContent(StringVector splittedContent) { this->splittedContent = splittedContent; }

        StringVector getSplittedContent() { return splittedContent; }

        void setWords(StringList words) { this->words = words; }

        StringList getWords() { return words; }
    };
} // namespace tabular

#endif // TABULAR_COLUMN_HPP