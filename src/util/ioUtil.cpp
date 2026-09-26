/**
 * @file ioUtil.cpp
 * @brief Implementation of the logs namespace (see ioUtil.hpp for
 *        namespace-, stream-, and initialize()-level documentation).
 */
#include "ioUtil.hpp"

namespace logs {
    std::ostream info(nullptr);
    std::ostream report(nullptr);

    /** @brief File stream backing the info log when initialize() is given a file name rather than "console". */
    std::ofstream ifs;
    /** @brief File stream backing the report log when initialize() is given a file name rather than "console". */
    std::ofstream rfs;
    /** @brief Stream buffer currently backing the report stream (either std::cout's buffer or rfs's). */
    std::streambuf *rBuf;
    /** @brief Stream buffer currently backing the info stream (either std::cout's buffer or ifs's). */
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