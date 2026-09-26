#include "ioUtil.hpp"

namespace logs {
    std::ostream info(nullptr);
    std::ostream report(nullptr);

    std::ofstream ifs;
    std::ofstream rfs;
    std::streambuf *rBuf;
    std::streambuf *iBuf;

    void initialize(std::string infoFName, std::string dir, std::string reportFName) {
        if(infoFName.compare("console") == 0) {
            iBuf = std::cout.rdbuf();
            std::cout << "Logging debug output to the console..." << std::endl;
        } else {
            std::string debugFilePath = dir+infoFName+".log";
            ifs.open(debugFilePath);
            iBuf = ifs.rdbuf();
            std::cout << "Logging debug output to " << debugFilePath << "..." << std::endl;
        }
        if(reportFName.compare("console") == 0) {
            rBuf = std::cout.rdbuf();
            std::cout << "Logging reports to the console..." << std::endl;
        } else {
            std::string reportFilePath = dir+reportFName+".log";
            rfs.open(reportFilePath);
            rBuf = rfs.rdbuf();
            std::cout << "Logging reports to " << reportFilePath << "..." << std::endl;
        }

        info.rdbuf(iBuf);
        report.rdbuf(rBuf);
    }
}