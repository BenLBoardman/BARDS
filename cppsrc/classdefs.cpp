#include "classdefs.hpp"

Precinct::Precinct() {}

Precinct::Precinct(State& state, std::string json) : state(state) {
    //todo - process JSON into a precinct data - includes calling geometry constructor and loading demographics/election data sets
}

int Precinct::getPopulation() {
    return population;
}

std::set<DemographicData<Precinct>> Precinct::getDemo() {
    return demo;
}

std::set<ElectionData<Precinct>> Precinct::getElex() {
    return elex;
}

District::District(){}

District::District(State& state, int target) : state(state), target(target) {}

State::State(){}

State::State(std::string name, int districtCount) : name(name) {
    districts = std::vector<District*>(districtCount);
    //todo - initialize demographics & election data
}

void State::addPrecinct(Precinct& p) {
    precincts.emplace_back(std::ref(p));
    population += p.getPopulation();
    
    //TODO - merge precinct demographics & election data into state
}

void State::finishProcessing() {
    int target = population / districts.size();
    int rem = population % districts.size();
    for(int i = 0; i < districts.size(); i++) {
        districts[i] = new District(*this, target + (rem != 0));
        rem -= (rem != 0);
    }
}