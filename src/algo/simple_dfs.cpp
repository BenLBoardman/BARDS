#include "algorithm_definition.hpp"

#include <iostream>

//Draw one district from a random starting precinct
void SimpleDFS::drawMap(State& s) {
    st = &s;
    dists = s.getDistricts();
    auto origin = s.getRandPrecinct(true);
    distIndex = 0;
    currDist = dists[0];
    process(origin);

}

void SimpleDFS::process(Precinct *prec) {
    if((currDist->getPopulation() + st->getAveragePrecinctPop()) > currDist->targetPop && distIndex < dists.size()-1) {
            distIndex++;
            currDist = dists[distIndex];
        }
    if(prec->isAssigned()) return;
    logs::info << "Processing precinct " << prec->name << "..." << std::endl;
    currDist->addPrecinct(prec);
    prec->permuteNeighbors();
    for(auto n : prec->getNeighbors())
        process(n);
    
}