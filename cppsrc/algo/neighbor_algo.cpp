#include "algorithm_definition.hpp"

#include <queue>
#include <iostream>

void NeighborDistrictTest::drawMap(State& s) {
    District* d = s.getDistrict(1);
    auto origin = s.getPrecincts()[0];
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
}