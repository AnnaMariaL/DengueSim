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

using namespace std;

void generateHumans(vector<Location> &rLocations, vector<Human> *pHumans, const int32_t humanCount) {
    //reference to location (will not be changed), pointer to humans (avoid copying)
    int32_t humanID=0; //initialize human ID
    for (auto &rLocation : rLocations) {//reference to individual location
        for(int32_t i=0; i<humanCount; i++) {
            pHumans->emplace_back(humanID,rLocation,0); //add single human
            humanID++; //increment human ID
        }
    }
}

void generateHumans(vector<Location> &rLocations, vector<Human> *pHumans, const double randomProbability, const double randomSize, gsl_rng *prandomNumberGenerator){
    unsigned int randomNumber;
    int32_t humanID=0;
    
    for (auto &rLocation : rLocations) {//reference to individual location
        randomNumber = gsl_ran_negative_binomial(prandomNumberGenerator, randomProbability, randomSize);
        for(int32_t i=0; i<randomNumber; i++) {
            pHumans->emplace_back(humanID,rLocation,0); //add single human
            humanID++; //increment human ID
        }
    }
}

void Human::generateMovement(vector<Location> *pLocations, gsl_rng *prandomNumberGenerator, const double mu) {
    unsigned int randomNumber;
    unsigned int visitIndex=0;
    randomNumber = gsl_ran_poisson(prandomNumberGenerator, mu); //determine total number of visited locations
    
    while (visitIndex<randomNumber) { //while locations to visit
        double randomUniform = gsl_ran_flat(prandomNumberGenerator, 0, pLocations->size()-1); //sample location
        int32_t LocationIndex = round(randomUniform);
        if ((*pLocations)[LocationIndex].getLocationID()!=rhomeLocation.getLocationID()) { //if not home location
            visitIndex++; //increment
            (*pLocations)[LocationIndex].registerVisit(*this); //register visit in location
        }
    }
    rhomeLocation.registerVisit(*this); //always increment home Location
};

std::ostream &print(std::ostream &os, const Human &rHuman){
    os << rHuman.ID << " ";
    os << rHuman.rhomeLocation.getLocationID() << " ";
    os << rHuman.infectiousDays;
    return os;
}
