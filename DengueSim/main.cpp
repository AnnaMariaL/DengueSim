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


int main(int argc, const char * argv[]) {
    
    long int argv_randomSeed=42;
    unsigned int argv_minimumInfectionDuration=2;
    unsigned int argv_maximumInfectionDuration=5;
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
        
        if (strcmp(argv[arg], "-maxInfectionDuration") == 0) //minimum duration of infectious period
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
        
        if (strcmp(argv[arg], "-alphaSeasonality") == 0) //baseline disease establishment proportion
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
    
    system("pwd");
    
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
    
    int32_t locationCount=100; //total number of locations
    int numberTicks=6 ; //total number of simulated ticks
    const double humansPerLocationNegBinomProb=0.594; //0.5939354, Reiner et al. (2014)
    const double humansperLocationNegBinomN=9.01; //9.01, Reiner et al. (2014)
    unsigned int exposureDuration=0; //infection parameters
    
    const gsl_rng_type * randomNumberType; //build random number generator (GSL library)
    gsl_rng * rng;
    gsl_rng_env_setup();
    randomNumberType = gsl_rng_default;
    rng = gsl_rng_alloc(randomNumberType);
    gsl_rng_set(rng, randomSeed);
    
    generateLocations(locationCount, &locations); //set up locations (use pointer to avoid copying)
    generateHumans(locations, &humans, humansPerLocationNegBinomProb,humansperLocationNegBinomN,rng,numberTicks);//set up humans, # of humans per location ~ nbinom

    auto infectionSeed = gsl_rng_uniform_int(rng, humans.size()); //seed infection via an exposed individual
    humans[infectionSeed].initiateInfection(exposureDuration);
    
    long double startValueDiseaseEstablishment = diseaseEstablishment*(1+ seasonalityCoefficient * gsl_sf_cos(2*M_PI*(0-numberTicks/2)/numberTicks)); //set disease establishment proportion
    for (auto &location:locations) location.initiateRiskScore(startValueDiseaseEstablishment);
    
    std::ofstream myfile;
    myfile.open(outputFile);
    if(!myfile.is_open()) {
       std::cerr << "Can not open file";
        return -1;
     }
    
    for (int currentTick=1; currentTick<=numberTicks; currentTick++) {//for each tick
        for (auto &human:humans) human.generateMovement(&locations, rng, randomMovementShape, randomMovementRate, exposureDuration); //generate movement
        long double currentDiseaseEstablishment = diseaseEstablishment*(1+ seasonalityCoefficient * gsl_sf_cos(2*M_PI*(currentTick-numberTicks/2)/numberTicks)); //calculate current disease establishment proportion
        for(auto &location:locations) location.updateCharacteristics(currentDiseaseEstablishment);//update disease establishment proportion & visits/location
        for(auto &human:humans) human.propagateInfection(minimumInfectionDuration,maximumInfectionDuration,rng,numberTicks-currentTick); //update infection status
        
        auto SusceptibleCount = count_if(humans.begin(), humans.end(), [](Human &human) {return human.getInfectionStatus()==InfectionStatus::kSusceptible;}); //generate output
        auto ExposedCount = count_if(humans.begin(), humans.end(), [](Human &human) {return human.getInfectionStatus()==InfectionStatus::kExposed;});
        auto InfectiousCount = count_if(humans.begin(), humans.end(), [](Human &human) {return human.getInfectionStatus()==InfectionStatus::kInfected;});
        auto RecoveredCount = count_if(humans.begin(), humans.end(), [](Human &human) {return human.getInfectionStatus()==InfectionStatus::kRecovered;});
        myfile << currentTick << "\t" << SusceptibleCount << "\t" << ExposedCount << "\t" << InfectiousCount << "\t" << RecoveredCount << std::endl;
        for (auto &human:humans) print(std::cout, human) << std::endl;
    }
    myfile.close();
    gsl_rng_free(rng);
return 0;
    
}
