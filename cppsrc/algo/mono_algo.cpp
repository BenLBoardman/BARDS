#include "algorithm_definition.hpp"

#include <iostream> 

void MonoDistrictTest::drawMap(State& s) {
    District* d = s.getDistrict(1);
    for(Precinct* p : s.getPrecincts()) {
        d->addPrecinct(p);
    }

    std::cout << "DISTRICTS COMPUTED. DISTRICT REPORT:" << std::endl;
    for(auto d : s.getDistricts()) {
        std::cout << "DISTRICT " <<d->id << ": CONTIGUITY " << (d->isContiguous() ? "TRUE" : "FALSE") <<", TARGET POP " << d->target << ", ACTUAL POP " << d->getPopulation() <<std::endl;
        std::cout << "\tCOMPACTNESS(P.P.)" << std::round(d->compactnessPolsbyPopper()*1000)/1000 
            << ", COMPACTNESS(R.)" << std::round(d->compactnessReock()*1000)/1000 << std::endl;
    }
}