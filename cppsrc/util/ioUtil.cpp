#include "ioUtil.hpp"

namespace logs {
    std::ofstream info;

    void initialize(std::string fileName, std::string dir) {
        info.open(dir+"/"+fileName+".log");
    }
}