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

void generateHumans(std::vector<Location> &p_locations, std::vector<Human> *p_humans, const int32_t p_humanCount, int p_nTicks) {
    //reference to location (will not be changed), pointer to humans (avoid copying)
    int32_t humanID=0; //initialize human ID
    for (auto &location : p_locations) {//reference to individual location
        for(int32_t i=0; i<p_humanCount; i++) {
            p_humans->emplace_back(humanID,location,InfectionStatus::kSusceptible,0,p_nTicks); //add single human
            humanID++; //increment human ID
        }
    }
}

void generateHumans(std::vector<Location> &p_locations, std::vector<Human> *p_humans, const double p_humansPerLocationNegBinomProb, const double p_humansPerLocationNegBinomN, gsl_rng *p_rng, int p_nTicks){
    int32_t numberOfInhabitantsPerLocation;
    int32_t humanID=0;
    
    for (auto &location : p_locations) {//reference to individual location
        numberOfInhabitantsPerLocation = gsl_ran_negative_binomial(p_rng, p_humansPerLocationNegBinomProb, p_humansPerLocationNegBinomN);
        for(int32_t i=0; i<numberOfInhabitantsPerLocation; i++) {
            p_humans->emplace_back(humanID,location,InfectionStatus::kSusceptible,0,p_nTicks); //add single human
            humanID++; //increment human ID
        }
    }
}

void Human::initiateInfection(unsigned int p_exposureDuration){
    infectionStatus_= InfectionStatus::kExposed;
    remainingTicks_ = p_exposureDuration;
}

void Human::generateMovement(std::vector<Location> *p_locations, gsl_rng *p_rng, const double p_randomMovementShape, const double p_randomMovementRate, const unsigned int p_exposureDuration) {
    
    double randomDrawNumberOfLocations = gsl_ran_gamma(p_rng, p_randomMovementShape, 1/p_randomMovementRate);
    if (randomDrawNumberOfLocations<=0) randomDrawNumberOfLocations=0;
    size_t numberOfLocationVisited = (size_t)round(randomDrawNumberOfLocations);
    
    std::vector<int32_t> locationsVisited;
    size_t visitIndex=1;
    size_t numberTrials=0; //to avoid endless loop if only few locations are present
    
    locationsVisited.emplace_back(getHomeLocation().getLocationID()); //add home location to places visited
    
    while (visitIndex<numberOfLocationVisited) {
        numberTrials++;
        
        if (numberTrials>100*numberOfLocationVisited) {
            std::cout << "Warning: It took more than "  << numberTrials << " draws to attempt visiting " << numberOfLocationVisited << " locations. ";
            std::cout << "Sampling terminated for human " << id_ << ". Already sampled places will be visited." << std::endl;
            break;
        }
        if(numberOfLocationVisited>=p_locations->size()) {
            std::cout << "Warning: Human " << id_ << " is scheduled to visit " << numberOfLocationVisited << " locations. Only " << p_locations->size() << " available. ";
            std::cout << "Sampling terminated." << std::endl;
            break;
        }
        
        auto locationIndex = gsl_rng_uniform_int(p_rng, p_locations->size());
        auto alreadySampled = find(locationsVisited.begin(), locationsVisited.end(), locationIndex);
        
        if(alreadySampled==end(locationsVisited)) {
            locationsVisited.emplace_back(locationIndex);
            visitIndex++;
        }
    }
    
    for (size_t i=0; i<locationsVisited.size(); i++) {
        int32_t locationIndex = locationsVisited[i];
        (*p_locations)[locationIndex].registerVisit(*this); //register visit
        
        if(infectionStatus_== InfectionStatus::kSusceptible) { //register potential exposure of susceptibles
            int infectiousVisits = (*p_locations)[locationIndex].accessVisits(0);
            long double diseaseEstablishment = (*p_locations)[locationIndex].accessRiskScores(0);
            for (int j=0; j<infectiousVisits; j++) {
                double infectionProbability = gsl_ran_flat(p_rng, 0 , 1);
                if (infectionProbability<diseaseEstablishment) initiateInfection(p_exposureDuration);
            }
        }
    }
}

void Human::propagateInfection(const unsigned int p_minimumInfectionDuration, const unsigned int p_maximumInfectionDuration, gsl_rng *p_rng, int p_remainingTicks){

    switch (infectionStatus_) {
        case InfectionStatus::kSusceptible:
            elapsedTicks_++;
            remainingTicks_--;
            break;
        case InfectionStatus::kExposed:
            if(remainingTicks_ == 0) { //exposed --> infected if exposed period has passed
                auto infectionDuration = gsl_ran_flat(p_rng, p_minimumInfectionDuration-0.5+1e-7, p_maximumInfectionDuration+0.5-1e-7);
                infectionStatus_ = InfectionStatus::kInfected;
                elapsedTicks_ = 0;
                remainingTicks_ = (unsigned int) round(infectionDuration);
            } else { //decrement exposed period
                elapsedTicks_++;
                remainingTicks_--;
            }
            break;
        case InfectionStatus::kInfected:
            if (remainingTicks_ == 0) { //infected --> recovered if infectious period has passed
                infectionStatus_ = InfectionStatus::kRecovered;
                elapsedTicks_ = 0;
                remainingTicks_ = p_remainingTicks; //TO DO: function that returns current tick
            } else {
                elapsedTicks_++;
                remainingTicks_--;
            }
            break;
        case InfectionStatus::kRecovered: //increment time in recovered period
            elapsedTicks_++;
            remainingTicks_--;
            break;
    }
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

std::ostream &print(std::ostream &p_os, const Human &p_human){
    p_os << p_human.id_ << " ";
    p_os << p_human.homeLocation_.getLocationID() << " ";
    print(p_os, p_human.infectionStatus_) << " ";
    p_os << p_human.elapsedTicks_ << " ";
    p_os << p_human.remainingTicks_ ;
    return p_os;
}
