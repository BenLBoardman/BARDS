#pragma once

#include <fstream>
#include <filesystem>
#include <iostream>

#define DATAPATH_OUT "output/"

namespace logs {
    extern std::ostream info;
    extern std::ostream report;

    void initialize(std::string infoFName, std::string dir, std::string reportFName);
}