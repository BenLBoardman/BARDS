#include "algorithm_definition.hpp"

#include <queue>
#include <iostream>

//Draw one district from a random starting precinct
void NeighborDistrictTest::drawMap(State& s) {
    District* d = s.getDistrict(1);
    auto origin = s.getRandPrecinct(true);
    int count = 25;
    std::queue<Precinct*> queue;
    queue.push(origin);
    while(!queue.empty() && d->getPopulation() < d->target) {
        auto curr = queue.front();
        std::cout << "Processing precinct " << curr->name << "..." << std::endl;
        queue.pop();
        if(d->addPrecinct(curr)) {
            count--;
            for(auto n : curr->getNeighbors()) {
                std::cout << "\tFound neighbor " << n->name << "..." << std::endl;
                if(!n->isAssigned())
                    queue.push(n);
            }
        }
    }

    std::cout << "DISTRICTS COMPUTED. DISTRICT REPORT:" << std::endl;
    for(auto d : s.getDistricts()) {
        std::cout << "DISTRICT " <<d->id << ": CONTIGUITY " << (d->isContiguous() ? "TRUE" : "FALSE") <<", TARGET POP " << d->target << ", ACTUAL POP " << d->getPopulation() <<std::endl;
        std::cout << "\tCOMPACTNESS(P.P.)" << std::round(d->compactnessPolsbyPopper()*1000)/1000 
            << ", COMPACTNESS(R.)" << std::round(d->compactnessReock()*1000)/1000 << std::endl;
    }


}