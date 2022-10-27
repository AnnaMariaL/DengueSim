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

class Human;
class Location;

extern std::ostream &print(std::ostream &os, const Location &l);

class Location {
friend std::ostream &print(std::ostream &os, const Location &l);
    
public:     //constructors
    Location(const int32_t &i): ID(i) {};
private:
    int32_t ID;
};


#endif /* Location_h */
