/**
 * @file ioUtil.hpp
 * @brief Logging utilities: named output streams for debug info and
 *        generated reports, configurable to write to the console or to
 *        files on disk.
 */
#pragma once

#include <fstream>
#include <filesystem>
#include <iostream>

/** @brief Default output directory used for log/report files. */
#define DATAPATH_OUT "output/"

/**
 * @namespace logs
 * @brief Holds the program's two logging streams (debug info and
 *        generated reports) and the logic to direct each to the console
 *        or to a file.
 */
namespace logs {
    /** @brief Stream for debug/progress logging; directed to std::cout or a file by initialize(). */
    extern std::ostream info;
    /** @brief Stream for generated report output; directed to std::cout or a file by initialize(). */
    extern std::ostream report;

    /**
     * @brief Configure the info and report streams to write to the console or to files under a given directory.
     * @param infoFName The debug log's file name (without extension), or "console" to log to stdout.
     * @param dir The directory to write log files into, used when infoFName/reportFName are not "console".
     * @param reportFName The report log's file name (without extension), or "console" to log to stdout.
     */
    void initialize(std::string infoFName, std::string dir, std::string reportFName);
}