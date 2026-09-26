#include "algorithm_definition.hpp"

#include <queue>
 

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
        curr->permuteNeighbors();
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


}