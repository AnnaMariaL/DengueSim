//
//  main.cpp
//  DDv0
//
//  Created by Anna Maria on 10/17/22.
//


#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

#include "Human.h"
#include "Location.h"



using namespace std;

int main(int argc, const char * argv[]) {
    std::cout << "Now in main()..." << std::endl;
    
    vector<Location> vLocation; //empty vector location
    vector<Human> vHuman; //empty vector humans
    
    int32_t locationCount=10000; //total number of locations
    int NumberTicks=360; //total number of simulated ticks
    const double randomProbability=0.5939354; //Reiner et al. (2014)
    const double randomSize=9.01; //Reiner et al. (2014)
    const long int randomSeed=420; //random number seed
    
    const gsl_rng_type * prandomNumberType; //build random number generator (GSL library)
    gsl_rng * prandomNumberGenerator;
    gsl_rng_env_setup();
    prandomNumberType = gsl_rng_default;
    prandomNumberGenerator = gsl_rng_alloc(prandomNumberType);
    gsl_rng_set(prandomNumberGenerator, randomSeed);
    
    generateLocations(locationCount, &vLocation); //set up locations (use pointer to avoid copying)
    generateHumans(vLocation, &vHuman, randomProbability,randomSize,prandomNumberGenerator);//set up humans, # of humans follows negative binomial distribution

    auto infectionSeed = gsl_rng_uniform_int(prandomNumberGenerator, vHuman.size()); //seed infection
    vHuman[infectionSeed].initiateInfection();
    
    ofstream myfile;
    myfile.open("output.txt");
    if(!myfile.is_open()) {
        cerr << "Can not open file";
        return -1;
    }
    
    for (int currentTick=0; currentTick<NumberTicks; currentTick++) {//for each tick
        for(auto &rHuman:vHuman) rHuman.generateMovement(&vLocation,prandomNumberGenerator,1); //generate movement
        for(auto &rLocation:vLocation) rLocation.updateRiskScore(); //update risk score for each location
        for(auto &rHuman:vHuman) rHuman.propagateInfection(10); //update infection status
        
        auto InfectiousCount = count_if(vHuman.begin(), vHuman.end(), [](Human &rHuman) {return rHuman.getInfectiousDays()>0;}); //generate output
        auto SusceptibleCount = vHuman.size()-InfectiousCount;
        myfile << currentTick << "\t" << InfectiousCount << "\t" << SusceptibleCount << endl;
    }
    myfile.close();
    gsl_rng_free(prandomNumberGenerator);
return 0;
    
}
