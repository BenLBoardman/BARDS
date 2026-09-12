#include "classdefs.hpp"

#include <iostream>
#include <iomanip>
#include <cmath>

/**
 * Calculate and print the two-party efficiency gap across an entire state.
 * The efficiency gap is the difference between each party's wasted votes as a
 * fraction of the total two-party votes cast in an election.
 */
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

/**
 * Calculate and print analysis of the seat count for both parties in a proportional map,
 * and then use statistics to estimate and print the expected seat count of the current map.
 */
void State::partisanExpectedSeatAnalysis() {
    double proportionalDeviation, calculatedD, calculatedR, dSum = 0, rSum = 0,
    vFracD, vFracR, disprop;
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
    std::cout << "\tIn the selected election, Democrats won " << vFracD*100 << "\% of the vote, while Republicans won " 
        << vFracR*100 << "\%." << std::endl;
    std::cout << "\tIn a truly proportional map, Democrats would win " << proportionalD << " seats, while Republicans would win " 
        << proportionalR << " seats." << std::endl;
    std::cout << "\tBelow are seat-by-seat two party percentages and win probabilities (excluding third parties)." <<std::endl;
    for(auto d : districts) {
        auto e = d->getCanonicalElex();
        auto _D = 1.0 * e->getDem() / (e->getDem()+e->getRep());
        auto _R = 1.0 * e->getRep() / (e->getDem()+e->getRep());
        calculatedD = 0.5*(std::erfc((_R-0.5)/0.04/std::sqrt(2.0)));
        calculatedR = 0.5*(std::erfc((_D-0.5)/0.04/std::sqrt(2.0)));

        std::cout << "\t\tDistrict 1: " << _D*100 << "\% Democrats, " << _R*100 << "% Republicans." << std::endl;
        std::cout <<  "\t\t\tStatistical analysis suggests this district will vote"  <<
            (_D > _R ? " Democratic " : " Republican") << (_D > _R ?  calculatedD*100 : calculatedR*100) << "\% of the time." << std::endl; 
        //optional - print out per district analysis here

        dSum += calculatedD;
        rSum += calculatedR;
    }
    dSum = dSum < 0.01 ? 0 : dSum;
    rSum = rSum < 0.01 ? 0 : rSum;
    std::cout << "\tStatistical analysis estimates that on this map, Democrats would win " << dSum << " seats, while Republicans would win " 
        << rSum << " seats." << std::endl;
    disprop = (dSum - proportionalD)/districtCount;
    std::cout << "\tThis represents a " << (std::abs(disprop*100)) << "\% disproportionality in favor of" << ((disprop > 0) ? " Democrats." : " Republicans.") << std::endl;
}