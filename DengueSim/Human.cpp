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
#include "SocialGroup.h"

void generateHumans(std::vector<Location> &p_locations, std::vector<Human> *p_humans, const double p_humansPerLocationNegBinomProb, const double p_humansPerLocationNegBinomN, gsl_rng *p_rng, unsigned int p_locationsPerSocialGroup, std::vector<SocialGroup> *p_socialGroups)
{
    HumanID humanID = 0; //initialize human ID
    int32_t socialGroupCount = (int32_t)std::ceil((double)p_locations.size() / p_locationsPerSocialGroup); //determine social group count
    int32_t numberOfInhabitantsPerLocation;
    p_socialGroups->reserve(socialGroupCount);      // allocate the right size up front, so it doesn't move!
    for (SocialGroupID group_id = 0; group_id < socialGroupCount; group_id++)
    {
        p_socialGroups->emplace_back(group_id); //add new social group to vector p_socialGroups
        SocialGroup &socialGroup = p_socialGroups->back(); //retrieve current social group
        
        //std::cout << "new sociaslgroup with id " << group_id << ": " << socialGroup.getID() << " (" << (&socialGroup) << ")" << std::endl;
        
        for (int32_t j = 0; j < p_locationsPerSocialGroup; j++) //add p_locationsPerSocialGroup to current social group
        {
            size_t location_index = group_id * p_locationsPerSocialGroup + j;
            
            if (location_index < p_locations.size()) //avoid index out of bounds
            {
                Location &location = p_locations[location_index];
                
                socialGroup.AddLocation(location);
                
                do {
                    numberOfInhabitantsPerLocation = gsl_ran_negative_binomial(p_rng, p_humansPerLocationNegBinomProb, p_humansPerLocationNegBinomN);
                }
                while (numberOfInhabitantsPerLocation <= 0); //at least one inhabitant per location
                
                for (int32_t i = 0; i < numberOfInhabitantsPerLocation; i++)
                {
                    p_humans->emplace_back(humanID, location, socialGroup, InfectionStatus::kSusceptible); //add single human
                    humanID++; //increment human ID
                }
            }
        }
    }
    
    // DEBUG
    //for (SocialGroupID group_id = 0; group_id < socialGroupCount; group_id++)
    //{
      //  SocialGroup &socialGroup = (*p_socialGroups)[group_id];
       // std::cout << "post-init: " << socialGroup.getID() << " (" << &socialGroup << ")" << std::endl;
   // }
}

void Human::initiateInfection(const unsigned int p_exposureDuration)
{
    infectionStatus_= InfectionStatus::kExposed;
    remainingTicks_ = p_exposureDuration;
    elapsedTicks_ = 0;
}

