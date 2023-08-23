//
//  SocialGroup.h
//  DD
//
//  Copyright © 2022 Anna Maria Langmüller. All rights reserved.
//

#ifndef SOCIALGROUP_H
#define SOCIALGROUP_H

#include <vector>
#include <cstdint>

class Human;
class Location;
class SocialGroup;

typedef int32_t SocialGroupID;

class SocialGroup {
public:
    SocialGroup(const SocialGroup&) = delete; //deactivate copy constructor: SocialGroup s(s12);
    SocialGroup& operator = (const SocialGroup&) = delete; //deactivate copy assignment: SocialGroup s = s2;
    SocialGroup(SocialGroup&&) noexcept = default;  //default move constructor to allow objects to move in memory (e.g., when std::vector reallocates its buffer)
    
    explicit SocialGroup(const SocialGroupID p_socialGroupID): socialGroupID_(p_socialGroupID) {}
    
    const std::vector<Location *> &Locations(void) { return locations_; } //return locations assigned to social group
    SocialGroupID getID(void) const {return socialGroupID_; } //return ID
    void AddLocation(Location &p_location) { locations_.push_back(&p_location); } //add locations to private data member locations_
    
private:
    SocialGroupID socialGroupID_;
    std::vector<Location *> locations_;
    
};

#endif /* SOCIALGROUP_H */
