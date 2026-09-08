#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

#include "classdefs.hpp"
#include "util/json.hpp"

#include "algo/algorithm_definition.hpp"

#define DATAPATH_OUT "output/"
#define DEFAULT_CSV_PATH "data/2020/default_dist_counts.csv"
#define DISTRICT_OUTPUT_EXTENSION ".csv"


extern std::string state;
extern std::string year;
extern int dists;
extern std::string logName;
extern std::string reportName;

int main(int argc, char *argv[]);
bool handleArgs(int argc, char *argv[]);
bool validateArgs();
void pickAlgorithm();
std::string getStatePath(std::string state, std::string year);
State processGeoJson(std::string stateName, std::string filename);
void outputDistricts(State s);