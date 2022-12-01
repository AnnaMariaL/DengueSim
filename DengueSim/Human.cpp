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

void HumanSetup(vector<Location> &l, vector<Human> *h, const int32_t n) {
    //reference to location (will not be changed), pointer to humans (avoid copying)
    int32_t id=0; //initialize human ID
    for (auto &a : l) {//reference to individual location
        for(int32_t i=0; i<n; i++) {
            h->emplace_back(id,a,0); //add single human
            id++; //increment human ID
        }
    }
}

void HumanSetup(vector<Location> &l, vector<Human> *h, const double p, const double n, const long int rns){
    const gsl_rng_type * T;
    gsl_rng * r;
    unsigned int k;
    int32_t id=0;
    
    gsl_rng_env_setup();
    T = gsl_rng_default;
    r = gsl_rng_alloc(T);
    gsl_rng_set(r, rns);
    
    for (auto &a : l) {//reference to individual location
        k = gsl_ran_negative_binomial(r, p, n);
        for(int32_t i=0; i<k; i++) {
            h->emplace_back(id,a,0); //add single human
            id++; //increment human ID
        }
    }
    gsl_rng_free(r);
}

std::ostream &print(std::ostream &os, const Human &h){
    os << h.ID << " ";
    print(os, h.home);
    os << " " << h.infected_days;
    return os;
}

