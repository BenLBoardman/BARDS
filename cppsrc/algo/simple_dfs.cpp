#include "algorithm_definition.hpp"

#include <iostream>

//Draw one district from a random starting precinct
void SimpleDFS::drawMap(State& s) {
    st = &s;
    dists = s.getDistricts();
    auto origin = s.getRandPrecinct(true);
    int i = 0;
    distIndex = 0;
    currDist = dists[0];
    process(origin);

    std::cout << "DISTRICTS COMPUTED. DISTRICT REPORT:" << std::endl;
    for(auto d : s.getDistricts()) {
        std::cout << "DISTRICT " <<d->id << ": CONTIGUITY " << (d->isContiguous() ? "TRUE" : "FALSE") <<", TARGET POP " << d->targetPop << ", ACTUAL POP " << d->getPopulation() <<std::endl;
        std::cout << "\tCOMPACTNESS(P.P.)" << std::round(d->compactnessPolsbyPopper()*1000)/1000 
            << ", COMPACTNESS(R.)" << std::round(d->compactnessReock()*1000)/1000 << std::endl;
    }

    s.efficiencyGapAnalysis();
    s.partisanExpectedSeatAnalysis();
}

void SimpleDFS::process(Precinct *prec) {
    if((currDist->getPopulation() + st->getAveragePrecinctPop()) > currDist->targetPop && distIndex < dists.size()-1) {
            distIndex++;
            currDist = dists[distIndex];
        }
    if(prec->isAssigned()) return;
    currDist->addPrecinct(prec);
    prec->permuteNeighbors();
    for(auto n : prec->getNeighbors())
        process(n);
    
}