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

void generateHumans(vector<Location> &p_Locations, vector<Human> *p_Humans, const int32_t p_humanCount) {
    //reference to location (will not be changed), pointer to humans (avoid copying)
    int32_t humanID=0; //initialize human ID
    for (auto &rLocation : p_Locations) {//reference to individual location
        for(int32_t i=0; i<p_humanCount; i++) {
            p_Humans->emplace_back(humanID,rLocation); //add single human
            humanID++; //increment human ID
        }
    }
}

//humansPerLocationNegBinomProb,humansperLocationNegBinomN,randomNumberGenerator
void generateHumans(vector<Location> &p_Locations, vector<Human> *p_Humans, const double p_humansPerLocationNegBinomProb, const double p_humansPerLocationNegBinomN, gsl_rng *p_randomNumberGenerator){
    int32_t numberOfInhabitantsPerLocation;
    int32_t humanID=0;
    
    for (auto &rLocation : p_Locations) {//reference to individual location
        numberOfInhabitantsPerLocation = gsl_ran_negative_binomial(p_randomNumberGenerator, p_humansPerLocationNegBinomProb, p_humansPerLocationNegBinomN);
        for(int32_t i=0; i<numberOfInhabitantsPerLocation; i++) {
            p_Humans->emplace_back(humanID,rLocation); //add single human
            humanID++; //increment human ID
        }
    }
}

void Human::initiateInfection(unsigned int p_exposureDuration){
    infectionStatus_= InfectionStatus::kExposed;
    nTicksInStatus_=p_exposureDuration;
}

void Human::generateMovement(vector<Location> *p_Locations, gsl_rng *p_randomNumberGenerator, const double p_randomMovementShape, const double p_randomMovementRate, const unsigned int p_exposureDuration) {
    
    double randomDrawNumberOfLocations = gsl_ran_gamma(p_randomNumberGenerator, p_randomMovementShape, 1/p_randomMovementRate);
    if (randomDrawNumberOfLocations<=0) randomDrawNumberOfLocations=1;   //visit at least home
    size_t numberOfLocationVisited = (size_t)round(randomDrawNumberOfLocations);
    
    vector<int32_t> locationsVisited;
    size_t visitIndex=1;
    size_t numberTrials=0; //to avoid endless loop if only few locations are present
    
    locationsVisited.emplace_back(getHomeLocation().getLocationID()); //add home location to places visited
    
    while (visitIndex<numberOfLocationVisited) {
        numberTrials++;
        
        if (numberTrials>100*numberOfLocationVisited) {
            cout << "Warning: It took more than "  << numberTrials << " draws to attempt visiting " << numberOfLocationVisited << " locations. ";
            cout << "Sampling terminated for human " << id_ << ". Already sampled places will be visited." << endl;
            break;
        }
        if(numberOfLocationVisited>=p_Locations->size()) {
            cout << "Warning: Human " << id_ << " is scheduled to visit " << numberOfLocationVisited << " locations. Only " << p_Locations->size() << " available. ";
            cout << "Sampling terminated." << endl;
            break;
        }
        
        auto locationIndex = gsl_rng_uniform_int(p_randomNumberGenerator, p_Locations->size());
        auto alreadySampled = find(locationsVisited.begin(), locationsVisited.end(), locationIndex);
        
        if(alreadySampled==end(locationsVisited)) {
            locationsVisited.emplace_back(locationIndex);
            visitIndex++;
        }
    }
    
    for (size_t i=0; i<locationsVisited.size(); i++) {
        int32_t locationIndex = locationsVisited[i];
        (*p_Locations)[locationIndex].registerVisit(*this); //register visit
        
        if(infectionStatus_== InfectionStatus::kSusceptible) { //register potential exposure of susceptibles
            int infectiousVisits = (*p_Locations)[locationIndex].accessVisits(0);
            long double diseaseEstablishment = (*p_Locations)[locationIndex].accessRiskScores(0);
            for (int j=0; j<infectiousVisits; j++) {
                double infectionProbability = gsl_ran_flat(p_randomNumberGenerator, 0 , 1);
                if (infectionProbability<diseaseEstablishment) initiateInfection(p_exposureDuration);
            }
        }
    }
}

void Human::propagateInfection(const unsigned int p_minimumInfectionDuration, const unsigned int p_maximumInfectionDuration, gsl_rng *p_randomNumberGenerator){
    
    if (infectionStatus_ == InfectionStatus::kInfected) {
        if(nTicksInStatus_ == 0) {
            infectionStatus_ = InfectionStatus::kRecovered;
        } else {
            nTicksInStatus_--;
        }
    }
    
    if (infectionStatus_ == InfectionStatus::kExposed) {
        if(nTicksInStatus_ == 0) {
            auto infectionDuration = gsl_ran_flat(p_randomNumberGenerator, p_minimumInfectionDuration-0.5+1e-7, p_maximumInfectionDuration+0.5-1e-7);
            infectionStatus_ = InfectionStatus::kInfected;
            nTicksInStatus_=(unsigned int)round(infectionDuration);
            
        } else {
            nTicksInStatus_--;
        }
    }
    
    if ((infectionStatus_==InfectionStatus::kRecovered) & (nTicksInStatus_>0)) nTicksInStatus_++;
}

std::ostream &print(std::ostream &p_os, InfectionStatus p_status)
{
    switch (p_status)
    {
        case InfectionStatus::kSusceptible: p_os << "S"; break;
        case InfectionStatus::kExposed: p_os << "E"; break;
        case InfectionStatus::kInfected: p_os << "I"; break;
        case InfectionStatus::kRecovered: p_os << "R"; break;
    }
    return p_os;
}

std::ostream &print(std::ostream &p_os, const Human &p_Human){
    p_os << p_Human.id_ << " ";
    p_os << p_Human.rhomeLocation_.getLocationID() << " ";
    print(p_os, p_Human.infectionStatus_);
    p_os << " ";
    p_os << p_Human.nTicksInStatus_;
    return p_os;
}
