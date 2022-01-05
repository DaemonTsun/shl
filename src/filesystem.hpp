
#pragma once

#include <fstream>
#include <vector>
#include <filesystem>

namespace fs
{
using namespace std::filesystem;

inline void read_binary_file(const fs::path &pth, std::vector<char> &out)
{
    if (!fs::exists(pth) || !fs::is_regular_file(pth))
        throw std::runtime_error(std::string("file '") + pth.c_str() + "' does not exist");

    std::ifstream file(pth, std::ios::ate | std::ios::binary);

    size_t sz = file.tellg();
    out.resize(sz);
    
    file.seekg(0);
    file.read(out.data(), sz);
    file.close();
}

inline std::vector<char> read_binary_file(const fs::path &pth)
{
    std::vector<char> ret;
    read_binary_file(pth, ret);
    return ret;
}
}

