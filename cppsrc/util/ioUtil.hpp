#pragma once

#include <fstream>
#include <filesystem>

#define DATAPATH_OUT "output/"

namespace logs {
    extern std::ofstream info;

    void initialize(std::string fileName, std::string dir);
}