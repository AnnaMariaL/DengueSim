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
#include "SocialGroup.h"

int main(int argc, const char * argv[])
{
    long int argv_randomSeed = 42;
    double argv_infectionDuration = 2.55; //average duration of infectiousness (in number of ticks)
    double argv_diseaseEstablishment = 0.025; // baseline disease establishment proportion
    double argv_seasonalityCoefficient = 0; // strength of seasonality
    double argv_seasonalityPhaseShift = 0; //phase shift for seasonality
    double argv_avgNumberVisits = 4.00;
    double argv_varNumberVisits = 1.00;
    std::string argv_outputFile = "foo.txt";
    
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
        
        if (strcmp(argv[arg], "-infectionDuration") == 0) //minimum duration of infectious period
        {
            if (arg == argc - 1)
            {
                std::cerr << "missing argument for -infectionDuration!";
                exit(1);
            }
            argv_infectionDuration = std::stof(argv[arg+1]);
            arg++;
            std::cout << "-infectionDuration set to " << argv_infectionDuration << std::endl;
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
        
        if (strcmp(argv[arg], "-seasonalityPhaseShift") == 0) //phase shift for seasonlity
        {
            if (arg == argc - 1)
            {
                std::cerr << "missing argument for -seasonalityPhaseShift!";
                exit(1);
            }
            argv_seasonalityPhaseShift = std::stof(argv[arg+1]);
            arg++;
            std::cout << "-seasonalityPhaseShift set to " << argv_seasonalityPhaseShift << std::endl;
        }
        
        if(strcmp(argv[arg], "-avgNumberVisits") == 0) //average number of places visited, human movement (negative binomial distribution)
        {
            if (arg == argc - 1)
            {
                std::cerr << "missing argument for -movementShape!";
                exit(1);
            }
            argv_avgNumberVisits = std::stod(argv[arg+1]);
            arg++;
            std::cout << "-avgNumberVisits set to " << argv_avgNumberVisits << std::endl;
        }
        
        if(strcmp(argv[arg], "-varNumberVisits") == 0) //size parameter, human movement (negative binomial distribution)
        {
            if (arg == argc - 1)
            {
                std::cerr << "missing argument for -movementRate!";
                exit(1);
            }
            argv_varNumberVisits = std::stod(argv[arg+1]);
            arg++;
            std::cout << "-varNumberVisits set to " << argv_varNumberVisits << std::endl;
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
    
    const long int randomSeed = argv_randomSeed; //variables from argument parser
    const double infectionDuration = argv_infectionDuration;
    const double diseaseEstablishment = argv_diseaseEstablishment;
    const double seasonalityCoefficient = argv_seasonalityCoefficient;
    const double seasonalityPhaseshift = argv_seasonalityPhaseShift;
    const double avgNumberVisits = argv_avgNumberVisits;
    const double varNumberVisits = argv_varNumberVisits;
    const std::string outputFile = argv_outputFile;
    
    std::vector<Location> locations; //empty vector location
    std::vector<SocialGroup> socialGroups; //empty vector socialGroups
    std::vector<Human> humans; //empty vector humans
    
    const int32_t locationCountToAdd = 10000; //total number of locations
    const unsigned int numberTicks = 365 * 10 ; //maximum number of simulated ticks
    const size_t numberTicksToTrackInDeque = 10; //deque size for infectedVisitCount and riskScore
    const double humansPerLocationNegBinomProb = 0.5; //0.5939354, Reiner et al. (2014)
    const double humansPerLocationNegBinomN = 9; //9.01, Reiner et al. (2014)
    const unsigned int exposureDuration = 0; //infection parameters
    const double omega = 1; // 1/wavelength
    const unsigned int locationsPerSocialGroup = 5;
    const double proportionSocialVisits = 0; //proportion of visits that should occurr inside the social group
    const double startValueDiseaseEstablishment = diseaseEstablishment + seasonalityCoefficient * std::cos(omega * 2 * M_PI*(0 - seasonalityPhaseshift) / 365); //set disease Establishment proportion
    
    const gsl_rng_type *randomNumberType; //build random number generator (GSL library)
    gsl_rng *rng;
    gsl_rng_env_setup();
    randomNumberType = gsl_rng_default;
    rng = gsl_rng_alloc(randomNumberType);
    gsl_rng_set(rng, randomSeed);
    
    std::ofstream myfile;
    myfile.open(outputFile);
    if (!myfile.is_open())
    {
       std::cerr << "Can not open file";
        return -1;
     }
    
    system("pwd");
    
    generateLocations(locationCountToAdd, &locations, startValueDiseaseEstablishment, numberTicksToTrackInDeque); //set up locations (use pointer to avoid copying)
    generateHumans(locations, &humans, humansPerLocationNegBinomProb, humansPerLocationNegBinomN, rng, locationsPerSocialGroup, &socialGroups); //set up humans, # of humans per location ~ nbinom
    //social groups - a property of humans - are set up inside generateHumans()
    
    const auto infectionSeed = gsl_rng_uniform_int(rng, humans.size()); //seed infection via an exposed individual
    humans[infectionSeed].initiateInfection(exposureDuration);
    for (unsigned int currentTick=1; currentTick <= numberTicks; currentTick++)
    {//for each tick
        std::cout << currentTick << std::endl;
        const double currentDiseaseEstablishment = diseaseEstablishment +  seasonalityCoefficient * std::cos(omega * 2 * M_PI * (currentTick-seasonalityPhaseshift)/365); //calculate current disease establishment proportion
        
        for (auto &human : humans)
            human.generateMovement(&locations, avgNumberVisits, varNumberVisits, exposureDuration, proportionSocialVisits, rng); //generate movement
        for (auto &location : locations)
            location.storeRiskScoreAndNumberOfInfectedVisitors(currentDiseaseEstablishment);//update disease establishment proportion & number of infected visits per location
        for (auto &human : humans)
            human.propagateInfection(infectionDuration, rng); //update infection status
        
        auto susceptibleCount = count_if(humans.begin(), humans.end(), [](Human &human) {return human.getInfectionStatus() == InfectionStatus::kSusceptible;}); //generate output
        auto exposedCount = count_if(humans.begin(), humans.end(), [](Human &human) {return human.getInfectionStatus() == InfectionStatus::kExposed;});
        auto infectedCount = count_if(humans.begin(), humans.end(), [](Human &human) {return human.getInfectionStatus() == InfectionStatus::kInfected;});
        auto recoveredCount = count_if(humans.begin(), humans.end(), [](Human &human) {return human.getInfectionStatus() == InfectionStatus::kRecovered;});
        myfile << currentTick << "\t" << susceptibleCount << "\t" << exposedCount << "\t" << infectedCount << "\t" << recoveredCount << std::endl;
        
       if (infectedCount == 0) { //stop simulation if no infectious individuals left
            break;
       }
    }
    
    myfile.close();
    gsl_rng_free(rng);
    return 0;
}
