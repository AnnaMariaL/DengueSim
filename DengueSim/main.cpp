//
//  main.cpp
//  DDv0
//
//  Created by Anna Maria on 10/17/22.
//


#include <stdio.h>
#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_sf.h>
#include "Human.h"
#include "Location.h"



using namespace std;

int main(int argc, const char * argv[]) {
    
    long int argv_randomSeed=0;
    unsigned int argv_InfectionDuration=0;
    double argv_DiseaseEstablishment=0;
    double argv_randomMovementMu = 0; //8.55 over 15-days period, Stoddard et al. (2013)
    double argv_randomMovementTheta = 0; //11.94 over 15-days period, Stoddard et al. (2013)
    string argv_OutPutFile="foo.txt";
    
    for (int arg = 1; arg < argc; ++arg) //parse command line arguments
    {
        
        if (strcmp(argv[arg], "-seed") == 0) //random seed
        {
            if (arg == argc - 1)
            {
                cerr << "missing argument for -seed!";
                exit(1);
                
            }
            argv_randomSeed = stol(argv[arg+1]);
            arg++;
            cout << "-seed set to " << argv_randomSeed << endl;
        }
        
        if (strcmp(argv[arg], "-gammaInv") == 0) //avg. infectious period (= 1 / recovery rate)
        {
            if (arg == argc - 1)
            {
                cerr << "missing argument for -gammaInv!";
                exit(1);
            }
            argv_InfectionDuration = stoi(argv[arg+1]);
            arg++;
            cout << "-gammaInv set to " << argv_InfectionDuration << endl;
        }
        
        if (strcmp(argv[arg], "-alpha") == 0) //disease establishment proportion
        {
            if (arg == argc - 1)
            {
                cerr << "missing argument for -alpha!";
                exit(1);
            }
            argv_DiseaseEstablishment = stof(argv[arg+1]);
            arg++;
            cout << "-alpha set to " << argv_DiseaseEstablishment << endl;
        }
        
        if(strcmp(argv[arg], "-movementMu") == 0) //average number of places visited, human movement (negative binomial distribution)
        {
            if (arg == argc - 1)
            {
                cerr << "missing argument for -movementMu!";
                exit(1);
            }
            argv_randomMovementMu = stod(argv[arg+1]);
            arg++;
            cout << "-movementMu set to " << argv_randomMovementMu << endl;
        }
        
        if(strcmp(argv[arg], "-movementTheta") == 0) //size parameter, human movement (negative binomial distribution)
        {
            if (arg == argc - 1)
            {
                cerr << "missing argument for -movementTheta!";
                exit(1);
            }
            argv_randomMovementTheta = stod(argv[arg+1]);
            arg++;
            cout << "-movementTheta set to " << argv_randomMovementTheta << endl;
        }
        
        
        if (strcmp(argv[arg], "-output") == 0) //output file
        {
            if (arg == argc - 1)
            {
                cerr << "missing argument for -output!";
                exit(1);
            }
            argv_OutPutFile = argv[arg+1];
            arg++;
            cout << "-output set to " << argv_OutPutFile << endl;
        }
    }
    
    system("pwd");
    
    const long int randomSeed=argv_randomSeed; //variables from argument parser
    const unsigned int InfectionDuration=argv_InfectionDuration;
    const double DiseaseEstablishment=argv_DiseaseEstablishment;
    const double randomMovementMu=argv_randomMovementMu;
    const double randomMovementTheta=argv_randomMovementTheta;
    const string OutPutFile=argv_OutPutFile;
    
    vector<Location> vLocation; //empty vector location
    vector<Human> vHuman; //empty vector humans
    
    int32_t locationCount=1000; //total number of locations
    int NumberTicks=365; //total number of simulated ticks
    const double randomProbability=0.594; //0.5939354, Reiner et al. (2014)
    const double randomSize=9.01; //9.01, Reiner et al. (2014)
    unsigned int ExposureDuration=0; //infection parameters
    
    const gsl_rng_type * prandomNumberType; //build random number generator (GSL library)
    gsl_rng * prandomNumberGenerator;
    gsl_rng_env_setup();
    prandomNumberType = gsl_rng_default;
    prandomNumberGenerator = gsl_rng_alloc(prandomNumberType);
    gsl_rng_set(prandomNumberGenerator, randomSeed);
    
    generateLocations(locationCount, &vLocation); //set up locations (use pointer to avoid copying)
    generateHumans(vLocation, &vHuman, randomProbability,randomSize,prandomNumberGenerator);//set up humans, # of humans per location ~ nbinom

    auto infectionSeed = gsl_rng_uniform_int(prandomNumberGenerator, vHuman.size()); //seed infection
    vHuman[infectionSeed].initiateInfection(ExposureDuration);
    
    ofstream myfile;
    myfile.open(OutPutFile);
    if(!myfile.is_open()) {
       cerr << "Can not open file";
        return -1;
     }
    
    for (int currentTick=1; currentTick<=NumberTicks; currentTick++) {//for each tick
        for (auto &rHuman:vHuman) rHuman.generateMovement(&vLocation, prandomNumberGenerator, randomMovementMu, randomMovementTheta, ExposureDuration, DiseaseEstablishment); //generate movement
        for(auto &rLocation:vLocation) rLocation.updateRiskScore(); //update risk score for each location
        for(auto &rHuman:vHuman) rHuman.propagateInfection(InfectionDuration,prandomNumberGenerator); //update infection status
        auto SusceptibleCount = count_if(vHuman.begin(), vHuman.end(), [](Human &rHuman) {return rHuman.getInfectionStatus()==0;}); //generate output
        auto ExposedCount = count_if(vHuman.begin(), vHuman.end(), [](Human &rHuman) {return rHuman.getInfectionStatus()==1;});
        auto InfectiousCount = count_if(vHuman.begin(), vHuman.end(), [](Human &rHuman) {return rHuman.getInfectionStatus()==2;});
        auto RecoveredCount = count_if(vHuman.begin(), vHuman.end(), [](Human &rHuman) {return rHuman.getInfectionStatus()==3;});
        //myfile << currentTick << "\t" << SusceptibleCount << "\t" << ExposedCount << "\t" << InfectiousCount << "\t" << RecoveredCount << endl;
        long double test = DiseaseEstablishment*(1+ 1e-6 * gsl_sf_cos(1/2*2*M_PI*(currentTick-NumberTicks/2)/NumberTicks));
        myfile << test << endl;
    }
    myfile.close();
    gsl_rng_free(prandomNumberGenerator);
return 0;
    
}
