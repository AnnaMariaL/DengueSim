//
//  Location.cpp
//  DD
//
//  Created by Anna Maria Langmüller on 10/20/22.
//  Copyright © 2022 Anna Maria Langmüller. All rights reserved.
//

#include <stdio.h>
#include "Location.h"


std::ostream &print(std::ostream &os, const Location &l)
{
    os << l.ID;
    return os;
}
