#include "classdefs.hpp"


Precinct::Precinct(State& state, std::string json) : state(state), geo(*this) {
    district = nullptr;
    //todo - process JSON into a precinct data - includes calling geometry constructor and loading demographics/election data sets
}

void Precinct::computeNeighbors() {
    //TODO - use Geometry object & GeoLine to find neighbors of a precinct. Will likely involve functions to find neighbors for a Geometry.
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


District::District(State& state, int target) : state(state), target(target), geo(*this) { population = 0; }


State::State(std::string name, int districtCount) : name(name), districtCount(districtCount) {
    districts = std::vector<District*>();
    population = 0;
    //todo - initialize demographics & election data
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