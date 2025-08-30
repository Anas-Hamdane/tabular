#pragma once

#include <vector>
#include <string>

namespace tabular {
  /*
   *  The `Attribute` class serves three main purposes:
   *    - 1. applying textual attributes to the whole text.
   *    - 2. applying textual attributes to a partial string
   *      indicated using the start and the end index.
   *    - 3. applying textual attributes to all instance of
   *      a given partial string.
   *  
   *  differing between these three behaviors is done using
   *  the private enum `Option`
   */
  class Attribute {
    public:
      enum class Knd : int {
        bold = 1,
        dim = 2,
        italic = 3,

        underline = 4,
        double_underline = 21, // not popular

        blink = 5, // rarely supported
        fast_blink = 6, // rarely supported
        reverse = 7,
        concealed = 8,
        crossed = 9,
      };
      Attribute();
      Attribute(Knd attr); // for convenience
      Attribute(std::vector<Knd> attrs);

      Attribute(Knd attr, std::pair<size_t, size_t> range);
      Attribute(std::vector<Knd> attrs, std::pair<size_t, size_t> range);

      Attribute(Knd attr, std::string target);
      Attribute(std::vector<Knd> attrs, std::string target);

      ~Attribute();

      void setAttrs(std::vector<Knd> attrs);
      void setRange(std::pair<size_t, size_t> range);
      void setTarget(std::string target);

      std::vector<Knd> getAttrs();
      std::pair<size_t, size_t> getRange();
      std::string getTarget();

    private:
      // the indicator
      enum class Option { Global, Range, String } opt;

      std::vector<Knd> attrs; // the attributes
      union Data {
        // Global doesn't need a union member
        std::pair<size_t, size_t> range; // identified by `Range`
        std::string target; // identified by `String`

        // to shut the compilers mouth
        Data() {}
        ~Data() {}
      } data;

      void destroyUnion();
  };
}
