#include "main.hpp"


std::string state = "NH";
std::string year = "2020";
int districtCount = 2;


int main(int argc, char *argv[]) {
    std::string fpath = getStatePath(state, year);
    std::cout << "Retrieving data at " << fpath << "..." << std::endl;
    processGeoJson(state, fpath, districtCount);
}

std::string getStatePath(std::string state, std::string year) {
    return "data/"+year+"/"+state+".geojson";
}


State processGeoJson(std::string stateName, std::string filename, int districtCount) {
    State state(stateName, districtCount);

    std::ifstream file(filename);
    if(!file.is_open()) {
        throw std::runtime_error("Error: Could not open file. Check to ensure that the state abbreviation and year are correct.");
    }
    
    std::string fileContents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    JsonValue json = parseJson(fileContents);

    state.loadDatasets(json["datasets"]);

    for(const JsonValue& feature : json["features"].asArray()) {
        Precinct *p = new Precinct(state, feature);
        state.addPrecinct(*p);
    }
    
    state.finishProcessing();
    return state;
}