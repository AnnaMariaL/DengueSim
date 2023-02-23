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

std::ostream &print(std::ostream &os, const Location &rLocation)
{
    os << rLocation.getLocationID() << " " <<  rLocation.getCurrentVisits() << " " << rLocation.getCurrentRiskScore();
    return os;
}

void generateLocations(const int32_t locationCount, vector<Location> *pLocations) { //use pointer to locations to avoid copying
    for(int32_t i=0; i<locationCount; i++)
        pLocations->emplace_back(i);
}

void Location::registerVisit(Human &visitor) {
   if((visitor.getInfectionStatus()==2) & (visitor.getNTicksInStatus()>0)) Currentvisits++; //count infectious visits
}

void Location::updateRiskScore(void) { //calculate new risk score, add to deque, and reset visit counter
    //CurrentRiskScore = 1-pow(0.97, Currentvisits); //calculate current risk score
    CurrentRiskScore = Currentvisits;
    VisitHistory.push_front(Currentvisits); //store number of current visits into deque
    RiskScoreHistory.push_front(CurrentRiskScore); //store current risk score into deque
    
    if(VisitHistory.size()>HistoryLength) VisitHistory.resize(HistoryLength); //resize deque if necessary
    if(RiskScoreHistory.size()>HistoryLength) RiskScoreHistory.resize(HistoryLength); //resize deque if necessary
    
    Currentvisits=0; //reset the number of currentvisits
}

void Location::printRiskScoreHistory(){
    if(!RiskScoreHistory.empty()) {
        for(deque<double>::iterator it = RiskScoreHistory.begin(); it!=RiskScoreHistory.end(); ++it) cout << " " << *it ;
        cout << endl;
    } else {
        cout << "Empty RiskScoreHistory" << endl;
    }
}

void Location::printVisitHistory(){
    if(!VisitHistory.empty()) {
        for (deque<int32_t>::iterator it = VisitHistory.begin(); it != VisitHistory.end(); ++it) cout << " " << *it;
        cout << endl;
    } else {
        cout << "Empty VisitHistory." << endl;
    }
}
