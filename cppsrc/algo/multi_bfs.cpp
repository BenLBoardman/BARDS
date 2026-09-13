#include "algorithm_definition.hpp"

#include <queue>
#include <vector>
#include <algorithm>
 

//Draw one district from a random starting precinct
void MultiBFS::drawMap(State& s) {
    auto dists = s.getDistricts();
    int i = 0;
    District *d;
    std::vector<std::queue<Precinct*>*> queues;
    auto origins = s.getRandPrecincts(true, dists.size());
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

    std::cout << "DISTRICTS COMPUTED. DISTRICT REPORT:" << std::endl;
    for(auto d : s.getDistricts()) {
        std::cout << "DISTRICT " <<d->id << ": CONTIGUITY " << (d->isContiguous() ? "TRUE" : "FALSE") <<", TARGET POP " << d->targetPop << ", ACTUAL POP " << d->getPopulation() <<std::endl;
        std::cout << "\tCOMPACTNESS(P.P.)" << std::round(d->compactnessPolsbyPopper()*1000)/1000 
            << ", COMPACTNESS(R.)" << std::round(d->compactnessReock()*1000)/1000 << std::endl;
    }

    s.efficiencyGapAnalysis();
    s.partisanExpectedSeatAnalysis();


}