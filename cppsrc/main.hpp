#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include "classdefs.hpp"

#define DATAPATH_OUT "output/"

int main(int argc, char *argv[]);
std::string getStatePath(std::string state, std::string year);
State processGeoJson(std::string stateName, std::string filename, int districtCount);