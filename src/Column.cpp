#include "../include/Column.h"

namespace tabular {
  Column::Column(std::string content)
    : content(std::move(content)) {}
  Column::Column() = default;

  std::string Column::getContent() { return this->content; }

  Config& Column::getConfig() { return this->config; }
  const Config& Column::getConfig() const { return this->config; }

  void Column::setContent(std::string content) { this->content = std::move(content); }
  void Column::setConfig(Config config) { this->config = std::move(config); }
}
