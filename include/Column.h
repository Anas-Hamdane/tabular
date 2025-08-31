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

      std::vector<String> toString() const;

    private:
      Config config;
      std::string content;

      std::vector<uint8_t> getAttrsCodes() const;
      std::vector<uint8_t> getFGCode() const;
      std::vector<uint8_t> getBGCode() const;
      std::string getBaseFormat() const;
  };
}
