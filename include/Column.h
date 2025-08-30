#pragma once

#include <string>
#include "Config.h"

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

      std::string toString();

    private:
      Config config;
      std::string content;
  };
}
