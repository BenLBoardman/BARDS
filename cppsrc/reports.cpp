#include "classdefs.hpp"

#include <iostream>
#include <iomanip>
#include <cmath>


void State::efficiencyGapAnalysis() {
    int totalR=0, totalD=0, wastedD=0, wastedR=0;
    double gap;
    for(auto d : districts) {
        auto e = d->getCanonicalElex();
        auto D = e->getDem();
        auto R = e->getRep();
        auto T = e->getTotal();
        wastedD += D > R ? D-(((R+D)/2)+1) : D;
        wastedR += R > D ? R-(((R+D)/2)+1) : R;
        totalD += D;
        totalR += R;
    }
    gap =  1.0*(wastedR-wastedD)/(totalD+totalR);
    std::cout << std::setprecision(4);
    std::cout << "Statewide two-party efficiency gap: " << std::abs(gap*100) << "% biased towards " << (gap > 0 ? "Democrats." : "Republicans.") << std::endl;
}

void State::partisanExpectedSeatAnalysis() {
    double proportionalDeviation, calculatedD = 0, calculatedR = 0,
    vFracD, vFracR;
    int proportionalD, proportionalR, D, R;

    D = canonicalElex->getDem();
    R = canonicalElex->getRep();
    std::cout << "Statewide Partisan Fairness Analysis: " << std::endl;
    //calculate proportional Democratic seat wins
    vFracD = 1.0 * D / canonicalElex->getTotal();
    vFracR = 1.0 * R / canonicalElex->getTotal();
    
    //todo - assign
    proportionalD = std::round(vFracD * districtCount);
    proportionalR = std::round(vFracR * districtCount);
    if(proportionalD+proportionalR < districtCount && (D-D/districtCount*proportionalD > R-R/districtCount*proportionalR)) {
        proportionalD++;
    } else if (proportionalD+proportionalR < districtCount) {
        proportionalR++;
    }
    std::cout << std::setprecision(4);
    std::cout << "\tIn the selected election, democrats won " << vFracD*100 << "\% of the vote, while Republicans won " 
        << vFracR*100 << "\%." << std::endl;
    std::cout << "\tIn a truly proportional map, Democrats would win " << proportionalD << " seats, while Republicans would win " 
        << proportionalR << " seats." << std::endl;
    for(auto d : districts) {
        auto e = d->getCanonicalElex();
        auto _D = 1.0 * canonicalElex->getDem() / canonicalElex->getTotal();
        auto _R = 1.0 * canonicalElex->getRep() / canonicalElex->getTotal();
        calculatedD += 0.5*(std::erfc((_R-0.5)/0.04/std::sqrt(2.0)));
        calculatedR += 0.5*(std::erfc((_D-0.5)/0.04/std::sqrt(2.0)));

        //optional - print out per district analysis here
    }
    calculatedD = calculatedD < 0.01 ? 0 : calculatedD;
    calculatedR = calculatedR < 0.01 ? 0 : calculatedR;
    std::cout << "\tStatistical analysis estimates that on this map, Democrats would win " << calculatedD << " seats, while Republicans would win " 
        << calculatedR << " seats." << std::endl;
    
}