#pragma once

#include <string>

namespace tabular {
  class String {
    public:
      String();
      String(std::string content);
      String(std::string content, size_t visibleWidth);
      String operator=(std::string content);
      bool operator==(const std::string& other);
      bool operator==(const String& other);
      bool operator!=(const std::string& other);
      bool operator!=(const String& other);
      String& operator+=(const String& other);
      String& operator+=(const std::string& other);
      char operator[](int index);

      std::string getContent() const;
      size_t getVisibleWidth() const;
      size_t getVisibleWidth(); // caches the width if it should be calculated

      void setContent(std::string content);
      static size_t calculateVisibleWidth(const std::string& str);
      static bool utf8twc(const char* s, wchar_t& wc, int& consumed);
      static size_t wcwidth(wchar_t ucs);

      size_t len() const;
      bool empty() const;
      void reserve(size_t n);
      void clear();
      void insert(size_t pos, std::string s);

    private:
      std::string content;
      size_t visibleWidth;
      bool recalculate;
  };
}
