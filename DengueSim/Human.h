//
//  Human.h
//  DD
//
//  Copyright © 2022 Anna Maria Langmüller. All rights reserved.
//

#ifndef HUMAN_H
#define HUMAN_H

#include <iostream>
#include <string>
#include <random>
#include <vector>
#include <fstream>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

class Human;
class Location;
class SocialGroup;

typedef enum class _InfectionStatus {
    kSusceptible = 0,
    kExposed,
    kInfected,
    kRecovered
} InfectionStatus;

typedef int32_t HumanID;

extern std::ostream &print(std::ostream &p_os, const InfectionStatus p_status);
extern std::ostream &print(std::ostream &p_os, const Human &p_human);

extern void generateHumans(std::vector<Location> &p_locations, std::vector<Human> *p_humans, const double p_humansPerLocationNegBinomProb, const double p_humansPerLocationNegBinomN, gsl_rng *p_rng, unsigned int p_locationsPerSocialGroup, std::vector<SocialGroup> *p_socialGroups);

class Human {
friend std::ostream &print(std::ostream &p_os, const Human &p_human);

public:
    Human(const Human&) = delete; //deactivate copy constructor: Human h(h12);
    Human& operator = (const Human&) = delete; //deactivate copy assignment: Human h = h2;
    Human(Human&&) noexcept = default;  //default move constructor to allow objects to move in memory (e.g., when std::vector reallocates its buffer)
    
    Human(const HumanID p_humanID, Location &p_homeLocation, SocialGroup &p_socialGroup, InfectionStatus p_infectionStatus): id_(p_humanID), homeLocation_(p_homeLocation), socialGroup_(p_socialGroup), infectionStatus_(p_infectionStatus) {}
    
    Location &getHomeLocation(void) const { return homeLocation_; }
    InfectionStatus getInfectionStatus(void) const { return infectionStatus_; }
    SocialGroup &getSocialGroup(void) const { return  socialGroup_; }
    unsigned int getRemainingTicksInStatus(void) const;
    unsigned int getElapsedTicksInStatus(void) const { return elapsedTicks_; }
    HumanID getID(void) const {return id_; }
    void initiateInfection(const unsigned int p_exposureDuration);
    void generateMovement(std::vector<Location> *p_locations, const double p_randomMovementShape, const double p_randomMovementRate, const unsigned int p_exposureDuration,const double p_proportionSocialVisits, gsl_rng *p_rng);
    void propagateInfection(const double p_infectionDuration, gsl_rng *p_rng);
    
private:
    HumanID id_;
    Location &homeLocation_; //reference, because home is not changing & must not be a null ptr.
    SocialGroup &socialGroup_; //reference: can't be null pointer, does not change
    InfectionStatus infectionStatus_ = InfectionStatus::kSusceptible;
    unsigned int remainingTicks_ = 0; //remaining ticks in infectionStatus_
    unsigned int elapsedTicks_ = 0; //elapsed ticks in infectionStatus_
};

#endif /* HUMAN_H */
