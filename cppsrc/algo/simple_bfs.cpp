#include "algorithm_definition.hpp"

#include <queue>
#include <iostream>
 

//Draw one district from a random starting precinct
void SimpleBFS::drawMap(State& s) {
    auto dists = s.getDistricts();
    auto origin = s.getRandPrecinct(true);
    int i = 0;
    District *d = dists[0];
    std::queue<Precinct*> queue;
    queue.push(origin);
    while(!queue.empty()) {
        if((d->getPopulation() + s.getAveragePrecinctPop()) > d->targetPop && i < dists.size()-1) {
            i++;
            d = dists[i];
        }
        auto curr = queue.front();
        queue.pop();
        if(curr->isAssigned()) continue;
        logs::info << "Processing precinct " << curr->name << "..." << std::endl;
        if(d->addPrecinct(curr)) {
            for(auto n : curr->getNeighbors()) {
                if(!n->isAssigned()) {
                    logs::info << "\tAdding neighbor " << n->name << " to queue..." << std::endl;
                    queue.push(n);
                }
            }
        }
    }

    std::cout << "DISTRICTS COMPUTED. DISTRICT REPORT:" << std::endl;
    for(auto d : s.getDistricts()) {
        std::cout << "DISTRICT " <<d->id << ": CONTIGUITY " << (d->isContiguous() ? "TRUE" : "FALSE") <<", TARGET POP " << d->targetPop << ", ACTUAL POP " << d->getPopulation() <<std::endl;
        std::cout << "\tCOMPACTNESS(P.P.)" << std::round(d->compactnessPolsbyPopper()*1000)/1000 
            << ", COMPACTNESS(R.)" << std::round(d->compactnessReock()*1000)/1000 << std::endl;
    }

    s.efficiencyGapAnalysis();
    s.partisanExpectedSeatAnalysis();


}