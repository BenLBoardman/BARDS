#include "algorithm_definition.hpp"


void MonoDistrictTest::drawMap(State& s) {
    District* d = s.getDistrict(1);
    for(Precinct* p : s.getPrecincts()) {
        d->addPrecinct(p);
    }
}