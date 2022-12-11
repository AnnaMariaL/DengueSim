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
    for(int32_t i=0; i<locationCount; i++) pLocations->emplace_back(i);
}

void Location::registerVisit(Human &visitor) {
    if(visitor.getInfectiousDays()>0) Currentvisits++; //count infectious visits
}

void Location::updateInfectionRisk() { //calculate new risk score, add to deque, and reset visit counter
    calculateCurrentRiskScore();
    storeCurrentVisits();
    storeCurrentRiskScore();
}

void Location::storeCurrentVisits(){
    VisitHistory.push_front(Currentvisits); //store number of visits
    if(VisitHistory.size()>HistoryLength) VisitHistory.resize(HistoryLength); //resize deque if necessary
    resetCurrentVisits(); //reset counter CurrentVisits to 0
    
}

void Location::storeCurrentRiskScore(){
    RiskScoreHistory.push_front(CurrentRiskScore); //store
    if(RiskScoreHistory.size()>HistoryLength) //resize if necessary
        RiskScoreHistory.resize(HistoryLength);
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

void Location::calculateCurrentRiskScore() {
    CurrentRiskScore = 1-pow(0.99, Currentvisits);
}
