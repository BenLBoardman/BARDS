#include "classdefs.hpp"

#include <iostream>
Precinct::Precinct(State& state, const JsonValue& json) : state(state), geo(*this) {
    district = nullptr;
    const JsonValue& properties = json["properties"];

    id = properties["id"].asString();
    name = properties["name"].asString();

    //demographic data
    std::set<std::string> datasets = state.getDatasetNames();
    for(std::string dataName : datasets) {
        if(state.getDataSet(dataName).isDemographic()) {
            demo.emplace(static_cast<const DemographicData&>(state.getDataSet(dataName)), properties["datasets"][dataName].asObject());
        }
        else {
            elex.emplace(static_cast<const ElectionData&>(state.getDataSet(dataName)), properties["datasets"][dataName].asObject());
        }
    }

    //geometry
    geo.loadGeometry(json["geometry"]);
}

void Precinct::computeNeighbors() {
    //TODO - use Geometry object & GeoLine to find neighbors of a precinct. Will likely involve functions to find neighbors for a Geometry.
}

int Precinct::getPopulation() {
    return population;
}

std::set<DemographicData> Precinct::getDemo() {
    return demo;
}

std::set<ElectionData> Precinct::getElex() {
    return elex;
}


District::District(State& state, int target) : state(state), target(target), geo(*this) { population = 0; }


State::State(std::string name, int districtCount) : name(name), districtCount(districtCount) {
    districts = std::vector<District*>();
    population = 0;
}

void State::addPrecinct(Precinct& p) {
    precincts.emplace_back(&p);
    population += p.getPopulation();
    
    //TODO - merge precinct demographics & election data into state
}


void State::finishProcessing() {
    int target = population / districtCount;
    int rem = population % districtCount;
    for(int i = 0; i < districtCount; i++) {
        districts.push_back(new District(*this, target + (rem != 0)));
        rem -= (rem != 0);
    }
}

void State::loadDatasets(const JsonValue& json) {
    for(const auto& [key, set] : json.asObject()) {
        datasetNames.insert(key);
        if (isDemographic(set)) {
            demo.emplace(key, DemographicData(key, set));
        } else {
            elex.emplace(key, ElectionData(key, set));
        }
        std::cout << "Found data set " << key << std::endl;
    }
}

const DataSet& State::getDataSet(const std::string& name) const {
    auto dSet = demo.find(name);
    if(dSet != demo.end()) {
        return dSet->second;
    }
    auto eSet = elex.find(name);
    if(eSet != elex.end()) {
        return eSet->second;
    }
    throw std::runtime_error("Selected data set "+name+" does not exist!");
}