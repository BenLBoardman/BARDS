/**
 * @file main.hpp
 * @brief Entry point and command-line/state-loading support for the
 *        redistricting tool: argument parsing/validation, algorithm
 *        selection, GeoJSON ingestion, and district output.
 */
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <chrono>

#include "classdefs.hpp"
#include "util/json.hpp"
#include "util/ioUtil.hpp"

#include "algo/algorithm_definition.hpp"

/** @brief Path to the CSV of default district counts per state, used when no districts= argument is given. */
#define DEFAULT_CSV_PATH "data/2020/default_dist_counts.csv"
/** @brief File extension appended to the generated district-assignment output file. */
#define DISTRICT_OUTPUT_EXTENSION ".csv"


/** @brief The two-letter state abbreviation (uppercased) supplied on the command line. */
extern std::string state;
/** @brief The census/data year supplied on the command line, used to locate the input GeoJSON directory. */
extern std::string year;
/** @brief The requested number of districts to draw; 0 (the default) selects the state's default district count. */
extern unsigned int dists;
/** @brief The debug log's output file name (without extension), or "console" to log to stdout. */
extern std::string logName;
/** @brief The generated report's output file name (without extension), or "console" to log to stdout. */
extern std::string reportName;

/**
 * @brief Program entry point: loads algorithms, parses and validates arguments, loads state data, runs the selected districting algorithm, and writes the resulting district assignments and report.
 * @param argc Argument count.
 * @param argv Argument values: `<YEAR> <STATE> {opt. args}`.
 * @return 0 on success; 1 if argument parsing/validation fails.
 */
int main(int argc, char *argv[]);
/**
 * @brief Parse command-line arguments into the year, state, and optional parameters (districts=, log=, report=, name=).
 * @param argc Argument count.
 * @param argv Argument values.
 * @return True if arguments were successfully parsed; false (with an error message printed) if too few arguments were given or an argument was unrecognized.
 */
bool handleArgs(int argc, char *argv[]);
/**
 * @brief Validate the parsed year and state against the data directory, create a unique output directory, and initialize logging.
 * @return True if the year/state are valid and setup succeeded; false (with an error message printed) otherwise.
 */
bool validateArgs();
/**
 * @brief Prompt the user (via stdin) to select a district-drawing algorithm from the global algos list, storing the choice in the global D.
 */
void pickAlgorithm();
/**
 * @brief Build the file path to a state's input GeoJSON file for a given year.
 * @param state The two-letter state abbreviation.
 * @param year The data year.
 * @return The path "data/{year}/{state}.geojson".
 */
std::string getStatePath(std::string state, std::string year);
/**
 * @brief Load a state's precinct, dataset, and geometry data from its GeoJSON file, building a fully-populated State ready for districting.
 * @param stateName The two-letter state abbreviation, used to look up its default district count and display name.
 * @param filename Path to the state's GeoJSON input file.
 * @return The constructed State, with all precincts loaded and neighbor adjacency computed.
 * @throws std::runtime_error if the GeoJSON file cannot be opened.
 */
State processGeoJson(std::string stateName, std::string filename);
/**
 * @brief Write the final district assignment for every precinct in the state to a CSV file in the output directory.
 * @param s The districted state to export.
 */
void outputDistricts(State s);

/**
 * @brief Create reports on a list of states produced by a batch run of BARDS.
 * @param maps A std::vector of the maps to include in the output.
 */
void multiMapReport(std::vector<State> maps);