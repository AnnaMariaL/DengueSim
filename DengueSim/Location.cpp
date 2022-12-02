//
//  Location.cpp
//  DD
//
//  Created by Anna Maria Langmüller on 10/20/22.
//  Copyright © 2022 Anna Maria Langmüller. All rights reserved.
//

#include <stdio.h>
#include "Location.h"


std::ostream &print(std::ostream &os, const Location &rLocation)
{
    os << rLocation.ID;
    return os;
}

void generateLocations(const int32_t locationCount, vector<Location> *pLocations) { //use pointer to locations to avoid copying
    for(int32_t i=0; i<locationCount; i++) pLocations->emplace_back(i);
}

