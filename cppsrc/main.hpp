#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include "classdefs.hpp"
#include "json.hpp"

#include "algo/algorithm_definition.hpp"

#define DATAPATH_OUT "output/"
#define DEFAULT_CSV_PATH "data/2020/default_dist_counts.csv"

int main(int argc, char *argv[]);
std::string getStatePath(std::string state, std::string year);
State processGeoJson(std::string stateName, std::string filename);
void outputDistricts(State s);