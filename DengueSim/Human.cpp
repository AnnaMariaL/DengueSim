//
//  Human.cpp
//  DD
//
//  Copyright © 2022 Anna Maria Langmüller. All rights reserved.
//

#include <stdio.h>
#include <algorithm>
#include "Human.h"
#include "Location.h"

void generateHumans(std::vector<Location> &p_locations, std::vector<Human> *p_humans, const int32_t p_humanCountToAdd)
{
    //reference to location (will not be changed), pointer to humans (avoid copying)
    HumanID humanID = 0; //initialize human ID
    for (auto &location : p_locations) //reference to individual location
    {
        for (int32_t i = 0; i < p_humanCountToAdd; i++)
        {
            p_humans->emplace_back(humanID, location, InfectionStatus::kSusceptible); //add single human
            humanID++; //increment human ID
        }
    }
}

void generateHumans(std::vector<Location> &p_locations, std::vector<Human> *p_humans, const double p_humansPerLocationNegBinomProb, const double p_humansPerLocationNegBinomN, gsl_rng *p_rng)
{
    HumanID humanID = 0; //initialize human ID
    int32_t numberOfInhabitantsPerLocation;
    for (auto &location : p_locations) //reference to individual location
    {
        numberOfInhabitantsPerLocation = gsl_ran_negative_binomial(p_rng, p_humansPerLocationNegBinomProb, p_humansPerLocationNegBinomN);
        for (int32_t i = 0; i < numberOfInhabitantsPerLocation; i++)
        {
            p_humans->emplace_back(humanID, location, InfectionStatus::kSusceptible); //add single human
            humanID++; //increment human ID
        }
    }
}

void Human::initiateInfection(const unsigned int p_exposureDuration)
{
    infectionStatus_= InfectionStatus::kExposed;
    remainingTicks_ = p_exposureDuration;
    elapsedTicks_ = 0;
}

void Human::generateMovement(std::vector<Location> *p_locations, const double p_randomMovementShape, const double p_randomMovementRate, const unsigned int p_exposureDuration, gsl_rng *p_rng)
{
    double randomDrawNumberOfLocations = gsl_ran_gamma(p_rng, p_randomMovementShape, 1 / p_randomMovementRate);
    if (randomDrawNumberOfLocations <= 0)
        randomDrawNumberOfLocations = 0;
    const size_t numberOfLocationVisited = (size_t)round(randomDrawNumberOfLocations);
    
    std::vector<LocationID> locationsVisited;
    size_t visitIndex = 1;
    size_t numberTrials = 0; //to avoid endless loop if only few locations are present
    
    locationsVisited.emplace_back(getHomeLocation().getLocationID()); //add home location to places visited
    
    while (visitIndex < numberOfLocationVisited)
    {
        numberTrials++;
        
        if (numberTrials > 100 * numberOfLocationVisited)
        {
            std::cerr << "Warning: It took more than "  << numberTrials << " draws to attempt visiting " << numberOfLocationVisited << " locations. ";
            std::cerr << "Sampling terminated for human " << id_ << ". Already sampled places will be visited." << std::endl;
            break;
        }
        if (numberOfLocationVisited >= p_locations->size())
        {
            std::cerr << "Warning: Human " << id_ << " is scheduled to visit " << numberOfLocationVisited << " locations. Only " << p_locations->size() << " available. ";
            std::cerr << "Sampling terminated." << std::endl;
            break;
        }
        
        const auto locationIndex = gsl_rng_uniform_int(p_rng, p_locations->size());
        const auto alreadySampled = find(locationsVisited.begin(), locationsVisited.end(), locationIndex);
        
        if (alreadySampled == end(locationsVisited)) {
            locationsVisited.emplace_back(locationIndex);
            visitIndex++;
        }
    }
    
    for (size_t i = 0; i < locationsVisited.size(); i++)
    {
        const LocationID locationIndex = locationsVisited[i];
        (*p_locations)[locationIndex].registerInfectiousVisits(*this); //register infectious visit
        
        if (infectionStatus_ == InfectionStatus::kSusceptible) //register potential exposure of susceptibles
        {
            const int32_t infectiousVisits = (*p_locations)[locationIndex].infectedVisitsCountNTicksAgo(0);
            const double diseaseEstablishment = (*p_locations)[locationIndex].riskScoreNTicksAgo(0);
            
            for (int j = 0; j < infectiousVisits; j++)
            {
                const double infectionProbability = gsl_ran_flat(p_rng, 0 , 1);
                if (infectionProbability < diseaseEstablishment) initiateInfection(p_exposureDuration);
            }
        }
    }
}

void Human::propagateInfection(const unsigned int p_minimumInfectionDuration, const unsigned int p_maximumInfectionDuration, gsl_rng *p_rng)
{
    switch (infectionStatus_)
    {
        case InfectionStatus::kSusceptible:
            elapsedTicks_++;
            break;
        case InfectionStatus::kExposed:
            if (remainingTicks_ == 0) //exposed --> infected if exposed period has passed
            {
                auto infectionDuration = gsl_rng_uniform_int(p_rng, (p_maximumInfectionDuration-p_minimumInfectionDuration+1));
                infectionDuration = infectionDuration + p_minimumInfectionDuration;
                
                infectionStatus_ = InfectionStatus::kInfected;
                elapsedTicks_ = 0;
                remainingTicks_ = (unsigned int)infectionDuration;
            }
            else
            { //decrement exposed period
                elapsedTicks_++;
                remainingTicks_--;
            }
            break;
        case InfectionStatus::kInfected:
            if (remainingTicks_ == 0) //infected --> recovered if infectious period has passed
            {
                infectionStatus_ = InfectionStatus::kRecovered;
                elapsedTicks_ = 0;
            }
            else
            {
                elapsedTicks_++;
                remainingTicks_--;
            }
            break;
        case InfectionStatus::kRecovered:
            elapsedTicks_++;
            break;
    }
}

unsigned int Human::getRemainingTicksInStatus(void) const {
    if ((infectionStatus_ == InfectionStatus::kSusceptible) || (infectionStatus_ == InfectionStatus::kRecovered))
    {
        std::cerr << "getRemainingTicksInStatus() called in non-informative state" << std::endl;
        throw std::domain_error("getRemainingTicksInStatus() called in non-informative state");
    }
    return remainingTicks_;
}

std::ostream &print(std::ostream &p_os, const InfectionStatus p_status)
{
    switch (p_status)
    {
        case InfectionStatus::kSusceptible: p_os << "S"; break;
        case InfectionStatus::kExposed: p_os << "E"; break;
        case InfectionStatus::kInfected: p_os << "I"; break;
        case InfectionStatus::kRecovered: p_os << "R"; break;
    }
    return p_os;
}

std::ostream &print(std::ostream &p_os, const Human &p_human)
{
    p_os << p_human.id_ << " ";
    p_os << p_human.homeLocation_.getLocationID() << " ";
    print(p_os, p_human.infectionStatus_) << " ";
    p_os << p_human.elapsedTicks_ << " ";
    p_os << p_human.remainingTicks_ ;
    return p_os;
}
