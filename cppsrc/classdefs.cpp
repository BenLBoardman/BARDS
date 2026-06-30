#include "classdefs.hpp"

#include <iostream>
Precinct::Precinct(State& state, const JsonValue& json) : state(state), id(json["properties"]["id"].asString()), name(json["properties"]["name"].asString()), geo(*this) {
    district = nullptr;
    const JsonValue& properties = json["properties"];


    //demographic data
    std::set<std::string> datasets = state.getDatasetNames();
    for(std::string dataName : datasets) {
        if(state.getDataSet(dataName).isDemographic()) {
            demo.emplace(static_cast<DemographicData&>(state.getDataSet(dataName)), properties["datasets"][dataName].asObject());
        }
        else {
            elex.emplace(static_cast<ElectionData&>(state.getDataSet(dataName)), properties["datasets"][dataName].asObject());
        }
    }

    auto d = demo.find(*state.getCanonicalDemo());
    if (d != demo.end()) {
        canonicalDemo = &(*d);
        population = canonicalDemo->getTotal();
    }
    auto e = elex.find(*state.getCanonicalElex());
    if (e != elex.end()) {
        canonicalElex = &(*e);
    }

    //geometry
    geo.loadGeometry(json["geometry"]);
}

void Precinct::computeNeighbors() {
    const std::set<GeoLine*> boundaries = geo.getLines();
    for(auto segment : boundaries) {
        for(auto owner : segment->getOwners()) {
            Geometry<Precinct>* neighbor = dynamic_cast<Geometry<Precinct>*>(owner);
            if(owner == &geo || neighbor == nullptr) continue;
            neighbors.insert(&neighbor->getOwner());
        }
    }
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


District::District(State& state, int id, int target) : state(state), id(id), target(target), geo(*this) { population = 0; }

bool District::addPrecinct(Precinct* p) {
    if(p->isAssigned()) {
        std::cout << "Attempt to add precinct to district when it is already assigned to a district" << std::endl;
        return false;
    }
    p->setDistrict(this);
    precincts.push_back(p);
    population += p->getPopulation();
    geo.mergeGeometry(p->getGeo());
    return true;
}

bool District::removePrecinct(Precinct* p) {
    auto it = std::find(precincts.begin(), precincts.end(), p);
    if(it == precincts.end()) {
        std::cout << "Attempt to remove precinct from district, but this precinct is not assigned to this district" << std::endl;
        return false;
    }
    precincts.erase(it);
    p->setDistrict(nullptr);
    population -= p->getPopulation();
    geo.mergeGeometry(p->getGeo());
    return true;
}

State::State(std::string abbr, std::string name, int districtCount) : abbr(abbr), name(name), districtCount(districtCount) {
    districts = std::vector<District*>();
    population = 0;
}

void State::addPrecinct(Precinct& p) {
    precincts.emplace_back(&p);
    population += p.getPopulation();
}

Precinct* State::getRandomPrecinct() {
    std::random_device rd;
    std::uniform_int_distribution<int> rand(0, precincts.size());
    return precincts[rand(rd)];
}

void State::finishProcessing() {
    int target = population / districtCount;
    int rem = population % districtCount;
    for(int i = 0; i < districtCount; i++) {
        districts.push_back(new District(*this, i+1, target + (rem != 0)));
        rem -= (rem != 0);
    }

    std::cout << "Precincts loaded, begining neighbor computations..." << std::endl;
    for(auto p : precincts) {
        p->computeNeighbors();
    }
    std::cout << "Data loading complete. Calculated state population is " << population << "..." << std::endl;
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

    // present available demographic datasets to the user
    std::cout << "Available demographic datasets:" << std::endl;
    int i = 1;
    std::vector<std::string> demoKeys;
    for (const auto& [key, data] : demo) {
        std::cout << i++ << ": " << key << " - " << data.getTitle() << std::endl;
        demoKeys.push_back(key);
    }

    int choice = 0;
    while (choice < 1 || choice > demoKeys.size()) {
        std::cout << "Select a canonical demographic dataset: ";
        std::cin >> choice;
    }

    canonicalDemo = &demo.at(demoKeys[choice - 1]);

    // present available election datasets to the user
    std::cout << "Available election datasets:" << std::endl;
    i = 1;
    std::vector<std::string> elexKeys;
    for (const auto& [key, data] : elex) {
        std::cout << i++ << ": " << key << " - " << data.getTitle() << std::endl;
        elexKeys.push_back(key);
    }

    choice = 0;
    while (choice < 1 || choice > elexKeys.size()) {
        std::cout << "Select a canonical election dataset: ";
        std::cin >> choice;
    }

    canonicalElex = &elex.at(elexKeys[choice - 1]);
}

DataSet& State::getDataSet(const std::string& name) {
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