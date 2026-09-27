/**
 * @file main.cpp
 * @brief Implementation of the program entry point and its supporting
 *        argument-handling/state-loading functions (see main.hpp for
 *        function-level documentation).
 */
#include "main.hpp"


std::string state;
std::string year;
/** @brief Optional user-supplied run name (from the name= argument), used to name the output directory and district file; empty if not supplied. */
std::string name = "";
unsigned int dists = 0;
std::string logName = "info";
std::string reportName = "report";
std::vector<DistrictAlgorithm*> algos;
/** @brief The output directory for this run's logs, report, and district assignment file, created by validateArgs(). */
std::string outDir;
/** @brief Numeric suffix appended to the output directory/file names to keep each run's output unique. */
unsigned int nameIndex;
/** @brief The district-drawing algorithm selected by the user via pickAlgorithm(). */
DistrictAlgorithm* D;
/** @brief The number of maps to be generated as part of this run. A value other than one sets up different reporting structures. */
unsigned int mapCount = 0;

int main(int argc, char *argv[]) {
    
    loadAlgorithms();

    if(!handleArgs(argc, argv) || !validateArgs()) {
        return 1;
    }

    pickAlgorithm();

    std::string fpath = getStatePath(state, year);
    std::cout << "Retrieving data at " << fpath << "..." << std::endl;
    State s = processGeoJson(state, fpath);
    if(mapCount == 0) {

        std::cout << "Drawing districts..." << std::endl;
        D->drawMap(s);

        std::cout << "District drawing complete..." << std::endl;
        outputDistricts(s);
        s.fullReport();
    } else {
        double polsbyPopper = 0, reock = 0, disproportionality = 0, populationDeviation = 0;
        int numIncomplete = 0;
        auto elapsed = std::chrono::milliseconds::zero();
        for(int i = 0; i < mapCount; i++) {
            auto start = std::chrono::steady_clock::now();
            D->drawMap(s);
            auto now = std::chrono::steady_clock::now();
            elapsed += std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
            
            numIncomplete += !s.isComplete();
            polsbyPopper += s.compactnessPolsbyPopper();
            reock += s.compactnessReock();
            disproportionality += s.getSeatDisproportionality();
            populationDeviation += s.getPopulationDeviation();

            s.clearMap();
        }
        
        double averageTime = elapsed.count() / (mapCount*1000);
        double avgPolsbyPopper = polsbyPopper/mapCount;
        double avgReock = reock/mapCount;
        double avgDisproportionality = disproportionality/mapCount;
        double avgDeviation = populationDeviation/mapCount;
        logs::report << std::setprecision(8) << mapCount << " maps for " << s.name << " drawn with " << dists << " districts each using algorithm " << D->name << ":" <<std::endl;
        logs::report << "Average time per map: " << averageTime << " seconds." << std::setprecision(4) << std::endl;
        logs::report << numIncomplete << "(" << (100.0*numIncomplete/mapCount) << "\%) of the maps were incomplete." << std::endl;
        logs::report << "The maps had an average population deviation of " << avgDeviation*100 << "\%." << std::endl;
        logs::report << "The maps had an average disproportionality of " << avgDisproportionality << "\% in favor of" << (avgDisproportionality > 0 ? " Democrats." : " Republicans.") << std::endl;
    }
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
            transform(logName.begin(), logName.end(), logName.begin(), ::tolower);
        }
        else if(currArg.compare(0, 7, "report=") == 0) {
            reportName = currArg.substr(7);
            transform(reportName.begin(), reportName.end(), reportName.begin(), ::tolower);
        }
        else if(currArg.compare(0, 5, "name=") == 0) {
            name = currArg.substr(5);
        }
        else if(currArg.compare(0, 10, "map-count=") == 0) {
            mapCount = std::stoi(currArg.substr(10));
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
    if(dists == 0) {
        std::cout << "District count has not been specified, using the default value for this state." << std::endl;
    }

    nameIndex = -1;
    do {
        nameIndex++;
        outDir = DATAPATH_OUT+state+"_"+name+std::to_string(nameIndex)+"/";
    } while(!std::filesystem::create_directory(outDir));

    logs::initialize(logName, outDir, reportName);
    return true;
}

void pickAlgorithm() {
    unsigned int i = 1, selection;
    for(auto a : algos) {
            std::cout << "\t" <<  i << ": " << a->name << " (" << a->desc << ")" << std::endl;
            i++;
        }
        
    do {
        std::cout << "Select an algorithm to use." << std::endl;
        std::cin >> selection;
    } while (selection < 1 || selection > algos.size());
    D = algos[selection-1];
}

std::string getStatePath(std::string state, std::string year) {
    return "data/"+year+"/"+state+".geojson";
}


State processGeoJson(std::string stateAbbr, std::string filename) {
    std::ifstream file(filename);
    std::filesystem::create_directory(DATAPATH_OUT);
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
    std::ofstream out(outDir+(name.compare("") ? name : s.id) + "_" + std::to_string(nameIndex) + DISTRICT_OUTPUT_EXTENSION);
    out << "GEOID20,District" << std::endl;
    for(auto d : s.getDistricts()) {
        for(auto p : d->getPrecincts()) {
            out << p->id << "," << d->id << std::endl;
        }
    }
    
}