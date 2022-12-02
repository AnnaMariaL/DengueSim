//
//  main.cpp
//  DDv0
//
//  Created by Anna Maria on 10/17/22.
//


#include <stdio.h>
#include <iostream>
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
    vector<int32_t> vHumanPerLocation(locationCount);
    
    const double randomProbability=0.5939354; //Reiner et al. (2014)
    const double randomSize=9.01; //Reiner et al. (2014)
    const long int randomSeed=42; //random number seed
    const gsl_rng_type * prandomNumberType; //build random number generator (GSL library)
    gsl_rng * prandomNumberGenerator;
    gsl_rng_env_setup();
    prandomNumberType = gsl_rng_default;
    prandomNumberGenerator = gsl_rng_alloc(prandomNumberType);
    gsl_rng_set(prandomNumberGenerator, randomSeed);
    
    generateLocations(locationCount, &vLocation); //set up locations (use pointer to avoid copying)
    //generateHumans(vLocation, &vHuman, 5); //set up humans, constant # of humans per location
    generateHumans(vLocation, &vHuman, randomProbability,randomSize,prandomNumberGenerator);
    //generateHumans(vLocation, &vHuman, randomProbability, randomSize, prandomNumberGenerator); //set up humans, # of humans follows negative binomial distribution
    
    for(auto &rLocation:vLocation) print(cout, rLocation) << endl; //print locations
    
    int32_t myhomeID;
    
    for(auto &rHuman:vHuman) {
        print(cout, rHuman) << endl; //print humans
        myhomeID=rHuman.GetLocation().getLocationID();
        vHumanPerLocation[myhomeID]++;
    }
   
   //calculate mean # of humans per location
   double mean = accumulate(std::begin(vHumanPerLocation), std::end(vHumanPerLocation), 0.0);
   mean = mean / vHumanPerLocation.size();
   cout << mean << endl;
   
    //calculate var # of humans per location
   double sq_sum = std::inner_product(vHumanPerLocation.begin(), vHumanPerLocation.end(), vHumanPerLocation.begin(), 0.0);
   double stdev = sq_sum / vHumanPerLocation.size() - mean * mean;
   cout << stdev << endl;
    
   gsl_rng_free(prandomNumberGenerator);
return 0;
    
}



//int main (void)
//{
//    const gsl_rng_type * T;
//    gsl_rng * r;
//
//    int i, n=1000;
//    double mu=3.0;
//    vector<int> p;
//
//    gsl_rng_env_setup();
//    T = gsl_rng_default;
//    r = gsl_rng_alloc(T);
//
//    for (i=0; i<n; i++) {
//        unsigned int k = gsl_ran_poisson(r, mu);
//        p.push_back(k);
//        printf(" %u", k);
//    }
//    printf("\n");
//    gsl_rng_free(r);
//
//    double m = accumulate(std::begin(p), std::end(p), 0.0);
//    m = m / p.size();
//    cout << m << endl;
//    cout << "Another change to main." << endl;
//  return 0;
//}
