# tabular
A lightweight, header-only C++ library for creating well-formatted, fully-customizable CLI tables.

## Features
- full Unicode support
- locale-independent display width handling
- cross-platform rendering support through `render()` function
- compatible with [fmtlib](https://github.com/fmtlib/fmt)'s colored strings
- fully customizable through `Alignment`, `Padding`, `Color`, `Rgb`...
- flexible tables with variant number of columns are supported
- full c++11 support, uses only `vector`, `string`, `stdexcept` and `cstdint` standard headers
- fully customizable border with pre-defined ready-to use templates
- header-only lightweight library with a [single-include file](./single_include/tabular/tabular.h)

## Quick Start
Include the [header files](./include/tabular) or the [single-header file](./single_include/tabular/tabular.h)
and start the journey!

```c++
#include "tabular/table.h"
#include "tabular/render.h"

int main()
{
  using namespace tabular;
  Table table;

  table.addRow({"Countries Capitals"})
      .addRow({"United States", "Washington"})
      .addRow({"Brazil", "Brasilia"})
      .addRow({"France", "Paris"})
      .addRow({"Japan", "Tokyo"})
      .addRow({"India", "New Delhi"});

  render(table.str() + '\n', stdout);
  return 0;
}
```

## Documentation
For complete documentation with examples:
- [Getting Started Guide](docs/doc.md) - Complete usage guide
- [Examples Directory](./examples) - Some examples