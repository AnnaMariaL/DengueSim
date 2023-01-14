//
//  Human.cpp
//  DD
//
//  Created by Anna Maria Langmüller on 10/20/22.
//  Copyright © 2022 Anna Maria Langmüller. All rights reserved.
//

#include <stdio.h>
#include "Human.h"
#include "Location.h"
#include <algorithm>

using namespace std;

void generateHumans(vector<Location> &rLocations, vector<Human> *pHumans, const int32_t humanCount) {
    //reference to location (will not be changed), pointer to humans (avoid copying)
    int32_t humanID=0; //initialize human ID
    for (auto &rLocation : rLocations) {//reference to individual location
        for(int32_t i=0; i<humanCount; i++) {
            pHumans->emplace_back(humanID,rLocation); //add single human
            humanID++; //increment human ID
        }
    }
}

void generateHumans(vector<Location> &rLocations, vector<Human> *pHumans, const double randomProbability, const double randomSize, gsl_rng *prandomNumberGenerator){
    unsigned int InhabitantsPerLocation;
    int32_t humanID=0;
    
    for (auto &rLocation : rLocations) {//reference to individual location
        InhabitantsPerLocation = gsl_ran_negative_binomial(prandomNumberGenerator, randomProbability, randomSize);
        for(int32_t i=0; i<InhabitantsPerLocation; i++) {
            pHumans->emplace_back(humanID,rLocation); //add single human
            humanID++; //increment human ID
        }
    }
}

void Human::initiateInfection(unsigned int ExposedDuration){
    InfectionStatus=1;
    NTicksInStatus=ExposedDuration;
}

void Human::generateMovement(vector<Location> *pLocations, gsl_rng *prandomNumberGenerator, const double mu, unsigned int ExposedDuration) {
    unsigned int NumberOfLocationVisited;
    NumberOfLocationVisited = gsl_ran_poisson(prandomNumberGenerator, mu); //determine total number of visited locations
    
    vector<int32_t> LocationsVisited;
    int32_t VisitIndex=0;
    int32_t NumberTrials=0; //to avoid endless loop if only few locations are present
    
    while (VisitIndex<NumberOfLocationVisited) {
        
        NumberTrials++;
        
        if (NumberTrials>100*NumberOfLocationVisited) {
            cout << "Warning: It took more than "  << NumberTrials << " draws to attempt visiting " << NumberOfLocationVisited << " locations. ";
            cout << "Sampling terminated for human " << ID << ". Already sampled places will be visited." << endl;
            break;
        }
        
        if(NumberOfLocationVisited>=pLocations->size()) {
            cout << "Warning: Human " << ID << " is scheduled to visit " << NumberOfLocationVisited << " locations. Only " << pLocations->size()-1 << " available. ";
            cout << "Sampling terminated." << endl;
            break;
        }
        
        
        auto LocationIndex = gsl_rng_uniform_int(prandomNumberGenerator, pLocations->size());
        auto AlreadySampled = find(LocationsVisited.begin(), LocationsVisited.end(), LocationIndex);
        
        if((AlreadySampled==end(LocationsVisited)) & (LocationIndex != GetHomeLocation().getLocationID())) {
            LocationsVisited.emplace_back(LocationIndex);
            VisitIndex++;
        }
    }
    
    LocationsVisited.emplace_back(GetHomeLocation().getLocationID()); //add home location to places visited
    
    for (int i=0; i<LocationsVisited.size(); i++) {
        int32_t LocationIndex = LocationsVisited[i];
        (*pLocations)[LocationIndex].registerVisit(*this); //register visit
        
        if(InfectionStatus==0) { //register potential exposure of susceptibles
            double InfectionProbability = gsl_ran_flat(prandomNumberGenerator, 0, 1);
            if(InfectionProbability<(*pLocations)[LocationIndex].getCurrentRiskScore()) {
                initiateInfection(ExposedDuration);
            }
        }
    }
}

void Human::propagateInfection(unsigned int InfectionDuration){
    if ((InfectionStatus > 0) & (InfectionStatus < 3) & (NTicksInStatus>0)) NTicksInStatus--; //decrement exposed/infectious period
    if (InfectionStatus == 3) NTicksInStatus++; //increment recovered period (assuming lifelong immunity)
    
    if ((InfectionStatus==1) & (NTicksInStatus==0)) { //exposed --> infectious
        InfectionStatus++;
        NTicksInStatus=InfectionDuration;
    }
    
    if ((InfectionStatus==2) & (NTicksInStatus==0)) { //infectious --> recovered
        InfectionStatus++;
        NTicksInStatus=0;
    }
}

std::ostream &print(std::ostream &os, const Human &rHuman){
    os << rHuman.ID << " ";
    os << rHuman.rhomeLocation.getLocationID() << " ";
    os << rHuman.InfectionStatus << " ";
    os << rHuman.NTicksInStatus;
    return os;
}
