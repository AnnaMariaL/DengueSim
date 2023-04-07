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

std::ostream &print(std::ostream &p_os, const Location &p_Location)
{
    p_os << p_Location.getLocationID() << " " <<  p_Location.getCurrentVisits() << " " << p_Location.getCurrentRiskScore();
    return p_os;
}

void generateLocations(const int32_t p_locationCount, vector<Location> *p_Locations) { //use pointer to locations to avoid copying
    for(int32_t i=0; i<p_locationCount; i++)
        p_Locations->emplace_back(i);
}

<<<<<<< HEAD
void Location::registerVisit(Human &p_visitor) {
   if((p_visitor.getInfectionStatus()==InfectionStatus::kInfected) & (p_visitor.getNTicksInStatus()>0)) currentvisits_++; //count infectious visits
=======
void Location::registerVisit(Human &visitor) {
   if((visitor.getInfectionStatus()==InfectionStatus::kInfected) & (visitor.getNTicksInStatus()>0)) Currentvisits++; //count infectious visits
>>>>>>> 6c831185a23d40fc82d23b894f8c6162ea0bb00b
}

void Location::updateCharacteristics(long double p_currentDiseaseEstablishment) { //calculate new risk score, add to deque, and reset visit counter
    currentRiskScore_ = p_currentDiseaseEstablishment; //current disease establishment proportion
    visitHistory_.push_front(currentvisits_); //store number of current visits into deque
    riskScoreHistory_.push_front(currentRiskScore_); //store current risk score into deque
    
    if(visitHistory_.size()>historyLength_) visitHistory_.resize(historyLength_); //resize deque if necessary
    if(riskScoreHistory_.size()>historyLength_) riskScoreHistory_.resize(historyLength_); //resize deque if necessary
    
    currentvisits_=0; //reset the number of currentvisits
}

void Location::printRiskScoreHistory(){
    if(!riskScoreHistory_.empty()) {
        for(deque<long double>::iterator it = riskScoreHistory_.begin(); it!=riskScoreHistory_.end(); ++it) cout << " " << *it ;
        cout << endl;
    } else {
        cout << "Empty RiskScoreHistory" << endl;
    }
}

void Location::printVisitHistory(){
    if(!visitHistory_.empty()) {
        for (deque<int32_t>::iterator it = visitHistory_.begin(); it != visitHistory_.end(); ++it) cout << " " << *it;
        cout << endl;
    } else {
        cout << "Empty VisitHistory." << endl;
    }
}
