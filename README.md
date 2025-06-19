<div align="center">
  <img src="img/examples.png"/>
</div>

# tabular
A lightweight, header-only C++ library for creating well-formatted, fully-customizable CLI tables.

## Why Yet Another Tabling Library?
This project was inspired primarily by [tabulate](https://github.com/p-ranav/tabulate), a modern C++ table formatting library. However, while using it, I encountered several limitations:

- **Dynamic Tables Support** - Rows with variant number of columns aren't properly handled
- **Locale Dependent Behavior** - Multi-byte character handling depends on system locale
- **Performance Overhead** - Simple examples could be up to 40x slower at runtime compared to this implementation

So I decided to write my own implementation with better performance and more flexible design.

## Features
- **Easy integration** - Header-only implementation with single include file
- **Lightweight design** - Minimal overhead and resource usage
- **Zero dependencies** - No external libraries required
- **Locale independent** - Consistent behavior with robust multi-byte character support
- **Cross-platform compatibility** - Seamless operation on Unix-like systems and Windows
- **Unicode support** - Full multi-byte character handling across different platforms
- **Rich styling** - Complete RGB color support, text attributes, and border customization
- **Dynamic tables support** - Handle tables with varying column counts per row
- **Modern C++ support** - Compatible with C++11 and later standards

## Quick Start
Include the main header file (from the `include/` or the `single_include/`) and start creating tables immediately:

```cpp
#include "tabular.hpp"

int main() {
    using namespace tabular;
    
    Table table;
    table.add_row({"Country", "Capital"})
         .add_row({"United States", "Washington"})
         .add_row({"Brazil", "Brasilia"})
         .add_row({"France", "Paris"})
         .add_row({"Japan", "Tokyo"})
         .add_row({"India", "New Delhi"});
    
    print(table);
    return 0;
}
```

## Documentation
For complete documentation with examples:
- **[Getting Started Guide](./docs/getting-started.md)** - Complete usage guide with examples
- **[Examples Directory](./examples/source/)** - Advanced usage examples

## Requirements
- C++11 or later
- No external dependencies

## Contributing
Contributions are welcome! Please see [CONTRIBUTING.md](./CONTRIBUTING.md) for guidelines.

## License
This project is distributed under the MIT License. See [LICENSE](./LICENSE) for more details.
