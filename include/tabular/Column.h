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

      // @form indicates the word itself
      // @escs indecate the escape sequences found inside the word
      struct Word {
        String form;
        std::vector<std::string> escs;
      };

      std::vector<uint8_t> getAttrsCodes() const;
      std::vector<uint8_t> getFGCode() const;
      std::vector<uint8_t> getBGCode() const;
      std::string getBaseFormat() const;
      void processWord(std::vector<String>& lines, String& line, String& word) const;
      String processLine(std::vector<std::string>& words, size_t& pos, String& next) const;
  };
}
