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
    
    explicit Location(const int32_t i, const int HistoryLength=10): ID(i), Currentvisits(0), HistoryLength(HistoryLength), CurrentRiskScore(0) {};
    int32_t getLocationID(void) const { return ID; }
    int32_t getCurrentVisits(void) const { return Currentvisits; }
    deque<int32_t>::size_type getHistoryLength(void) const {return VisitHistory.size(); }
    long double getCurrentRiskScore(void) const {return CurrentRiskScore;}
    void updateCharacteristics(long double currentDiseaseEstablishment);
    void registerVisit(Human &visitor);

    void printRiskScoreHistory(void);
    void printVisitHistory(void);
private:
    int32_t ID;
    int32_t Currentvisits; //number of infectious visitors in current tick
    long double CurrentRiskScore; //disease establishment proportion in current tick
    int HistoryLength; //length of the visitor history that should be tracked per location
    deque<int32_t> VisitHistory; //visitor history for <HistoryLength> ticks
    deque<long double> RiskScoreHistory; //disease establishment proportion history for <HistoryLength> ticks
};


#endif /* Location_h */
