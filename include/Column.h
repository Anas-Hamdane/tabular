#pragma once

#include <string>
#include "Config.h"
#include "String.h"

namespace tabular {
  class Column {
    public:
      Column(std::string content);
      Column();

      std::string getContent();

      Config& getConfig();
      const Config& getConfig() const;

      void setContent(std::string name);
      void setConfig(Config config);

      String toString();

    private:
      Config config;
      std::string content;
  };
}
