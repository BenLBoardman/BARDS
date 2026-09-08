#include "algorithm_definition.hpp"

#include <iostream> 

void MonoDistrictTest::drawMap(State& s) {
    District* d = s.getDistrict(1);
    for(Precinct* p : s.getPrecincts()) {
        d->addPrecinct(p);
    }

    std::cout << "DISTRICTS COMPUTED. DISTRICT REPORT:" << std::endl;
    std::cout << "MAP " << (s.isComplete() ? "IS " : "IS NOT ") << "COMPLETE." << std::endl;
    for(auto d : s.getDistricts()) {
        std::cout << "DISTRICT " <<d->id << ": CONTIGUITY " << (d->isContiguous() ? "TRUE" : "FALSE") <<", TARGET POP " << d->targetPop << ", ACTUAL POP " << d->getPopulation() <<std::endl;
        std::cout << "\tCOMPACTNESS(P.P.)" << std::round(d->compactnessPolsbyPopper()*1000)/1000 
            << ", COMPACTNESS(R.)" << std::round(d->compactnessReock()*1000)/1000 << std::endl;

        auto demo = s.getCanonicalDemo();
        std::cout <<"STATEWIDE CANONICAL DEMOGRAPHICS (" << demo->getTitle() << "):" << std::endl << "\t";
        std::cout << "TOTAL: "<< demo->getTotal() << std::endl << "\t";
        std::cout << "WHITE: " << demo->getWhite() << std::endl;

    }

    s.efficiencyGapAnalysis();
    s.partisanExpectedSeatAnalysis();
}