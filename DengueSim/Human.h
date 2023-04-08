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

typedef enum class _InfectionStatus {
    kSusceptible = 0,
    kExposed,
    kInfected,
    kRecovered
} InfectionStatus;

typedef int32_t HumanID;

extern std::ostream &print(std::ostream &p_os, const InfectionStatus p_status);
extern std::ostream &print(std::ostream &p_os, const Human &p_human);

extern void generateHumans(std::vector<Location> &p_locations, std::vector<Human> *p_humans, const int32_t p_humanCount, const unsigned int p_tickCount);

extern void generateHumans(std::vector<Location> &p_locations, std::vector<Human> *p_humans, const double p_humansPerLocationNegBinomProb, const double p_humansPerLocationNegBinomN, const unsigned int p_tickCount, gsl_rng *p_rng);

class Human {
friend std::ostream &print(std::ostream &p_os, const Human &p_human);

public:
    Human(const Human&) = delete; //deactivate copy constructor: Human h(h12);
    Human& operator=(const Human&) = delete; //deactivate copy assignment: Human h = h2;
    Human(Human&&) noexcept = default;  //default move constructor to allow objects to move in memory (e.g., when std::vector reallocates its buffer)
    
    Human(const HumanID p_humanID, Location &p_homeLocation, InfectionStatus p_infectionStatus, unsigned int p_elapsedTicksInStatus, unsigned int p_remainingTicksInStatus): id_(p_humanID), homeLocation_(p_homeLocation), infectionStatus_(p_infectionStatus), elapsedTicks_(p_elapsedTicksInStatus), remainingTicks_(p_remainingTicksInStatus){}
    Human(const HumanID p_humanID, Location &p_homeLocation, unsigned int p_remainingTicksInStatus): Human(p_humanID, p_homeLocation, InfectionStatus::kSusceptible, 0, p_remainingTicksInStatus) {}
    
    const Location &getHomeLocation(void) const { return homeLocation_; }
    InfectionStatus getInfectionStatus(void) { return infectionStatus_; }
    unsigned int getRemainingTicksInStatus(void) { return remainingTicks_; }
    unsigned int getElapsedTicksInStatus(void) { return elapsedTicks_; }
    HumanID getID( void) {return id_; }
    void initiateInfection(unsigned int p_exposureDuration);
    void generateMovement(std::vector<Location> *p_locations, const double p_randomMovementShape, const double p_randomMovementRate, const unsigned int p_exposureDuration, gsl_rng *p_rng);
    void propagateInfection(const unsigned int p_minimumInfectionDuration, const unsigned int p_maximumInfectionDuration, unsigned int p_remainingTicksInSimulation, gsl_rng *p_rng);
    
private:
    HumanID id_;
    Location &homeLocation_; //reference, because home is not changing & must not be a null ptr.
    InfectionStatus infectionStatus_ = InfectionStatus::kSusceptible;
    unsigned int remainingTicks_ = 0; //remaining ticks in infectionStatus_
    unsigned int elapsedTicks_ = 0; //elapsed ticks in infectionStatus_
};

#endif /* HUMAN_H */
