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
    unsigned int argv_minimumInfectionDuration=0;
    unsigned int argv_maximumInfectionDuration=0;
    double argv_diseaseEstablishment=0; // baseline disease establishment proportion
    double argv_seasonalityCoefficient=0; // strength of seasonality
    double argv_randomMovementShape=0; // 8.18 estimated from Schaber et al. (2019)
    double argv_randomMovementRate=0; // 3.55 estimated from Schaber et al. (2019)
    string argv_outputFile="foo.txt";
    
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
            argv_minimumInfectionDuration = stoi(argv[arg+1]);
            arg++;
            cout << "-minInfectionDuration set to " << argv_minimumInfectionDuration << endl;
        }
        
        if (strcmp(argv[arg], "-maxInfectionDuration") == 0) //minimum duration of infectious period
        {
            if (arg == argc - 1)
            {
                cerr << "missing argument for -maxInfectionDuration!";
                exit(1);
            }
            argv_maximumInfectionDuration = stoi(argv[arg+1]);
            arg++;
            cout << "-maxInfectionDuration set to " << argv_maximumInfectionDuration << endl;
        }
        
        
        if (strcmp(argv[arg], "-alpha") == 0) //baseline disease establishment proportion
        {
            if (arg == argc - 1)
            {
                cerr << "missing argument for -alpha!";
                exit(1);
            }
            argv_diseaseEstablishment = stof(argv[arg+1]);
            arg++;
            cout << "-alpha set to " << argv_diseaseEstablishment << endl;
        }
        
        if (strcmp(argv[arg], "-alphaSeasonality") == 0) //baseline disease establishment proportion
        {
            if (arg == argc - 1)
            {
                cerr << "missing argument for -alphaSeasonality!";
                exit(1);
            }
            argv_seasonalityCoefficient = stof(argv[arg+1]);
            arg++;
            cout << "-alphaSeasonality set to " << argv_seasonalityCoefficient << endl;
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
            argv_outputFile = argv[arg+1];
            arg++;
            cout << "-output set to " << argv_outputFile << endl;
        }
    }
    
    system("pwd");
    
    const long int randomSeed=argv_randomSeed; //variables from argument parser
    const unsigned int minimumInfectionDuration=argv_minimumInfectionDuration;
    const unsigned int maximumInfectionDuration=argv_maximumInfectionDuration;
    const double diseaseEstablishment=argv_diseaseEstablishment;
    const double seasonalityCoefficient=argv_seasonalityCoefficient;
    const double randomMovementShape=argv_randomMovementShape;
    const double randomMovementRate=argv_randomMovementRate;
    const string outputFile=argv_outputFile;
    
    vector<Location> v_Location; //empty vector location
    vector<Human> v_Human; //empty vector humans
    
    int32_t locationCount=1000; //total number of locations
    int numberTicks=365 ; //total number of simulated ticks
    const double humansPerLocationNegBinomProb=0.594; //0.5939354, Reiner et al. (2014)
    const double humansperLocationNegBinomN=9.01; //9.01, Reiner et al. (2014)
    unsigned int exposureDuration=0; //infection parameters
    
    const gsl_rng_type * randomNumberType; //build random number generator (GSL library)
    gsl_rng * randomNumberGenerator;
    gsl_rng_env_setup();
    randomNumberType = gsl_rng_default;
    randomNumberGenerator = gsl_rng_alloc(randomNumberType);
    gsl_rng_set(randomNumberGenerator, randomSeed);
    
    generateLocations(locationCount, &v_Location); //set up locations (use pointer to avoid copying)
    generateHumans(v_Location, &v_Human, humansPerLocationNegBinomProb,humansperLocationNegBinomN,randomNumberGenerator);//set up humans, # of humans per location ~ nbinom

    auto infectionSeed = gsl_rng_uniform_int(randomNumberGenerator, v_Human.size()); //seed infection via an exposed individual
    v_Human[infectionSeed].initiateInfection(exposureDuration);
    
    long double startValueDiseaseEstablishment = diseaseEstablishment*(1+ seasonalityCoefficient * gsl_sf_cos(2*M_PI*(0-numberTicks/2)/numberTicks)); //set disease establishment proportion
    for (auto &rLocation:v_Location) rLocation.initiateRiskScore(startValueDiseaseEstablishment);
    
    ofstream myfile;
    myfile.open(outputFile);
    if(!myfile.is_open()) {
       cerr << "Can not open file";
        return -1;
     }
    
    for (int currentTick=1; currentTick<=numberTicks; currentTick++) {//for each tick
        for (auto &rHuman:v_Human) rHuman.generateMovement(&v_Location, randomNumberGenerator, randomMovementShape, randomMovementRate, exposureDuration); //generate movement
        long double currentDiseaseEstablishment = diseaseEstablishment*(1+ seasonalityCoefficient * gsl_sf_cos(2*M_PI*(currentTick-numberTicks/2)/numberTicks)); //calculate current disease establishment proportion
        for(auto &rLocation:v_Location) rLocation.updateCharacteristics(currentDiseaseEstablishment);//update disease establishment proportion & visits/location
        for(auto &rHuman:v_Human) rHuman.propagateInfection(minimumInfectionDuration,maximumInfectionDuration,randomNumberGenerator); //update infection status
        
        auto SusceptibleCount = count_if(v_Human.begin(), v_Human.end(), [](Human &rHuman) {return rHuman.getInfectionStatus()==InfectionStatus::kSusceptible;}); //generate output
        auto ExposedCount = count_if(v_Human.begin(), v_Human.end(), [](Human &rHuman) {return rHuman.getInfectionStatus()==InfectionStatus::kExposed;});
        auto InfectiousCount = count_if(v_Human.begin(), v_Human.end(), [](Human &rHuman) {return rHuman.getInfectionStatus()==InfectionStatus::kInfected;});
        auto RecoveredCount = count_if(v_Human.begin(), v_Human.end(), [](Human &rHuman) {return rHuman.getInfectionStatus()==InfectionStatus::kRecovered;});
        myfile << currentTick << "\t" << SusceptibleCount << "\t" << ExposedCount << "\t" << InfectiousCount << "\t" << RecoveredCount << endl;
    }
    myfile.close();
    gsl_rng_free(randomNumberGenerator);
return 0;
    
}
