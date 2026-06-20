
#include "main.hpp"

std::string state = "NH";
std::string year = "2020";
int districtCount = 2;

int main(int argc, char *argv[]) {
    std::string fpath = getStatePath(state, year);
    processGeoJson(state, fpath, districtCount);
}

std::string getStatePath(std::string state, std::string year) {
    return datapath+"/"+year+"/"+state+".geojson";
}

State processGeoJson(std::string stateName, std::string filename, int districtCount) {
    State state(stateName, districtCount);

    //TODO - use file name to open the file as an ifstream
    //TODO - loop while reading a precinct into a std::string and use precinct constructor to parse new precincts from these strings
    //TODO - add those precincts to the State using State::addPrecinct()

}