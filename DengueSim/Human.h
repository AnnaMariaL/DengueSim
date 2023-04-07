//
//  Human.h
//  DDv0
//
//  Created by Anna Maria on 10/17/22.
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

extern std::ostream &print(std::ostream &p_os, InfectionStatus p_status);
extern std::ostream &print(std::ostream &p_os, const Human &p_human);

extern void generateHumans(std::vector<Location> &p_locations, std::vector<Human> *p_humans, const int32_t p_humanCount, int p_nTicks);

extern void generateHumans(std::vector<Location> &p_locations, std::vector<Human> *p_humans, const double p_humansPerLocationNegBinomProb, const double p_humansPerLocationNegBinomN, gsl_rng *p_rng, int p_nTicks);

class Human {
friend std::ostream &print(std::ostream &p_os, const Human &p_human);

public:     //constructors
    //deactivate copy constructor: Human h(h12);
    Human(const Human&) = delete;
    
    //deactivate copy assignment: Human h = h2;
    Human& operator=(const Human&) = delete;
    
    //default move constructor to allow objects to move in memory, such as when std::vector reallocates its buffer 
    Human(Human&&) noexcept = default;
    
    Human(const int32_t p_humanID, Location &p_homeLocation, InfectionStatus p_infectionStatus, unsigned int p_elapsedTicksInStatus, unsigned int p_remainingTicksInStatus): id_(p_humanID), homeLocation_(p_homeLocation), infectionStatus_(p_infectionStatus), elapsedTicks_(p_elapsedTicksInStatus), remainingTicks_(p_remainingTicksInStatus){}
    Human(const int32_t p_humanID, Location &p_homeLocation, unsigned int p_remainingTicksInStatus): Human(p_humanID, p_homeLocation, InfectionStatus::kSusceptible, 0, p_remainingTicksInStatus) {}
    
    const Location &getHomeLocation(void) const { return homeLocation_; }
    InfectionStatus getInfectionStatus(void) {return infectionStatus_;}
    unsigned int getRemainingTicksInStatus(void) {return remainingTicks_; }
    unsigned int getElapsedTicksInStatus(void) {return elapsedTicks_; }
    int32_t getID(void) {return id_; }
    void initiateInfection(unsigned int p_exposureDuration);
    void generateMovement(std::vector<Location> *p_locations, gsl_rng *p_rng, const double p_randomMovementShape, const double p_randomMovementRate, const unsigned int p_exposureDuration);
    void propagateInfection(const unsigned int p_minimumInfectionDuration, const unsigned int p_maximumInfectionDuration, gsl_rng *p_rng, int p_remainingTicks);
    
private:
    int32_t id_;
    Location &homeLocation_; //reference, because home is not changing & must not be a null ptr.
    InfectionStatus infectionStatus_ = InfectionStatus::kSusceptible;
    unsigned int remainingTicks_ = 0; //remaining ticks in infectionStatus_
    unsigned int elapsedTicks_ = 0; //elapsed ticks in infectionStatus_
};

#endif /* Human_h */
