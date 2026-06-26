#include <vector>
#include <string>

#include "classdefs.hpp"

#define STATE_DATA_PATH "https://raw.githubusercontent.com/BenLBoardman/BARDS/vtd-data"
#define DATAPATH_OUT "output/"

extern std::string datapath;

int main(int argc, char *argv[]);
std::string getStatePath(std::string state, std::string year);
State processGeoJson(std::string stateName, std::string filename, int districtCount);