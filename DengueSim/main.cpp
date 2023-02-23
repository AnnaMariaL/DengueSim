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
    
    long int argv_randomSeed=42;
    unsigned int InfectionDuration=200;
    float DiseaseEstablishment=1.6e-6;
    string OutPutFile="output.txt";
    
    for (int arg = 1; arg < argc; ++arg) //parse command line arguments
    {
        
        if (strcmp(argv[arg], "-seed") == 0) //random seed
        {
            if (arg == argc - 1)
            {
                std::cerr << "missing argument for -seed!";
                exit(1);
            }
            
            argv_randomSeed = stol(argv[arg+1]);
            arg++;
            
            std::cout << "-seed set to " << argv_randomSeed << std::endl;
        }
        
        if (strcmp(argv[arg], "-gammaInv") == 0) //avg. infectious period (= 1 / recovery rate)
        {
            if (arg == argc - 1)
            {
                std::cerr << "missing argument for -gammaInv!";
                exit(1);
            }
            
            InfectionDuration = stoi(argv[arg+1]);
            arg++;
            
            std::cout << "-gammaInv set to " << InfectionDuration << std::endl;
        }
        
        if (strcmp(argv[arg], "-alpha") == 0) //disease establishment proportion
        {
            if (arg == argc - 1)
            {
                std::cerr << "missing argument for -alpha!";
                exit(1);
            }
            
            DiseaseEstablishment = stof(argv[arg+1]);
            arg++;
            
            std::cout << "-alpha set to " << DiseaseEstablishment << std::endl;
        }
        
        if (strcmp(argv[arg], "-output") == 0) //output file
        {
            if (arg == argc - 1)
            {
                std::cerr << "missing argument for -output!";
                exit(1);
            }
            
            OutPutFile = argv[arg+1];
            arg++;
            
            std::cout << "-output set to " << OutPutFile << std::endl;
        }
        
        
    }
    system("pwd");
    
    vector<Location> vLocation; //empty vector location
    vector<Human> vHuman; //empty vector humans
    
    int32_t locationCount=1; //total number of locations
    int NumberTicks=5000; //total number of simulated ticks
    const double randomProbability=0.5; //0.5939354, Reiner et al. (2014)
    const double randomSize=10000; //9.01, Reiner et al. (2014)
    const long int randomSeed=argv_randomSeed;
    
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
    
    for (int currentTick=0; currentTick<NumberTicks; currentTick++) {//for each tick
        for(auto &rHuman:vHuman) rHuman.generateMovement(&vLocation,prandomNumberGenerator,0,ExposureDuration,DiseaseEstablishment); //generate movement
        for(auto &rLocation:vLocation) rLocation.updateRiskScore(); //update risk score for each location
        for(auto &rHuman:vHuman) rHuman.propagateInfection(InfectionDuration,prandomNumberGenerator); //update infection status
        //generate output
        auto SusceptibleCount = count_if(vHuman.begin(), vHuman.end(), [](Human &rHuman) {return rHuman.getInfectionStatus()==0;});
        auto ExposedCount = count_if(vHuman.begin(), vHuman.end(), [](Human &rHuman) {return rHuman.getInfectionStatus()==1;});
        auto InfectiousCount = count_if(vHuman.begin(), vHuman.end(), [](Human &rHuman) {return rHuman.getInfectionStatus()==2;});
        auto RecoveredCount = count_if(vHuman.begin(), vHuman.end(), [](Human &rHuman) {return rHuman.getInfectionStatus()==3;});
        myfile << currentTick << "\t" << SusceptibleCount << "\t" << ExposedCount << "\t" << InfectiousCount << "\t" << RecoveredCount << endl;
    }
    myfile.close();

    gsl_rng_free(prandomNumberGenerator);
return 0;
    
}
