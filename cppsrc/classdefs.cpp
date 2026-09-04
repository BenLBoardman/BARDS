#include "classdefs.hpp"

std::random_device rd;

#include <iostream>

Precinct::Precinct(State& state, const JsonValue& json) : state(state), ElectoralEntity(json["properties"]["id"].asString(), json["properties"]["name"].asString()) {
    district = nullptr;
    const JsonValue& properties = json["properties"];


    //demographic data
    std::set<std::string> datasets = state.getDatasetNames();
    for(std::string dataName : datasets) {
        if(state.getDataSet(dataName).isDemographic()) {
            auto& s = dynamic_cast<DemographicData&>(state.getDataSet(dataName));            
            demo.emplace(std::piecewise_construct, std::forward_as_tuple(s.name), std::forward_as_tuple(s, properties["datasets"][dataName].asObject()));
            auto& d = demo.at(s.name);
            s.mergeData(d);
        }
        else {
            auto& s = dynamic_cast<ElectionData&>(state.getDataSet(dataName));
            elex.emplace(std::piecewise_construct, std::forward_as_tuple(s.name), std::forward_as_tuple(s, properties["datasets"][dataName].asObject()));
            auto& e = elex.at(s.name);
            s.mergeData(e);
        }
    }

    auto d = &demo.at(state.getCanonicalDemo()->name);
    canonicalDemo = d;
    population = canonicalDemo->getTotal();

    auto e = &elex.at(state.getCanonicalElex()->name);
    canonicalElex = e;

    //geometry
    geo.loadGeometry(json["geometry"]);
}

void Precinct::computeNeighbors() {
    const std::set<GeoLine*> boundaries = geo.getLines();
    for(auto segment : boundaries) {
        for(auto owner : segment->getOwners()) {
            if(owner == &geo || owner == nullptr || !owner->getOwner().isPrecinct) continue;
            neighbors.push_back(dynamic_cast<Precinct *>(&owner->getOwner()));
        }
    }
}

Precinct* Precinct::getRandNeighbor(bool requireUnassigned) {
    std::uniform_int_distribution<int> rand(0, neighbors.size()-1);
    Precinct *p;
    do {
        p = neighbors[rand(rd)];
    } while(!requireUnassigned || !p->isAssigned());
    return p;
}

District::District(State& state, std::string id, int targetPop) : state(state), ElectoralEntity(id, std::string("District "+id)), targetPop(targetPop) { 
    population = 0;
    //generate empty data sets
    std::set<std::string> datasets = state.getDatasetNames();
    for(std::string dataName : datasets) {
        if(state.getDataSet(dataName).isDemographic()) {
            auto& s = dynamic_cast<DemographicData&>(state.getDataSet(dataName));            
            demo.emplace(std::piecewise_construct, std::forward_as_tuple(s.name), std::forward_as_tuple(s));
        }
        else {
            auto& s = dynamic_cast<ElectionData&>(state.getDataSet(dataName));
            elex.emplace(std::piecewise_construct, std::forward_as_tuple(s.name), std::forward_as_tuple(s));
        }
    }

    auto d = &demo.at(state.getCanonicalDemo()->name);
    canonicalDemo = d;
    population = canonicalDemo->getTotal();

    auto e = &elex.at(state.getCanonicalElex()->name);
    canonicalElex = e;
}

bool District::addPrecinct(Precinct* p) {
    if(p->isAssigned()) {
        std::cout << "Attempt to add precinct to district when it is already assigned to a district" << std::endl;
        return false;
    }
    p->setDistrict(this);
    precincts.push_back(p);
    population += p->getPopulation();
    geo.mergeGeometry(p->getGeo());
    for(auto& [key, d] : p->getDemo()) {
        demo.at(key).mergeData(d);
    }
    for(auto& [key, e] : p->getElex()) {
        elex.at(key).mergeData(e);
    }

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
    for(auto& [key, d] : p->getDemo()) {
        demo.at(key).unmergeData(d);
    }
    for(auto& [key, e] : p->getElex()) {
        elex.at(key).unmergeData(e);
    }
    return true;
}

State::State(std::string id, std::string name, int districtCount) : ElectoralEntity(id, name), districtCount(districtCount) {
    districts = std::vector<District*>();
    population = 0;
}

void State::addPrecinct(Precinct& p) {
    precincts.emplace_back(&p);
    population += p.getPopulation();
}

Precinct* State::getRandPrecinct(bool requireUnassigned) {
    std::uniform_int_distribution<int> rand(0, precincts.size()-1);
    Precinct *p;
    do {
        p = precincts[rand(rd)];
    } while(requireUnassigned && p->isAssigned());
    return p;
}

void State::finishProcessing() {
    int target = population / districtCount;
    int rem = population % districtCount;
    for(int i = 0; i < districtCount; i++) {
        districts.push_back(new District(*this, std::to_string(i+1), target + (rem != 0)));
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
        std::cout << i++ << ": " << key << " - " << data.name << std::endl;
        demoKeys.push_back(key);
    }

    int choice = 0;
    while (choice < 1 || choice > demoKeys.size()) {
        std::cout << "Select a canonical demographic dataset: ";
        std::cin >> choice;
    }

    auto d = &demo.at(demoKeys[choice - 1]);
    canonicalDemo = d;

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

    auto e = &elex.at(elexKeys[choice - 1]);
    canonicalElex = e;
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

double State::compactnessPolsbyPopper() {
    double tot = 0;
    for(auto d : districts) {
        tot += d->compactnessPolsbyPopper();
    }
    return tot / districtCount;
}
double State::compactnessReock() {
    double tot = 0;
    for(auto d : districts) {
        tot += d->compactnessReock();
    }
    return tot / districtCount;
}

bool State::isComplete() {
    for(auto p : precincts) {
        if(!p->isAssigned())
            return false;
    }
    return true;
}

double State::popDeviation() {
    double smallest = -1, largest = -1;
    double avgTarget = 1.0*population / districts.size();
    for(auto d : districts) {
        if(smallest == -1 || d->getPopulation() < smallest)
            smallest = d->getPopulation();
        if(largest == -1 || d->getPopulation() > largest)
            largest = d->getPopulation();
    }
    return (largest - smallest)/avgTarget;
}