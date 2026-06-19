
#include "main.hpp"


std::string state = "NH";
std::string year = "2020";

int main(int argc, char *argv[]) {
    std::string fpath = getStatePath(state, year);
}

std::string getStatePath(std::string state, std::string year) {
    return datapath+"/"+year+"/"+state+".geojson";
}