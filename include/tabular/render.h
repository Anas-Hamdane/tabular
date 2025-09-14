#pragma once

#include <string>

#if defined(_WIN32) || defined(_WIN64)
#include "windows.h"
#include <io.h>
#endif

namespace tabular {
  inline void render(const std::string& str, FILE* out)
  {
    if (str.empty())
      return;

#if defined(_WIN32) || defined(_WIN64)
    HANDLE handle;

    if (out == stdout)
      handle = GetStdHandle(STD_OUTPUT_HANDLE);
    else if (out == stderr)
      handle = GetStdHandle(STD_ERROR_HANDLE);
    else
      handle = reinterpret_cast<HANDLE>(_get_osfhandle(_fileno(out)));

    if (handle == INVALID_HANDLE_VALUE)
      return;

    DWORD mode;
    DWORD written;

    if (!GetConsoleMode(handle, &mode))
    {
      WriteFile(handle, str.c_str(), str.length(), &written, nullptr);
      return;
    }

    // convert UTF-8 to UTF-16 because it's Windows
    int wlen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);

    // fallback
    if (wlen <= 0)
    {
      WriteFile(handle, str.c_str(), str.length(), &written, nullptr);
      return;
    }

    wchar_t* buffer = new wchar_t[wlen];
    int result = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, buffer, wlen);

    if (result <= 0)
    {
      delete[] buffer;
      WriteFile(handle, str.c_str(), str.length(), &written, nullptr);
      return;
    }

    WriteConsoleW(handle, buffer, static_cast<DWORD>(wlen - 1), &written, nullptr);
    delete[] buffer;
#else
    fwrite(str.c_str(), 1, str.length(), out);
#endif
  }
}