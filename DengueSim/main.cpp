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
    vector<Location> vLocation; //empty vector location
    vector<Human> vHuman; //empty vector humans
    int32_t locationCount=200; //total number of locations
    int NumberTicks=600; //total number of simulated ticks
    
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
    //generateHumans(vLocation, &vHuman, 5); //set up humans, constant # of humans per location
    generateHumans(vLocation, &vHuman, randomProbability,randomSize,prandomNumberGenerator);//set up humans, # of humans follows negative binomial distribution

    
    double infectionSeed = gsl_ran_flat(prandomNumberGenerator, 0, vHuman.size()); //seed infection
    int32_t infectionID = int32_t(infectionSeed);
    vHuman[infectionID].initiateInfection();
    ofstream myfile;
    myfile.open("output.txt");
    if(!myfile.is_open()) {
        cerr << "Can not open file";
        return -1;
    }
    
    for (int currentTick=0; currentTick<NumberTicks; currentTick++) {//for each tick
        for(auto &rHuman:vHuman) rHuman.generateMovement(&vLocation,prandomNumberGenerator,1); //generate movement
        for(auto &rLocation:vLocation) rLocation.updateInfectionRisk(); //update infection risks
        for(auto &rHuman:vHuman) rHuman.propagateInfection(10); //update infection status
        
        auto InfectiousCount = count_if(vHuman.begin(), vHuman.end(), [](Human &rHuman) {return rHuman.getInfectiousDays()>0;}); //generate output
        auto SusceptibleCount = vHuman.size()-InfectiousCount;
        myfile << currentTick << "\t" << InfectiousCount << "\t" << SusceptibleCount << endl;
    }
    myfile.close();
    gsl_rng_free(prandomNumberGenerator);
return 0;
    
}

//GRAVEYARD

//    for(auto &rLocation:vLocation) {
//        print(cout, rLocation) << endl; //print locations
//    }


//vector<int32_t> vHumanPerLocation(locationCount);
//vHumanPerLocation[myhomeID]++;
//print(cout, rHuman) << endl; //print humans
//int32_t myhomeID;
//myhomeID=rHuman.GetHomeLocation().getLocationID(); //get homeID

//calculate mean # of humans per location
//double mean = accumulate(std::begin(vHumanPerLocation), std::end(vHumanPerLocation), 0.0);
//mean = mean / vHumanPerLocation.size();
//cout << mean << endl;

 //calculate var # of humans per location
//double sq_sum = std::inner_product(vHumanPerLocation.begin(), vHumanPerLocation.end(), vHumanPerLocation.begin(), 0.0);
//double stdev = sq_sum / vHumanPerLocation.size() - mean * mean;
//cout << stdev << endl;
