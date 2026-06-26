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
    

    // skip to the features array
    std::string token;
    while(file && token != "\"features\":") {
        file >> token;
    }
    // skip the [
    file >> token; // "["

    // read one feature object at a time by tracking brace depth
    std::string featureJson;
    char c;
    while(file.get(c)) {
        if(c == '{') {
            int depth = 1;
            featureJson = "{";
            while(file.get(c) && depth > 0) {
                featureJson += c;
                if(c == '{') depth++;
                else if(c == '}') depth--;
            }
            if(depth == 0) {
                Precinct* p = new Precinct(state, featureJson);
                state.addPrecinct(*p);
            }
        } else if(c == ']') {
            break; // end of features array
        }
    }
    
    state.finishProcessing();
    return state;
}