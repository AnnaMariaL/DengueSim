//
//  Location.cpp
//  DD
//
//  Created by Anna Maria Langmüller on 10/20/22.
//  Copyright © 2022 Anna Maria Langmüller. All rights reserved.
//

#include <stdio.h>
#include <iterator>
#include <math.h>
#include "Location.h"
#include "Human.h"

std::ostream &print(std::ostream &p_os, const Location &p_location)
{
    p_os << p_location.getLocationID() << " " <<  p_location.getCurrentVisits() << " " << p_location.getCurrentRiskScore();
    return p_os;
}

void generateLocations(const int32_t p_locationCount, std::vector<Location> *p_locations) { //use pointer to locations to avoid copying
    for(int32_t i=0; i<p_locationCount; i++)
        p_locations->emplace_back(i);
}

void Location::registerVisit(Human &p_visitor) {
    if((p_visitor.getInfectionStatus()==InfectionStatus::kInfected) & (p_visitor.getRemainingTicksInStatus()>0)) currentvisits_++; //count infectious visits
}

void Location::updateCharacteristics(long double p_currentDiseaseEstablishment) { //calculate new risk score, add to deque, and reset visit counter
    visitHistory_.push_front(currentvisits_); //store number of current visits into deque
    riskScoreHistory_.push_front(currentRiskScore_); //store current risk score into deque
    
    if(visitHistory_.size()>historyLength_) visitHistory_.resize(historyLength_); //resize deque if necessary
    if(riskScoreHistory_.size()>historyLength_) riskScoreHistory_.resize(historyLength_); //resize deque if necessary
    
    currentvisits_=0; //reset currentvisits_
    currentRiskScore_ = p_currentDiseaseEstablishment; //update currentRiskScore_
}

void Location::printRiskScoreHistory(void){
    if(!riskScoreHistory_.empty()) {
        for(std::deque<long double>::iterator it = riskScoreHistory_.begin(); it!=riskScoreHistory_.end(); ++it) std::cout << " " << *it ;
        std::cout << std::endl;
    } else {
        std::cout << "Empty riskScoreHistory" << std::endl;
    }
}

void Location::printVisitHistory(void){
    if(!visitHistory_.empty()) {
        for (std::deque<int32_t>::iterator it = visitHistory_.begin(); it != visitHistory_.end(); ++it) std::cout << " " << *it;
        std::cout << std::endl;
    } else {
        std::cout << "Empty visitHistory." << std::endl;
    }
}
