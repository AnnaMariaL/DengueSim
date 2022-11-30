//
//  main.cpp
//  DDv0
//
//  Created by Anna Maria on 10/17/22.
//


#include <stdio.h>
#include <iostream>
#include <vector>

#include "Human.h"
#include "Location.h"

#include <numeric> //needed for new Xcode version
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

using namespace std;


void LocationSetup(const int32_t n, vector<Location> *l) { //use pointer to locations to avoid copying
    for(int32_t i=0; i<n; i++) l->emplace_back(i);
}
void HumanSetup(vector<Location> &l, vector<Human> *h, const int32_t n=6) {
    //reference to location (will not be changed), pointer to humans (avoid copying)
    int32_t id=0; //initialize human ID
    for (auto &a : l) {//reference to individual location
        for(int32_t i=0; i<n; i++) {
            h->emplace_back(id,a,0); //add single human
            id++; //increment human ID
        }
    }
}

int main(int argc, const char * argv[]) {
    vector<Location> L; //empty vector location
    vector<Human> H; //empty vector humans
    
    int32_t nlocation=20; //total number of locations

    LocationSetup(nlocation, &L); //set up locations (use pointer to avoid copying)
    HumanSetup(L, &H, 3); //set up humans (use pointer to avoid copying)
 
    //for(auto &l:L) print(cout, l) << endl; //print locations
    for(auto &h:H) print(cout, h) << endl; //print humans
return 0;
    
}

    
    /*std::default_random_engine generator;
    std::poisson_distribution<int> distribution(4);
    
    vector<int> p;
    for(int i=0; i<1000; i++) {
        int number = distribution(generator);
        p.push_back(number);
    }
    
    double m = std::accumulate(std::begin(p), std::end(p), 0.0);
    m = m / p.size();
    cout << m;
    */



//int main (void)
//{
//    const gsl_rng_type * T;
//    gsl_rng * r;
//
//    int i, n=1000;
//    double mu=3.0;
//    vector<int> p;
//
//    gsl_rng_env_setup();
//    T = gsl_rng_default;
//    r = gsl_rng_alloc(T);
//
//    for (i=0; i<n; i++) {
//        unsigned int k = gsl_ran_poisson(r, mu);
//        p.push_back(k);
//        printf(" %u", k);
//    }
//    printf("\n");
//    gsl_rng_free(r);
//
//    double m = accumulate(std::begin(p), std::end(p), 0.0);
//    m = m / p.size();
//    cout << m << endl;
//    cout << "Another change to main." << endl;
//  return 0;
//}
