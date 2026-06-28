#include "main.hpp"


std::string state = "NH";


int main(int argc, char *argv[]) {
    std::string fpath = getStatePath(state);
    std::cout << "Retrieving data at " << fpath << "..." << std::endl;
    State s = processGeoJson(state, fpath);

    std::cout << "Drawing districts..." << std::endl;
    NeighborDistrictTest m;
    m.drawMap(s);

    std::cout << "District drawing complete..." << std::endl;
    outputDistricts(s);
}

std::string getStatePath(std::string state) {
    return "data/"+state+".geojson";
}


State processGeoJson(std::string stateAbbr, std::string filename) {
    std::ifstream file(filename);
    if(!file.is_open()) {
        throw std::runtime_error("Error: Could not open file. Check to ensure that the state abbreviation is correct.");
    }

    std::string line, stateName, temp;
    std::ifstream defaultCount(DEFAULT_CSV_PATH);
    do {
        std::getline(defaultCount, line);
    }while(line.find(stateAbbr) != 0 || defaultCount.eof());
    std::stringstream linestream(line);
    std::getline(linestream, temp, ',');
    std::getline(linestream, stateName, ',');
    stateName = stateName.substr(1);
    std::getline(linestream, temp, ',');
    int districtCount = std::stoi(temp);

    std::cout << "Drawing " << districtCount << " districts for " << stateName << "..." << std::endl;

    State state(stateAbbr, stateName, districtCount);
    std::string fileContents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    JsonValue json = parseJson(fileContents);

    state.loadDatasets(json["datasets"]);

    std::cout << "Loading precinct data..." << std::endl;
    for(const JsonValue& feature : json["features"].asArray()) {
        Precinct *p = new Precinct(state, feature);
        state.addPrecinct(*p);
    }
    
    state.finishProcessing();
    return state;
}

void outputDistricts(State s) {
    std::ofstream out(DATAPATH_OUT+s.abbr + ".csv");
    out << "GEOID20,District" << std::endl;
    for(auto d : s.getDistricts()) {
        for(auto p : d->getPrecincts()) {
            out << p->id << "," << d->id << std::endl;
        }
    }
}