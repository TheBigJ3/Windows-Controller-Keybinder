#pragma once
#include <windows.h>
#include <filesystem>

inline std::filesystem::path GetExeDir() {
    wchar_t buf[MAX_PATH];
    DWORD n = GetModuleFileNameW(nullptr, buf, MAX_PATH);
    return std::filesystem::path(buf, buf + n).parent_path();
}


inline std::filesystem::path ResolveConfigPath() {
    auto exeDir = GetExeDir();
    auto p1 = exeDir / "config.json";
    if (std::filesystem::exists(p1)) return p1;

    auto p2 = exeDir.parent_path() / "config.json";
    if (std::filesystem::exists(p2)) return p2;

    auto p3 = std::filesystem::current_path() / "config.json";
    if (std::filesystem::exists(p3)) return p3;

    return p1;
}
