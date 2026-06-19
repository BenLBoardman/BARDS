#include <vector>
#include <string>

//#include "classdefs.hpp"
#include "geometry.cpp"

#define STATE_DATA_PATH "https://raw.githubusercontent.com/BenLBoardman/BARDS/vtd-data"
#define DATAPATH_OUT "output/"

std::string datapath(STATE_DATA_PATH);

int main(int argc, char *argv[]);
std::string  getDataset(std::string state, int year);
std::string getStatePath(std::string state, std::string year);