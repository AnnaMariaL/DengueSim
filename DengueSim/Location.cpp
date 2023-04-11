//
//  Location.cpp
//  DD
//
//  Copyright © 2022 Anna Maria Langmüller. All rights reserved.
//

#include <stdio.h>
#include <iterator>
#include <math.h>
#include "Location.h"
#include "Human.h"

std::ostream &print(std::ostream &p_os, const Location &p_location)
{
    p_os << p_location.getLocationID() << " " <<  p_location.getRecentInfectedVisitorsCount() << " " << p_location.getRecentRiskScore();
    return p_os;
}

void generateLocations(const int32_t p_locationCountToAdd, std::vector<Location> *p_locations, const double p_currentRiskScore, const size_t p_numberTicksToTrack)
{ //use pointer to locations to avoid copying
    for (int32_t i = 0; i < p_locationCountToAdd; i++)
        p_locations->emplace_back(i, p_currentRiskScore, p_numberTicksToTrack);
}

void Location::registerInfectiousVisits(Human &p_visitor)
{
    if ( (p_visitor.getInfectionStatus() == InfectionStatus::kInfected) && (p_visitor.getRemainingTicksInStatus() > 0) )
        infectedVisitsCount_++; //count infectious visits
}

void Location::storeRiskScoreAndNumberOfInfectedVisitors(double p_currentDiseaseEstablishment)
{
    infectedVisitsCountsHistory_.push_front(infectedVisitsCount_); //add recent infectious visits count to deque
    riskScoreHistory_.push_front(riskScore_); //add recent risk score to deque
    
    if (infectedVisitsCountsHistory_.size() > numberTicksToTrack_)
        infectedVisitsCountsHistory_.resize(numberTicksToTrack_); //resize deque if necessary
    if (riskScoreHistory_.size() > numberTicksToTrack_)
        riskScoreHistory_.resize(numberTicksToTrack_); //resize deque if necessary
    
    infectedVisitsCount_ = 0; //reset infectedVisitsCount_
    riskScore_ = p_currentDiseaseEstablishment; //update riskScore_
}

void Location::printRiskScoreHistory(void)
{
    if (!riskScoreHistory_.empty()) {
        for (std::deque<double>::iterator it = riskScoreHistory_.begin(); it != riskScoreHistory_.end(); ++it) std::cout << " " << *it;
        std::cout << std::endl;
    } else {
        std::cout << "Empty riskScoreHistory_." << std::endl;
    }
}

void Location::printVisitHistory(void){
    if(!infectedVisitsCountsHistory_.empty()) {
        for (std::deque<int32_t>::iterator it = infectedVisitsCountsHistory_.begin(); it != infectedVisitsCountsHistory_.end(); ++it) std::cout << " " << *it;
        std::cout << std::endl;
    } else {
        std::cout << "Empty infectedVisitsCountHistory_." << std::endl;
    }
}
