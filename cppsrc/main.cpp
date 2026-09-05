#include "main.hpp"


std::string state = "NH";
std::string year = "2020";
int dists = -1;
std::string logName = "info.log";
std::string reportName = "report.log";


int main(int argc, char *argv[]) {


    if(!handleArgs(argc, argv) || !validateArgs()) {
        return 1;
    }

    std::string fpath = getStatePath(state, year);
    std::cout << "Retrieving data at " << fpath << "..." << std::endl;
    State s = processGeoJson(state, fpath);

    std::cout << "Drawing districts..." << std::endl;
    MonoDistrictTest m;
    m.drawMap(s);

    std::cout << "District drawing complete..." << std::endl;
    outputDistricts(s);
}

bool handleArgs(int argc, char *argv[]) {
    if(argc < 3) {
        std::cout << "Usage: ./bin/bards.exe <YEAR> <STATE> {opt. args}" << std::endl;
        return false;
    }
    year = argv[1];
    state = argv[2];
    transform(state.begin(), state.end(), state.begin(), ::toupper);

    std::string currArg;
    for(int i = 3; i < argc; i++) {
        currArg = argv[i];
        if(currArg.compare(0, 10, "districts=") == 0) {
            dists = std::stoi(currArg.substr(10));
        }
        else if(currArg.compare(0, 4, "log=") == 0) {
            logName = currArg.substr(4);
        }
        else if(currArg.compare(0, 7, "report=") == 0) {
            reportName = currArg.substr(7);
        }
        else {
            std::cout << "Error: Unrecognized argument " << argv[i] << "." << std::endl;
            return false; 
        }
    }
    return true;
}

bool validateArgs() {
    std::filesystem::path path = "data/"+year;
    if(!std::filesystem::exists(path) || !std::filesystem::is_directory(path)) {
        std::cout << "Invalid year parameter " << year << " entered. Aborting." << std::endl;
        return false;
    }
    path = path / std::string(state+".geojson");
    if(!std::filesystem::exists(path) || !std::filesystem::is_regular_file(path)) {
        std::cout << "Invalid state parameter " << state << " entered. Aborting." << std::endl;
        return false;
    }
    if(dists < 1) {
        std::cout << "Maps must be drawn with at least one district, entered " << dists <<". Value will be set to the default for this state." << std::endl;
    }
    return true;
}

std::string getStatePath(std::string state, std::string year) {
    return "data/"+year+"/"+state+".geojson";
}


State processGeoJson(std::string stateAbbr, std::string filename) {
    std::ifstream file(filename);
    if(!file.is_open()) {
        throw std::runtime_error("Error: Could not open file. Check to ensure that the state abbreviation and year are valid.");
    }

    std::string line, stateName, temp;
    std::ifstream defaultCount(DEFAULT_CSV_PATH);
    do {
        std::getline(defaultCount, line);
    }while(line.find(stateAbbr, 0) != 0);
    std::stringstream linestream(line);
    std::getline(linestream, temp, ',');
    std::getline(linestream, stateName, ',');
    stateName = stateName.substr(1);
    std::getline(linestream, temp, ',');
    if(dists < 1)
        dists = std::stoi(temp);

    std::cout << "Drawing " << dists << " districts for " << stateName << "..." << std::endl;

    State state(stateAbbr, stateName, dists);
    std::string fileContents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    JsonValue json = parseJson(fileContents);

    state.loadDatasets(json["datasets"]);

    std::cout << "Loading precinct data..." << std::endl;
    int i = 0;
    for(const JsonValue& feature : json["features"].asArray()) {
        Precinct *p = new Precinct(state, feature);
        state.addPrecinct(*p);
        i++;
    }
    
    std::cout << "Data for " << i << " precincts loaded." << std::endl;
    state.finishProcessing();
    return state;
}

void outputDistricts(State s) {
    std::ofstream out(DATAPATH_OUT+s.id + ".csv");
    out << "GEOID20,District" << std::endl;
    for(auto d : s.getDistricts()) {
        for(auto p : d->getPrecincts()) {
            out << p->id << "," << d->id << std::endl;
        }
    }
}