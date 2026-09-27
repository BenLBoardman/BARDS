/**
 * @file reports.cpp
 * @brief Implementation of State's statewide and district-by-district
 *        report-generation methods (see classdefs.hpp for
 *        method-level documentation).
 */

#include "classdefs.hpp"
#include "util/ioUtil.hpp"

#include <iostream>
#include <iomanip>
#include <cmath>

void State::updateReportCache() {
    if(reportCached) return;
 
    //completeness: every precinct assigned to a (real) district
    complete = true;
    for(auto p : precincts) {
        if(!p->isAssigned()) {
            complete = false;
            break;
        }
    }
 
    //single pass over districts: population extremes, efficiency-gap vote
    //totals, statistically expected seat counts, contiguity, and compactness
    District *smallest = nullptr, *largest = nullptr;
    int wastedD = 0, wastedR = 0, totalD = 0, totalR = 0;
    double expDSeats = 0, expRSeats = 0, totalPolsby = 0, totalReock = 0;
    allDistrictsContiguous = true;
    for(auto d : districts) {
        if(smallest == nullptr || d->getPopulation() < smallest->getPopulation())
            smallest = d;
        if(largest == nullptr || d->getPopulation() > largest->getPopulation())
            largest = d;
 
        auto e = d->getCanonicalElex();
        auto D = e->getDem();
        auto R = e->getRep();
        wastedD += D > R ? D-(((R+D)/2)+1) : D;
        wastedR += R > D ? R-(((R+D)/2)+1) : R;
        totalD += D;
        totalR += R;
 
        expDSeats += d->getDemWinProbability();
        expRSeats += d->getRepWinProbability();
 
        if(!d->isContiguous())
            allDistrictsContiguous = false;
        totalPolsby += d->compactnessPolsbyPopper();
        totalReock += d->compactnessReock();
    }
    smallestDistrict = smallest;
    largestDistrict = largest;
 
    double avgTarget = 1.0*population / districts.size();
    populationDeviation = (largest->getPopulation() - smallest->getPopulation())/avgTarget;
 
    efficiencyGap = 1.0*(wastedR-wastedD)/(totalD+totalR);
 
    expectedDemSeats = expDSeats < 0.01 ? 0 : expDSeats;
    expectedRepSeats = expRSeats < 0.01 ? 0 : expRSeats;
 
    avgPolsbyPopper = totalPolsby / districtCount;
    avgReock = totalReock / districtCount;
 
    //statewide proportional seat allocation, from the canonical election
    int D = canonicalElex->getDem();
    int R = canonicalElex->getRep();
    proportionalDemShare = 1.0 * D / canonicalElex->getTotal();
    proportionalRepShare = 1.0 * R / canonicalElex->getTotal();
    proportionalDemSeats = std::round(proportionalDemShare * districtCount);
    proportionalRepSeats = std::round(proportionalRepShare * districtCount);
    if(proportionalDemSeats+proportionalRepSeats < districtCount && (D-D/districtCount*proportionalDemSeats > R-R/districtCount*proportionalRepSeats)) {
        proportionalDemSeats++;
    } else if (proportionalDemSeats+proportionalRepSeats < districtCount) {
        proportionalRepSeats++;
    }
    seatDisproportionality = (expectedDemSeats - proportionalDemSeats) / districtCount;
 
    reportCached = true;
}
 
void State::populationDeviationAnalysis() {
    updateReportCache();
    logs::report << std::setprecision(4) << "Population Deviation Analysis:" << std::endl;
    logs::report << "\tThe statewide deviation is " << populationDeviation*100 << "\%. Courts typically expect less than 0.75\% deviation, so this map would likely " << (std::abs(populationDeviation) < 0.0075 ? "be " : "not be ") << "considered legal." << std::endl;
    logs::report << "\t The largest district is District " << largestDistrict->id << ", while the smallest district is District " << smallestDistrict->id << "." << std::endl;
}   
 
void State::efficiencyGapAnalysis() {
    updateReportCache();
    logs::report << std::setprecision(4);
    logs::report << "Statewide two-party efficiency gap: " << std::abs(efficiencyGap*100) << "% biased towards " << (efficiencyGap > 0 ? "Democrats." : "Republicans.") << std::endl;
}
 
void State::partisanExpectedSeatAnalysis() {
    updateReportCache();
    logs::report << "Statewide Partisan Fairness Analysis: " << std::endl;
    logs::report << std::setprecision(4);
    logs::report << "\tIn the selected election, Democrats won " << proportionalDemShare*100 << "\% of the vote, while Republicans won " 
        << proportionalRepShare*100 << "\%." << std::endl;
    logs::report << "\tIn a truly proportional map, Democrats would win " << proportionalDemSeats << " seats, while Republicans would win " 
        << proportionalRepSeats << " seats." << std::endl;
    logs::report << "\tBelow are seat-by-seat two party percentages and win probabilities (excluding third parties)." <<std::endl;
    logs::report << "\tStatistical analysis estimates that on this map, Democrats would win " << expectedDemSeats << " seats, while Republicans would win " 
        << expectedRepSeats << " seats." << std::endl;
    logs::report << "\tThis represents a " << (std::abs(seatDisproportionality*100)) << "\% disproportionality in favor of" << ((seatDisproportionality > 0) ? " Democrats." : " Republicans.") << std::endl;
}
 
void State::contiguityCompactnessAnalysis() {
    updateReportCache();
    logs::report << std::setprecision(4) << "Statewide Contiguity/Compactness Analysis:" << std::endl;
    logs::report << "\tThis map " << (complete ? "is" : "is not") << " complete (all precincts assigned to districts)." << std::endl;
    logs::report << "\tThis map " << (allDistrictsContiguous ? "is" : "is not") << " contiguous." << std::endl;
    logs::report << "\tThis map has a Polsby-Popper compactness of " << avgPolsbyPopper <<". Higher numbers are better." << std::endl;
    logs::report << "\tThis map has a Reock compactness of " << avgReock <<". Higher numbers are better." << std::endl;
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
        double _D = d->getDemVoteShare();
        double _R = d->getRepVoteShare();
 
        logs::report << "\t\tPartisanship: " << _D*100 << "\% Democrats, " << _R*100 << "% Republicans." << std::endl;
        logs::report <<  "\t\t\tStatistical analysis suggests this district will vote"  <<
            (_D > _R ? " Democratic " : " Republican ") << (_D > _R ?  d->getDemWinProbability()*100 : d->getRepWinProbability()*100) << "\% of the time." << std::endl; 
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