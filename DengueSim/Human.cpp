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

std::ostream &print(std::ostream &os, const Human &rHuman){
    os << rHuman.ID << " ";
    print(os, rHuman.rhomeLocation);
    os << " " << rHuman.infectiousDays;
    return os;
}

