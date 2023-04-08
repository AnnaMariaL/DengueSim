//
//  main.cpp
//  DD
//
//  Copyright © 2022 Anna Maria Langmüller. All rights reserved.
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

int main(int argc, const char * argv[]) {
    
    long int argv_randomSeed=42;
    unsigned int argv_minimumInfectionDuration=2; //minimum duration of infectiousness (in number of ticks)
    unsigned int argv_maximumInfectionDuration=5; //maximum duration of infectiousness (in number of ticks)
    double argv_diseaseEstablishment=0.025; // baseline disease establishment proportion
    double argv_seasonalityCoefficient=0; // strength of seasonality
    double argv_randomMovementShape=8.18; // 8.18 estimated from Schaber et al. (2019)
    double argv_randomMovementRate=3.55; // 3.55 estimated from Schaber et al. (2019)
    std::string argv_outputFile="foo.txt";
    
    for (int arg = 1; arg < argc; ++arg) //parse command line arguments
    {
        
        if (strcmp(argv[arg], "-seed") == 0) //random seed
        {
            if (arg == argc - 1)
            {
                std::cerr << "missing argument for -seed!";
                exit(1);
            }
            argv_randomSeed = std::stol(argv[arg+1]);
            arg++;
            std::cout << "-seed set to " << argv_randomSeed << std::endl;
        }
        
        if (strcmp(argv[arg], "-minInfectionDuration") == 0) //minimum duration of infectious period
        {
            if (arg == argc - 1)
            {
                std::cerr << "missing argument for -minInfectionDuration!";
                exit(1);
            }
            argv_minimumInfectionDuration = std::stoi(argv[arg+1]);
            arg++;
            std::cout << "-minInfectionDuration set to " << argv_minimumInfectionDuration << std::endl;
        }
        
        if (strcmp(argv[arg], "-maxInfectionDuration") == 0) //maximum duration of infectious period
        {
            if (arg == argc - 1)
            {
                std::cerr << "missing argument for -maxInfectionDuration!";
                exit(1);
            }
            argv_maximumInfectionDuration = std::stoi(argv[arg+1]);
            arg++;
            std::cout << "-maxInfectionDuration set to " << argv_maximumInfectionDuration << std::endl;
        }
        
        if (strcmp(argv[arg], "-alpha") == 0) //baseline disease establishment proportion
        {
            if (arg == argc - 1)
            {
                std::cerr << "missing argument for -alpha!";
                exit(1);
            }
            argv_diseaseEstablishment = std::stof(argv[arg+1]);
            arg++;
            std::cout << "-alpha set to " << argv_diseaseEstablishment << std::endl;
        }
        
        if (strcmp(argv[arg], "-alphaSeasonality") == 0) //strength of seasonality
        {
            if (arg == argc - 1)
            {
                std::cerr << "missing argument for -alphaSeasonality!";
                exit(1);
            }
            argv_seasonalityCoefficient = std::stof(argv[arg+1]);
            arg++;
            std::cout << "-alphaSeasonality set to " << argv_seasonalityCoefficient << std::endl;
        }
        
        if(strcmp(argv[arg], "-movementShape") == 0) //average number of places visited, human movement (negative binomial distribution)
        {
            if (arg == argc - 1)
            {
                std::cerr << "missing argument for -movementShape!";
                exit(1);
            }
            argv_randomMovementShape = std::stod(argv[arg+1]);
            arg++;
            std::cout << "-movementShape set to " << argv_randomMovementShape << std::endl;
        }
        
        if(strcmp(argv[arg], "-movementRate") == 0) //size parameter, human movement (negative binomial distribution)
        {
            if (arg == argc - 1)
            {
                std::cerr << "missing argument for -movementRate!";
                exit(1);
            }
            argv_randomMovementRate = std::stod(argv[arg+1]);
            arg++;
            std::cout << "-movementRate set to " << argv_randomMovementRate << std::endl;
        }
        
        if (strcmp(argv[arg], "-output") == 0) //output file
        {
            if (arg == argc - 1)
            {
                std::cerr << "missing argument for -output!";
                exit(1);
            }
            argv_outputFile = argv[arg+1];
            arg++;
            std::cout << "-output set to " << argv_outputFile << std::endl;
        }
    }
    
    
    const long int randomSeed=argv_randomSeed; //variables from argument parser
    const unsigned int minimumInfectionDuration=argv_minimumInfectionDuration;
    const unsigned int maximumInfectionDuration=argv_maximumInfectionDuration;
    const double diseaseEstablishment=argv_diseaseEstablishment;
    const double seasonalityCoefficient=argv_seasonalityCoefficient;
    const double randomMovementShape=argv_randomMovementShape;
    const double randomMovementRate=argv_randomMovementRate;
    const std::string outputFile=argv_outputFile;
    
    std::vector<Location> locations; //empty vector location
    std::vector<Human> humans; //empty vector humans
    
    int32_t locationCount=10000; //total number of locations
    unsigned int numberTicks=365 ; //total number of simulated ticks
    size_t historyLength = 10; //deque size for infectedVisitCount and riskScore
    const double humansPerLocationNegBinomProb=0.594; //0.5939354, Reiner et al. (2014)
    const double humansperLocationNegBinomN=9.01; //9.01, Reiner et al. (2014)
    unsigned int exposureDuration=0; //infection parameters
    long double startValueDiseaseEstablishment = diseaseEstablishment*(1+ seasonalityCoefficient * gsl_sf_cos(2*M_PI*(0-numberTicks/2)/numberTicks)); //set disease establishment proportion
    
    const gsl_rng_type * randomNumberType; //build random number generator (GSL library)
    gsl_rng * rng;
    gsl_rng_env_setup();
    randomNumberType = gsl_rng_default;
    rng = gsl_rng_alloc(randomNumberType);
    gsl_rng_set(rng, randomSeed);
    
    std::ofstream myfile;
    myfile.open(outputFile);
    if(!myfile.is_open()) {
       std::cerr << "Can not open file";
        return -1;
     }
    
    system("pwd");
    
    generateLocations(locationCount, &locations, startValueDiseaseEstablishment, historyLength); //set up locations (use pointer to avoid copying)
    generateHumans(locations, &humans, humansPerLocationNegBinomProb, humansperLocationNegBinomN, numberTicks, rng); //set up humans, # of humans per location ~ nbinom
    
    auto infectionSeed = gsl_rng_uniform_int(rng, humans.size()); //seed infection via an exposed individual
    humans[infectionSeed].initiateInfection(exposureDuration);
    
    for (unsigned int currentTick=1; currentTick<=numberTicks; currentTick++) {//for each tick
        for (auto &human:humans) human.generateMovement(&locations, randomMovementShape, randomMovementRate, exposureDuration, rng); //generate movement
        long double currentDiseaseEstablishment = diseaseEstablishment*(1+ seasonalityCoefficient * gsl_sf_cos(2*M_PI*(currentTick-numberTicks/2)/numberTicks)); //calculate current disease establishment proportion
        for(auto &location:locations) location.storeRiskScoreAndNumberOfInfectedVisitors(currentDiseaseEstablishment);//update disease establishment proportion & number of infected visits per location
        for(auto &human:humans) human.propagateInfection(minimumInfectionDuration, maximumInfectionDuration, numberTicks-currentTick, rng); //update infection status
        
        auto susceptibleCount = count_if(humans.begin(), humans.end(), [](Human &human) {return human.getInfectionStatus()==InfectionStatus::kSusceptible;}); //generate output
        auto exposedCount = count_if(humans.begin(), humans.end(), [](Human &human) {return human.getInfectionStatus()==InfectionStatus::kExposed;});
        auto infectedCount = count_if(humans.begin(), humans.end(), [](Human &human) {return human.getInfectionStatus()==InfectionStatus::kInfected;});
        auto recoveredCount = count_if(humans.begin(), humans.end(), [](Human &human) {return human.getInfectionStatus()==InfectionStatus::kRecovered;});
        myfile << currentTick << "\t" << susceptibleCount << "\t" << exposedCount << "\t" << infectedCount << "\t" << recoveredCount << std::endl;
    }
    myfile.close();
    gsl_rng_free(rng);
return 0;
}
