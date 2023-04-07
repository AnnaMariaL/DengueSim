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

using namespace std;

class Human;
class Location;

typedef enum class _InfectionStatus {
    kSusceptible = 0,
    kExposed,
    kInfected,
    kRecovered
} InfectionStatus;

extern std::ostream &print(std::ostream &p_os, InfectionStatus p_status);
extern std::ostream &print(std::ostream &p_os, const Human &p_Human);

extern void generateHumans(vector<Location> &p_Locations, vector<Human> *p_Humans, const int32_t p_humanCount=6);

extern void generateHumans(vector<Location> &p_Locations, vector<Human> *p_Humans, const double p_humansPerLocationNegBinomProb, const double p_humansPerLocationNegBinomN, gsl_rng *p_randomNumberGenerator);

class Human {
friend std::ostream &print(std::ostream &p_os, const Human &p_Human);

public:     //constructors
    //decativate copy constructor: Human h(h12);
    Human(const Human&) = delete;
    
    //deactivate copy assignment: Human h = h2;
    Human& operator=(const Human&) = delete;
    
    //default move constructor to allow objects to move in memory, such as when std::vector reallocates its buffer 
    Human(Human&&) noexcept = default;
    
    Human(const int32_t p_humanID, Location &p_rhomeLocation, InfectionStatus p_infectionStatus, unsigned int p_ticksInStatus): id_(p_humanID), rhomeLocation_(p_rhomeLocation), infectionStatus_(p_infectionStatus), nTicksInStatus_(p_ticksInStatus){}
    Human(const int32_t p_humanID, Location &p_rhomeLocation): Human(p_humanID,p_rhomeLocation,InfectionStatus::kSusceptible,0) {}
    
    Location &getHomeLocation(void) const { return rhomeLocation_; }
    InfectionStatus getInfectionStatus() {return infectionStatus_;}
    unsigned int getNTicksInStatus() {return nTicksInStatus_;}
    
    void initiateInfection(unsigned int p_exposureDuration);
    void generateMovement(vector<Location> *p_Locations, gsl_rng *p_randomNumberGenerator, const double p_randomMovementShape, const double p_randomMovementRate, const unsigned int p_exposureDuration);
    void propagateInfection(const unsigned int p_minimumInfectionDuration, const unsigned int p_maximumInfectionDuration, gsl_rng *p_randomNumberGenerator);
    
private:
    int32_t id_;
    Location &rhomeLocation_; //reference, because home is not changing & must not be a null ptr.
    InfectionStatus infectionStatus_ = InfectionStatus::kSusceptible;
    unsigned int nTicksInStatus_ = 0; //remaining ticks in exposed & infectous class; non-informative for susceptible class; number of ticks in recovered class
};

#endif /* Human_h */
