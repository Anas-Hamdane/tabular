#pragma once

#include <string>
namespace tabular {
  class String {
    public:
      String();
      String(std::string content);
      String(std::string content, size_t visibleWidth);
      String operator=(std::string content);

      std::string getContent() const;
      size_t getVisibleWidth() const;
      size_t getVisibleWidth(); // caches the width if it should be calculated

      void setContent(std::string content);
      static size_t calculateVisibleWidth(const std::string& str);
      static bool utf8twc(const char* s, wchar_t& wc, int& consumed);
      static size_t wcwidth(wchar_t ucs);

    private:
      std::string content;
      size_t visibleWidth;
      bool recalculate;
  };
}
