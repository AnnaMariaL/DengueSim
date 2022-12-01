//
//  Location.h
//  DDv0
//
//  Created by Anna Maria on 10/17/22.
//

#ifndef Location_h
#define Location_h

#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Human;
class Location;


extern std::ostream &print(std::ostream &os, const Location &l);
extern void LocationSetup(const int32_t n, vector<Location> *l);

class Location {
friend std::ostream &print(std::ostream &os, const Location &l);
    
public:     //constructors
    //deactivate copy contructor: Location l(l2);
    Location(const Location&) = delete;
    
    //deactive copy assignment: Location l = l2;
    Location& operator=(const Location&) = delete;
    
    // default move constructor to allow objects to move in memory, such as when std::vector reallocates its buffer
    Location(Location&&) noexcept = default;
    
    explicit Location(const int32_t i): ID(i) {};
    
    int32_t GetLocationID(void) const { return ID; }
    
private:
    int32_t ID;
};


#endif /* Location_h */
