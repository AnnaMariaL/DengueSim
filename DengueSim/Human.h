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
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

using namespace std;

class Human;
class Location;

extern std::ostream &print(std::ostream &os, const Human &h);
extern void HumanSetup(vector<Location> &l, vector<Human> *h, const int32_t n=6);
extern void HumanSetup(vector<Location> &l, vector<Human> *h, const double p, const double n, const long int rns=2);

class Human {
friend std::ostream &print(std::ostream &os, const Human &h);

public:     //constructors
    //decativate copy constructor: Human h(h12);
    Human(const Human&) = delete;
    
    //deactivate copy assignment: Human h = h2;
    Human& operator=(const Human&) = delete;
    
    //default move constructor to allow objects to move in memory, such as when std::vector reallocates its buffer 
    Human(Human&&) noexcept = default;
    
    Human(const int32_t p_i, Location &p_home, unsigned p_d): ID(p_i), home(p_home), infected_days(p_d) {}
    Human(const int32_t p_i, Location &p_home): Human(p_i,p_home,0) {}
    //std::vector<Location>& move(const std::vector<Location> &L);
    //void MoveAround(std::vector<Location> &allLocations); //infection happens here
    
    Location &GetLocation(void) const { return home; }
    
private:
    int32_t ID;
    Location &home; //reference, because home is not changing & must not be a null ptr. 
    unsigned infected_days=0;
};

#endif /* Human_h */
