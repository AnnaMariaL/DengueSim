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


extern std::ostream &print(std::ostream &p_os, const Location &p_Location);
extern void generateLocations(const int32_t p_locationCount, vector<Location> *p_Locations);

class Location {
friend std::ostream &print(std::ostream &p_os, const Location &p_Location);
    
public:     //constructors
    //deactivate copy contructor: Location l(l2);
    Location(const Location&) = delete;
    //deactive copy assignment: Location l = l2;
    Location& operator=(const Location&) = delete;
    // default move constructor to allow objects to move in memory, such as when std::vector reallocates its buffer
    Location(Location&&) noexcept = default;
    explicit Location(const int32_t p_i, deque<int32_t>::size_type p_historyLength=10): id_(p_i), currentvisits_(0), historyLength_(p_historyLength), currentRiskScore_(0) {};
    
    int32_t getLocationID(void) const { return id_; }
    int32_t getCurrentVisits(void) const { return currentvisits_; }
    deque<int32_t>::size_type getHistoryLength(void) const {return visitHistory_.size(); }
    long double getCurrentRiskScore(void) const {return currentRiskScore_;}
    void updateCharacteristics(long double p_currentDiseaseEstablishment);
    void registerVisit(Human &p_visitor);

    void printRiskScoreHistory(void);
    void printVisitHistory(void);
    
private:
    int32_t id_;
    int32_t currentvisits_; //number of infectious visitors in current tick
    long double currentRiskScore_; //disease establishment proportion in current tick
    deque<int32_t>::size_type historyLength_; //length of the visitor history that should be tracked per location
    deque<int32_t> visitHistory_; //visitor history for <HistoryLength> ticks
    deque<long double> riskScoreHistory_; //disease establishment proportion history for <HistoryLength> ticks
};


#endif /* Location_h */