void Human::generateMovement(std::vector<Location> *p_locations, const double p_avgNumberVisits, const double p_varNumberVisits, const unsigned int p_exposureDuration, const double p_proportionSocialVisits, gsl_rng *p_rng)
{
    double randomMovementRate = p_avgNumberVisits/p_varNumberVisits;
    double randomMovementShape = p_avgNumberVisits * p_avgNumberVisits / p_varNumberVisits;
    double randomDrawNumberOfLocations = gsl_ran_gamma(p_rng, randomMovementShape, 1 / randomMovementRate);
    const size_t numberOfLocationsVisited = (size_t)std::round(randomDrawNumberOfLocations);
    std::vector<Location *> socialGroupLocations = getSocialGroup().Locations(); //social group = a vector of pointers to location objects

    if (numberOfLocationsVisited >= p_locations->size()) { //avoid endless loops if too few locations are present
        std::cerr << "Too many locations for location buffer. Sampling terminated." << std::endl;
        exit(1);
    }
    if ((numberOfLocationsVisited >= socialGroupLocations.size()) && (p_proportionSocialVisits >= 1.0)) { //avoid endless loop if too few locations in social group
        std::cerr << "Too many locations for social group. Sampling terminated." << std::endl;
        exit(1);
    }
    
    std::vector<Location *> whereWillIGo; //whereWillIGo = vector of pointers to location objects
    whereWillIGo.emplace_back(&getHomeLocation()); //always visit home; getHomeLocation() returns a reference, but we need the address of the home Location instance --> &getHomeLocation()
    
    size_t placesVisited = 1;
    
    while (placesVisited < numberOfLocationsVisited) {
        size_t trialCount = 0;
        const double isSocialGroupVisit = gsl_rng_uniform(p_rng); //visit inside/outside of social group
        
        do {
            if (++trialCount >= 100 * numberOfLocationsVisited) { //avoid endless loops
                std::cerr << "Too many trials. Sampling terminated" << std::endl;
                exit(1);
            }
            
            if (isSocialGroupVisit < p_proportionSocialVisits) { //social visit
                const auto sampledLocationIndex = gsl_rng_uniform_int(p_rng, socialGroupLocations.size());
                Location *sampledLocation = socialGroupLocations[sampledLocationIndex]; //store sampled pointer to location instance
                bool alreadySampled = (find(whereWillIGo.begin(), whereWillIGo.end(), sampledLocation) != whereWillIGo.end());
                if (alreadySampled) //if you visit sampledLocation already, sample again
                    continue; //continue = skip the remaining code and move to the next iteration
                whereWillIGo.emplace_back(sampledLocation);
                placesVisited++;
                break;
            } else {
                const auto sampledLocationIndex = gsl_rng_uniform_int(p_rng, p_locations->size());
                Location *sampledLocation = &(*p_locations)[sampledLocationIndex]; //retrieves element "sampledLocationIndex" from the container pointed to
                //by "p_locations" & storing the address of that element in the "sampledLocation" pointer
                bool inSocialGroup = (find(socialGroupLocations.begin(), socialGroupLocations.end(), sampledLocation) != socialGroupLocations.end());
                bool alreadySampled = (find(whereWillIGo.begin(), whereWillIGo.end(), sampledLocation) != whereWillIGo.end());
                if (alreadySampled || inSocialGroup) //if you visit sampledLocation already, or sampledLocation is part of your social group
                    continue; //continue = skip the remaining code and move to the next iteration
                whereWillIGo.emplace_back(sampledLocation);
                placesVisited++;
                break;
            }
        } while (true); //evaluation at the beginning of the do/while loop.
    }
    
    for (Location *location : whereWillIGo) //location = pointer to location instances
    {
        location->registerInfectiousVisits(*this); //register infectious visit
        
        if (infectionStatus_ == InfectionStatus::kSusceptible) //register potential exposure of susceptibles
        {
            const int32_t infectiousVisits = location->infectedVisitsCountNTicksAgo(0); //number of invectious visits previous tick
            const double diseaseEstablishment = location->getRecentRiskScore(); //disease establishment in current tick
            for (int j = 0; j < infectiousVisits; j++)
            {
                const double infectionProbability = gsl_rng_uniform(p_rng);
                if (infectionProbability < diseaseEstablishment)
                    initiateInfection(p_exposureDuration);
            }
        }
    }
}

void Human::propagateInfection(const double p_infectionDuration, gsl_rng *p_rng)
{
    switch (infectionStatus_)
    {
        case InfectionStatus::kSusceptible:
            elapsedTicks_++;
            break;
        case InfectionStatus::kExposed:
            if (remainingTicks_ == 0) //exposed --> infected if exposed period has passed
            {
                
                double p = p_infectionDuration - floor(p_infectionDuration);
                double probabilities[] = {1 - p, p};  //calculate probabilities
                gsl_ran_discrete_t* dist = gsl_ran_discrete_preproc(2, probabilities);
                auto infectionDuration = floor(p_infectionDuration) + gsl_ran_discrete(p_rng, dist);
                gsl_ran_discrete_free(dist);
                
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
    p_os << p_human.remainingTicks_  << " ";
    p_os << p_human.socialGroup_.getID();
    return p_os;
}
