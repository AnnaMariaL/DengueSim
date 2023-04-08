//
//  Location.h
//  DD
//
//  Copyright © 2022 Anna Maria Langmüller. All rights reserved.
//

#ifndef LOCATION_H
#define LOCATION_H

#include <iostream>
#include <string>
#include <vector>
#include <deque>

class Human;
class Location;

typedef int32_t LocationID;

extern std::ostream &print(std::ostream &p_os, const Location &p_location);
extern void generateLocations(const int32_t p_locationCount, std::vector<Location> *p_locations, const long double p_currentRiskScore, const size_t p_historyLength);

class Location {
friend std::ostream &print(std::ostream &p_os, const Location &p_location);
    
public:
    Location(const Location&) = delete; //deactivate copy contructor: Location l(l2);
    Location& operator=(const Location&) = delete; //deactivate copy assignment: Location l = l2;
    Location(Location&&) noexcept = default; //default move constructor to allow objects to move in memory (e.g., when std::vector reallocates its buffer)
    
    explicit Location(const LocationID p_id, long double p_riskRcore, size_t p_historyLength): id_(p_id), infectedVisitsCount_(0), historyLength_(p_historyLength), riskScore_(p_riskRcore) {};

    LocationID getLocationID(void) const { return id_; }
    int32_t getRecentInfectedVisitorsCount(void) const { return infectedVisitsCount_; }
    size_t getVisitHistoryLength(void) const {return infectedVisitsCountsHistory_.size(); }
    size_t getRiskScoreHistoryLength(void) const {return riskScoreHistory_.size(); }
    long double getRecentRiskScore(void) const {return riskScore_;}
    int32_t infectedVisitsCountAtIndex(int p_i) {return infectedVisitsCountsHistory_.at(p_i);}
    long double riskScoreAtIndex(int p_i) {return riskScoreHistory_.at(p_i);}
    void storeRiskScoreAndNumberOfInfectedVisitors(long double p_currentDiseaseEstablishment);
    void registerInfectiousVisits(Human &p_visitor);

    void printRiskScoreHistory(void);
    void printVisitHistory(void);
    
private:
    LocationID id_;
    int32_t infectedVisitsCount_=0; //number of infectious visitors in current tick
    long double riskScore_=0; //disease establishment proportion in current tick
    size_t historyLength_; //length of the infectedVisitsCount_ & riskScore_ history that should be tracked per location
    std::deque<int32_t> infectedVisitsCountsHistory_ = {infectedVisitsCount_}; //history of infectedVisitsCount_
    std::deque<long double> riskScoreHistory_ = {riskScore_}; //history of riskScore_
};

#endif /* LOCATION_H */
