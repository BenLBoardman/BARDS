#include "algorithm_definition.hpp"

#include <queue>
#include <vector>
#include <algorithm>


bool originPred(State* s, Precinct* p, std::vector<Precinct*> precs) {
    for(auto pr : precs) {
        if(p->getGeo().distance(&(pr->getGeo())) < 0.1) return false;
    }
    return true;
}

//Draw one district from a random starting precinct
void MultiBFS::drawMap(State& s) {
    auto dists = s.getDistricts();
    int i = 0;
    District *d;
    std::vector<std::queue<Precinct*>*> queues;
    auto origins = s.getRandPrecincts(true, dists.size(), originPred);
    for(int j = 0; j < dists.size(); j++) {
        queues.emplace_back(new std::queue<Precinct*>());
        auto origin = origins[j];
        queues[j]->push(origin);
        logs::info << "District " << j+1 << "'s origin will be " << origin->name << std::endl;
    }
    while(!std::all_of(queues.begin(), queues.end(), [](std::queue<Precinct*>* q) { return q->empty(); })) {
        d = dists[i];
        auto queue = queues[i];
        if (!queue->empty()) {
            auto curr = queue->front();
            queue->pop();
            if(curr->isAssigned()) continue;
            curr->permuteNeighbors();
            logs::info << "Processing precinct " << curr->name << " for district " << i+1 << "..." << std::endl;
            if(d->addPrecinct(curr)) {
                for(auto n : curr->getNeighbors()) {
                    if(!n->isAssigned()) {
                        logs::info << "\tAdding neighbor " << n->name << " to queue..." << std::endl;
                        queue->push(n);
                    }
                }
            }
        }
        i++;
        i = i % dists.size();
    }

}