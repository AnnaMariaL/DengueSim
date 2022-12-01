//
//  main.cpp
//  DDv0
//
//  Created by Anna Maria on 10/17/22.
//


#include <stdio.h>
#include <iostream>
#include <vector>
#include <map>

#include "Human.h"
#include "Location.h"



using namespace std;

int main(int argc, const char * argv[]) {
    vector<Location> L; //empty vector location
    vector<Human> H; //empty vector humans
    int32_t nlocation=20000; //total number of locations
    vector<int32_t> lcount(nlocation);
    double p=0.5939354; //Reiner et al. (2014)
    double n=9.01; //Reiner et al. (2014)
    const long int rns=42; //random number seed
    
    LocationSetup(nlocation, &L); //set up locations (use pointer to avoid copying)
    //HumanSetup(L, &H, 5); //set up humans, constant # of humans per location
    HumanSetup(L, &H, p, n, rns); //set up humans, # of humans follows negative binomial distribution
    
    int32_t myhome;
    //for(auto &l:L) print(cout, l) << endl; //print locations
    for(auto &h:H) {
        //print(cout, h) << endl; //print humans
        myhome=h.GetLocation().GetLocationID();
        //cout << myhome << endl;
        lcount[myhome]++;
    }
   
   //calculate mean # of humans per location
   double m = accumulate(std::begin(lcount), std::end(lcount), 0.0);
   m = m / lcount.size();
   cout << m << endl;
   
    //calculate var # of humans per location
   double sq_sum = std::inner_product(lcount.begin(), lcount.end(), lcount.begin(), 0.0);
   double stdev = sq_sum / lcount.size() - m * m;
   cout << stdev << endl;
return 0;
    
}



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
