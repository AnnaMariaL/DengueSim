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
    double argv_avgNumberVisits = 4.00; //average number of places visited, human movement (negative binomial distribution)
    double argv_pNumberVisits = 0.5; //success probability for places visited, human movement (negative binomial distribution)
    double argv_proportionSocialVisits = 0.5; //proportion of visits that occur inside the social Group
    double argv_locationsPerSocialGroup = 4.5; //average number of locations per social group
    int32_t argv_locationCountToAdd = 1000; //total number of locations
    std::string argv_releaseScenario = "H1"; //inital seeding scenario of the disease: H1; Hn; L1
    std::string argv_outputFile = "foo.txt"; //output file for S-E-I-R counts per tick
    std::string argv_verbose = "true"; //if true, print parameters to std::cout
    
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
        }
        
        if(strcmp(argv[arg], "-avgNumberVisits") == 0) //average number of places visited, human movement (negative binomial distribution)
        {
            if (arg == argc - 1)
            {
                std::cerr << "missing argument for -avgNumberVisits!";
                exit(1);
            }
            argv_avgNumberVisits = std::stod(argv[arg+1]);
            arg++;
        }
        
        if(strcmp(argv[arg], "-pNumberVisits") == 0) //success probability for places visited, human movement (negative binomial distribution)
        {
            if (arg == argc - 1)
            {
                std::cerr << "missing argument for -pNumberVisits!";
                exit(1);
            }
            argv_pNumberVisits = std::stod(argv[arg+1]);
            arg++;
        }
        
        if(strcmp(argv[arg], "-proportionSocialVisits") == 0) //proportion of visits that occur inside the social Group
        {
            if (arg == argc - 1)
            {
                std::cerr << "missing argument for -proportionSocialVisits!";
                exit(1);
            }
            argv_proportionSocialVisits = std::stod(argv[arg+1]);
            arg++;
        }
        
        if(strcmp(argv[arg], "-locationsPerSocialGroup") == 0) //average number of locations per social Group
        {
            if (arg == argc - 1)
            {
                std::cerr << "missing argument for -locationsPerSocialGroup!";
                exit(1);
            }
            argv_locationsPerSocialGroup = std::stod(argv[arg+1]);
            arg++;
        }
        
        if(strcmp(argv[arg], "-locationCount") == 0) //average number of locations per social Group
        {
            if (arg == argc - 1)
            {
                std::cerr << "missing argument for -locationCount!";
                exit(1);
            }
            argv_locationCountToAdd = std::stoi(argv[arg+1]);
            arg++;
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
        }
        
        if (strcmp(argv[arg], "-releaseScenario") == 0) //initial release scenario of the disease
        { // H1 = 1 random human; Hn = n random humans (sampled from NBD); L1 = 1 random location (all inhabitants become exposed)
            if (arg == argc - 1)
            {
                std::cerr << "missing argument for -releaseScenario!";
                exit(1);
            }
            argv_releaseScenario = argv[arg+1];
            arg++;
        }
        
        if (strcmp(argv[arg], "-verbose") == 0) //if true, prints set parameters to std::cout
        {
            if (arg == argc - 1)
            {
                std::cerr << "missing argument for -verbose!";
                exit(1);
            }
            argv_verbose = argv[arg+1];
            arg++;
        }
        
    }
    
    const long int randomSeed = argv_randomSeed; //variables from argument parser
    const double infectionDuration = argv_infectionDuration;
    const double diseaseEstablishment = argv_diseaseEstablishment;
    const double seasonalityCoefficient = argv_seasonalityCoefficient;
    const double seasonalityPhaseshift = argv_seasonalityPhaseShift;
    const double avgNumberVisits = argv_avgNumberVisits;
    const double pNumberVisits = argv_pNumberVisits;
    const double proportionSocialVisits = argv_proportionSocialVisits;
    const double locationsPerSocialGroup = argv_locationsPerSocialGroup;
    const int32_t locationCountToAdd = argv_locationCountToAdd;
    const std::string releaseScenario = argv_releaseScenario;
    const std::string outputFile = argv_outputFile;
    const std::string verbose = argv_verbose;
    
    if (verbose == "true") { //if set to true, print parameters to std::cout
        std::cout << "-seed: " << randomSeed << std::endl;
        std::cout << "-alpha: " << diseaseEstablishment << std::endl;
        std::cout << "-alphaSeasonality: " << argv_seasonalityCoefficient << std::endl;
        std::cout << "-seasonalityPhaseShift: " << argv_seasonalityPhaseShift << std::endl;
        std::cout << "-infectionDuration: " << infectionDuration << std::endl;
        std::cout << "-avgNumberVisits: " << avgNumberVisits << std::endl;
        std::cout << "-pNumberVisits: " << pNumberVisits << std::endl;
        std::cout << "-proportionSocialVisits: " << proportionSocialVisits << std::endl;
        std::cout << "-locationsPerSocialGroup: " << locationsPerSocialGroup << std::endl;
        std::cout << "-locationCount: " << locationCountToAdd << std::endl;
        std::cout << "-releaseScenario: " << releaseScenario << std::endl;
        std::cout << "-output: " << outputFile << std::endl;
        system("pwd");
    }
    
    std::vector<Location> locations; //empty vector location
    std::vector<SocialGroup> socialGroups; //empty vector socialGroups
    std::vector<Human> humans; //empty vector humans
    
    const unsigned int numberTicks = 3650 ; //maximum number of simulated ticks
    const size_t numberTicksToTrackInDeque = 10; //deque size for infectedVisitsCountsHistory_ and riskScoreHistory_ (Location classs members)
    const double humansPerLocationNegBinomProb = 0.5939354; //0.5939354, Reiner et al. (2014)
    const double humansPerLocationNegBinomN = 9.01 ; //9.01, Reiner et al. (2014)
    const unsigned int exposureDuration = 0; //infection parameters
    const double omega = 1; // 1/wavelength
    const double startValueDiseaseEstablishment = diseaseEstablishment * (1 + seasonalityCoefficient * std::cos(omega * 2 * M_PI*(0 - seasonalityPhaseshift))); //set disease Establishment proportion
    
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
    
    
    generateLocations(locationCountToAdd, &locations, startValueDiseaseEstablishment, numberTicksToTrackInDeque); //set up locations (use pointer to avoid copying)
    generateHumans(locations, &humans, humansPerLocationNegBinomProb, humansPerLocationNegBinomN, rng, locationsPerSocialGroup, &socialGroups); //set up humans, # of humans per location ~ nbinom
    //social groups - a property of humans - are set up inside generateHumans()
    
    size_t locationCount = 0; //check social Group assignment
    for (size_t id = 0; id < socialGroups.size(); id++)
    {
        SocialGroup &socialGroup = (socialGroups)[id];
        locationCount += socialGroup.Locations().size();
    }
    
    if (locationCount != locations.size()) {
        std::cerr << "Error in assignment of social groups. Not all locations are uniquely assigned." << std::endl;
        exit(1);
    }
    
    //initial release of disease
    
    if ( releaseScenario == "H1") { //seed infection in single individual
        
        const auto infectionSeed = gsl_rng_uniform_int(rng, humans.size());
        humans[infectionSeed].initiateInfection(exposureDuration);
        
    } else {
        if ( releaseScenario == "Hn" ) { //seed infection in multiple, randomly selected individuals
            
            std::vector<size_t> initialInfections; //book-keeping of already sampled humans
            int32_t exposedCounter = 0; //counter
            int32_t initialExposedCount = 0;
            
            do
                initialExposedCount = gsl_ran_negative_binomial(rng, humansPerLocationNegBinomProb, humansPerLocationNegBinomN);
            while (initialExposedCount < 1);
            
            while (exposedCounter < initialExposedCount) {
                const auto infectionSeed = gsl_rng_uniform_int(rng, humans.size());
                bool alreadySampled = (find(initialInfections.begin(), initialInfections.end(), infectionSeed) != initialInfections.end());
                if (alreadySampled) //resample if human is already exposed
                    continue;
                humans[infectionSeed].initiateInfection(exposureDuration); //initiate infection
                initialInfections.push_back(infectionSeed);
                exposedCounter++; //increase counter
            }
            
        } else {
            if ( releaseScenario == "L1" ) { //seed infection in all inhabitants of one randomly selected location
                
                const auto infectionSeed = gsl_rng_uniform_int(rng, locations.size());
                
                for (auto &human : humans) {
                    if ( human.getHomeLocation().getLocationID() == locations[infectionSeed].getLocationID() )
                        human.initiateInfection(exposureDuration);
                }
            } else {
                std::cerr << "Unknown release scenario. Select either \'H1\', \'Hn\', or \'L1\'." << std::endl;
                exit(1);
            }
        }
    }
    
    
    for (unsigned int currentTick=1; currentTick <= numberTicks; currentTick++)
    {//for each tick
        const double currentDiseaseEstablishment = diseaseEstablishment * (1 + seasonalityCoefficient * std::cos(omega * 2 * M_PI * ((double)currentTick/365-seasonalityPhaseshift))); //calculate current disease establishment proportion
        for (auto &human : humans)
            human.generateMovement(&locations, avgNumberVisits, pNumberVisits, exposureDuration, proportionSocialVisits, rng); //generate movement
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
