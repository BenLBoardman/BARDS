#include "classdefs.hpp"
#include "util/ioUtil.hpp"

#include <iostream>
#include <iomanip>
#include <cmath>


void State::populationDeviationAnalysis() {
    District *smallest = nullptr, *largest = nullptr;
    double avgTarget = 1.0*population / districts.size();
    logs::report << std::setprecision(4) << "Population Deviation Analysis:" << std::endl;
    for(auto d : districts) {
        if(smallest == nullptr || d->getPopulation() < smallest->getPopulation())
            smallest = d;
        if(largest == nullptr || d->getPopulation() > largest->getPopulation())
            largest = d;
    }
    double dev = (largest->getPopulation() - smallest->getPopulation())/avgTarget;
    logs::report << "\tThe statewide deviation is " << dev*100 << "\%. Courts typically expect less than 0.75\% deviation, so this map would likely " << (std::abs(dev) < 0.0075 ? "be " : "not be ") << "considered legal." << std::endl;
    logs::report << "\t The largest district is District " << largest->id << ", while the smallest district is District " << smallest->id << "." << std::endl;
}   

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
    logs::report << std::setprecision(4);
    logs::report << "Statewide two-party efficiency gap: " << std::abs(gap*100) << "% biased towards " << (gap > 0 ? "Democrats." : "Republicans.") << std::endl;
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
    logs::report << "Statewide Partisan Fairness Analysis: " << std::endl;
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
    logs::report << std::setprecision(4);
    logs::report << "\tIn the selected election, Democrats won " << vFracD*100 << "\% of the vote, while Republicans won " 
        << vFracR*100 << "\%." << std::endl;
    logs::report << "\tIn a truly proportional map, Democrats would win " << proportionalD << " seats, while Republicans would win " 
        << proportionalR << " seats." << std::endl;
    logs::report << "\tBelow are seat-by-seat two party percentages and win probabilities (excluding third parties)." <<std::endl;
    for(auto d : districts) {
        auto e = d->getCanonicalElex();
        auto _D = 1.0 * e->getDem() / (e->getDem()+e->getRep());
        auto _R = 1.0 * e->getRep() / (e->getDem()+e->getRep());
        calculatedD = 0.5*(std::erfc((_R-0.5)/0.04/std::sqrt(2.0)));
        calculatedR = 0.5*(std::erfc((_D-0.5)/0.04/std::sqrt(2.0)));

        dSum += calculatedD;
        rSum += calculatedR;
    }
    dSum = dSum < 0.01 ? 0 : dSum;
    rSum = rSum < 0.01 ? 0 : rSum;
    logs::report << "\tStatistical analysis estimates that on this map, Democrats would win " << dSum << " seats, while Republicans would win " 
        << rSum << " seats." << std::endl;
    disprop = (dSum - proportionalD)/districtCount;
    logs::report << "\tThis represents a " << (std::abs(disprop*100)) << "\% disproportionality in favor of" << ((disprop > 0) ? " Democrats." : " Republicans.") << std::endl;
}

void State::contiguityCompactnessAnalysis() {
    bool isContiguous = true;
    for(auto d : districts) {
        if(!d->isContiguous()) {
            isContiguous = false;
            break;
        }
    }
    logs::report << std::setprecision(4) << "Statewide Contiguity/Compactness Analysis:" << std::endl;
    logs::report << "\tThis map " << (isComplete() ? "is" : "is not") << " complete (all precincts assigned to districts)." << std::endl;
    logs::report << "\tThis map " << (isContiguous ? "is" : "is not") << " contiguous." << std::endl;

    double polsby = compactnessPolsbyPopper();
    double reock = compactnessReock();

    logs::report << "\tThis map has a Polsby-Popper compactness of " << polsby <<". Higher numbers are better." << std::endl;
    logs::report << "\tThis map has a Reock compactness of " << reock <<". Higher numbers are better." << std::endl;
}

void State::districtByDistrictAnalysis() {
    logs::report << std::setprecision(4) << "District-by-district statistics:" << std::endl;
    for(auto d : districts) {
        logs::report << "\tDistrict " << d->id << ":" << std::endl;
        logs::report << "\t\tContiguous & free of holes: " << (d->isContiguous() ? "Yes" : "No") << std::endl;
        if(d->isContiguous()) 
            logs::report << "\t\tCompactness: " << d->compactnessPolsbyPopper() << " Polsby-Popper, " << d->compactnessReock() << " Reock." << std::endl;
        else 
            logs::report << "\t\tCannot calculate compactness since this district is not contiguous (or has holes)." << std::endl;
        logs::report << "\t\tPopulation: " << d->getPopulation() << " (Target: " << d->targetPop << "). This is a " << (d->getDeviation()-1)*100 << "\% population deviation." << std::endl;
        auto e = d->getCanonicalElex();
        auto _D = 1.0 * e->getDem() / (e->getDem()+e->getRep());
        auto _R = 1.0 * e->getRep() / (e->getDem()+e->getRep());
        auto calculatedD = 0.5*(std::erfc((_R-0.5)/0.04/std::sqrt(2.0)));
        auto calculatedR = 0.5*(std::erfc((_D-0.5)/0.04/std::sqrt(2.0)));

        logs::report << "\t\tPartisanship: " << _D*100 << "\% Democrats, " << _R*100 << "% Republicans." << std::endl;
        logs::report <<  "\t\t\tStatistical analysis suggests this district will vote"  <<
            (_D > _R ? " Democratic " : " Republican ") << (_D > _R ?  calculatedD*100 : calculatedR*100) << "\% of the time." << std::endl; 
    }
}


void State::fullReport() {
    logs::report << "Map analysis & report using the selected canonical demographic survey " << canonicalDemo->getTitle() << " and election " << canonicalElex->getTitle() << "." << std::endl;
    contiguityCompactnessAnalysis();
    populationDeviationAnalysis();
    partisanExpectedSeatAnalysis();
    efficiencyGapAnalysis();
    districtByDistrictAnalysis();
}