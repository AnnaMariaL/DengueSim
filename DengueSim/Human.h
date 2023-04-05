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

extern std::ostream &print(std::ostream &os, const Human &rHuman);

extern void generateHumans(vector<Location> &rLocations, vector<Human> *pHumans, const int32_t humanCount=6);

extern void generateHumans(vector<Location> &rLocations, vector<Human> *pHumans, const double randomProbability, const double randomSize, gsl_rng *prandomNumberGenerator);

class Human {
friend std::ostream &print(std::ostream &os, const Human &rHuman);

public:     //constructors
    //decativate copy constructor: Human h(h12);
    Human(const Human&) = delete;
    
    //deactivate copy assignment: Human h = h2;
    Human& operator=(const Human&) = delete;
    
    //default move constructor to allow objects to move in memory, such as when std::vector reallocates its buffer 
    Human(Human&&) noexcept = default;
    
    Human(const int32_t humanID, Location &rhomeLocation, unsigned int InfectionStatus, unsigned int NTicksInStatus): ID(humanID), rhomeLocation(rhomeLocation), InfectionStatus(InfectionStatus), NTicksInStatus(NTicksInStatus){}
    Human(const int32_t humanID, Location &rhomeLocation): Human(humanID,rhomeLocation,0,0) {}
    //void MoveAround(std::vector<Location> &allLocations); //infection happens here
    
    Location &GetHomeLocation(void) const { return rhomeLocation; }
    unsigned int getInfectionStatus() {return InfectionStatus;}
    unsigned int getNTicksInStatus() {return NTicksInStatus;}
    
    void initiateInfection(unsigned int ExposedDuration);
    void generateMovement(vector<Location> *pLocations, gsl_rng *prandomNumberGenerator, const double randomMovementShape, const double randomMovementRate, const unsigned int ExposedDuration);
    void propagateInfection(const unsigned int MinimumInfectionDuration, const unsigned int MaximumInfectionDuration, gsl_rng *prandomNumberGenerator);
    
private:
    int32_t ID;
    Location &rhomeLocation; //reference, because home is not changing & must not be a null ptr.
    unsigned int InfectionStatus=0; //0 = susceptible, 1 = exposed, 2 = infected, 3 = recovered
    unsigned int NTicksInStatus=0; //remaining ticks in status (i.e., 5 remaining infectious ticks), non-informative for susceptible class
};

#endif /* Human_h */
