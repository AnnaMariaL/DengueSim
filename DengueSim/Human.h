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

class Human;
class Location;

extern std::ostream &print(std::ostream &os, const Human &h);

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
    
    
private:
    int32_t ID;
    Location &home; //reference, because home is not changing & must not be a null ptr. 
    unsigned infected_days=0;
};

#endif /* Human_h */
