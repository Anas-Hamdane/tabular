#pragma once

#include <string>
#include "Config.h"
#include "String.h"

namespace tabular {
  class Column {
    public:
      Column(std::string content);
      Column();

      std::string getContent() const;

      Config& getConfig();
      const Config& getConfig() const;

      void setContent(std::string name);
      void setConfig(Config config);

      String toString() const;

    private:
      Config config;
      std::string content;

      void applyAttributes();
      void applyColors();
      void applyBackColors();
      void applyBase();
      void applyPadding();
  };
}
