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
#include <deque>

using namespace std;

class Human;
class Location;


extern std::ostream &print(std::ostream &os, const Location &rLocation);
extern void generateLocations(const int32_t locationCount, vector<Location> *pLocations);

class Location {
friend std::ostream &print(std::ostream &os, const Location &rLocation);
    
public:     //constructors
    //deactivate copy contructor: Location l(l2);
    Location(const Location&) = delete;
    
    //deactive copy assignment: Location l = l2;
    Location& operator=(const Location&) = delete;
    
    // default move constructor to allow objects to move in memory, such as when std::vector reallocates its buffer
    Location(Location&&) noexcept = default;
    
    explicit Location(const int32_t i): ID(i), visits(0) {};
    
    int32_t getLocationID(void) const { return ID; }
    int32_t getVisits(void) const { return visits; }
    void registerVisit(Human &visitor) {visits++; }
    void resetVisits(void) {visits=0;}
    
private:
    int32_t ID;
    int32_t visits;
    std::deque<int32_t> visitHistory;
};


#endif /* Location_h */
