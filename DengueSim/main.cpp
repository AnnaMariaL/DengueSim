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
    unsigned int argv_MinimumInfectionDuration=0;
    unsigned int argv_MaximumInfectionDuration=0;
    double argv_DiseaseEstablishment=0; // baseline disease establishment proportion
    double argv_SeasonalityCoefficient=0; // strength of seasonality
    //double argv_randomMovementMu = 0; // 8.55 over 15-days period, Stoddard et al. (2013)
    //double argv_randomMovementTheta = 0; // 11.94 over 15-days period, Stoddard et al. (2013)
    double argv_randomMovementShape=0; // 8.18 estimated from Schaber et al. (2019)
    double argv_randomMovementRate=0; // 3.55 estimated from Schaber et al. (2019)
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
        
        if (strcmp(argv[arg], "-minInfectionDuration") == 0) //minimum duration of infectious period
        {
            if (arg == argc - 1)
            {
                cerr << "missing argument for -minInfectionDuration!";
                exit(1);
            }
            argv_MinimumInfectionDuration = stoi(argv[arg+1]);
            arg++;
            cout << "-minInfectionDuration set to " << argv_MinimumInfectionDuration << endl;
        }
        
        if (strcmp(argv[arg], "-maxInfectionDuration") == 0) //minimum duration of infectious period
        {
            if (arg == argc - 1)
            {
                cerr << "missing argument for -maxInfectionDuration!";
                exit(1);
            }
            argv_MaximumInfectionDuration = stoi(argv[arg+1]);
            arg++;
            cout << "-maxInfectionDuration set to " << argv_MaximumInfectionDuration << endl;
        }
        
        
        if (strcmp(argv[arg], "-alpha") == 0) //baseline disease establishment proportion
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
        
        if (strcmp(argv[arg], "-alphaSeasonality") == 0) //baseline disease establishment proportion
        {
            if (arg == argc - 1)
            {
                cerr << "missing argument for -alphaSeasonality!";
                exit(1);
            }
            argv_SeasonalityCoefficient = stof(argv[arg+1]);
            arg++;
            cout << "-alphaSeasonality set to " << argv_SeasonalityCoefficient << endl;
        }
        
        if(strcmp(argv[arg], "-movementShape") == 0) //average number of places visited, human movement (negative binomial distribution)
        {
            if (arg == argc - 1)
            {
                cerr << "missing argument for -movementShape!";
                exit(1);
            }
            argv_randomMovementShape = stod(argv[arg+1]);
            arg++;
            cout << "-movementShape set to " << argv_randomMovementShape << endl;
        }
        
        if(strcmp(argv[arg], "-movementRate") == 0) //size parameter, human movement (negative binomial distribution)
        {
            if (arg == argc - 1)
            {
                cerr << "missing argument for -movementRate!";
                exit(1);
            }
            argv_randomMovementRate = stod(argv[arg+1]);
            arg++;
            cout << "-movementRate set to " << argv_randomMovementRate << endl;
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
    const unsigned int MinimumInfectionDuration=argv_MinimumInfectionDuration;
    const unsigned int MaximumInfectionDuration=argv_MaximumInfectionDuration;
    const double DiseaseEstablishment=argv_DiseaseEstablishment;
    const double SeasonalityCoefficient=argv_SeasonalityCoefficient;
    const double randomMovementShape=argv_randomMovementShape;
    const double randomMovementRate=argv_randomMovementRate;
    const string OutPutFile=argv_OutPutFile;
    
    vector<Location> vLocation; //empty vector location
    vector<Human> vHuman; //empty vector humans
    
    int32_t locationCount=1000; //total number of locations
    int NumberTicks=365 ; //total number of simulated ticks
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
        for (auto &rHuman:vHuman) rHuman.generateMovement(&vLocation, prandomNumberGenerator, randomMovementShape, randomMovementRate, ExposureDuration); //generate movement
        long double currentDiseaseEstablishment = DiseaseEstablishment*(1+ SeasonalityCoefficient * gsl_sf_cos(2*M_PI*(currentTick-NumberTicks/2)/NumberTicks)); //calculate current disease establishment proportion
        for(auto &rLocation:vLocation) rLocation.updateCharacteristics(currentDiseaseEstablishment); //update disease establishment proportion & visits/location
        for(auto &rHuman:vHuman) rHuman.propagateInfection(MinimumInfectionDuration,MaximumInfectionDuration,prandomNumberGenerator); //update infection status
        auto SusceptibleCount = count_if(vHuman.begin(), vHuman.end(), [](Human &rHuman) {return rHuman.getInfectionStatus()==0;}); //generate output
        auto ExposedCount = count_if(vHuman.begin(), vHuman.end(), [](Human &rHuman) {return rHuman.getInfectionStatus()==1;});
        auto InfectiousCount = count_if(vHuman.begin(), vHuman.end(), [](Human &rHuman) {return rHuman.getInfectionStatus()==2;});
        auto RecoveredCount = count_if(vHuman.begin(), vHuman.end(), [](Human &rHuman) {return rHuman.getInfectionStatus()==3;});
        myfile << currentTick << "\t" << SusceptibleCount << "\t" << ExposedCount << "\t" << InfectiousCount << "\t" << RecoveredCount << endl;
    }
    myfile.close();
    gsl_rng_free(prandomNumberGenerator);
return 0;
    
}
