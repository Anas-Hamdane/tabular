#include "../include/Column.h"
#include <stdexcept>

namespace tabular {
  Column::Column(std::string content)
    : content(std::move(content)) {}
  Column::Column() = default;

  std::string Column::getContent() const { return this->content; }

  Config& Column::getConfig() { return this->config; }
  const Config& Column::getConfig() const { return this->config; }

  void Column::setContent(std::string content) { this->content = std::move(content); }
  void Column::setConfig(Config config) { this->config = std::move(config); }

  String Column::toString() const
  {
    if (this->config.getWidth() == -1)
      throw std::logic_error("Internal error: uninitialized column width");

    // TODO: complete this
  }
}
