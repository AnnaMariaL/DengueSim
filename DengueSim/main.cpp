//
//  main.cpp
//  DDv0
//
//  Created by Anna Maria on 10/17/22.
//


#include <stdio.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <iostream>
#include <vector>
#include <numeric> //needed for new Xcode version

using namespace std;

using std::cin;
using std::cout;
using std::cerr;
using std::endl;


int main (void)
{
    const gsl_rng_type * T;
    gsl_rng * r;
    
    int i, n=1000;
    double mu=3.0;
    vector<int> p;
    
    gsl_rng_env_setup();
    T = gsl_rng_default;
    r = gsl_rng_alloc(T);
    
    for (i=0; i<n; i++) {
        unsigned int k = gsl_ran_poisson(r, mu);
        p.push_back(k);
        printf(" %u", k);
    }
    printf("\n");
    gsl_rng_free(r);
    
    double m = accumulate(std::begin(p), std::end(p), 0.0);
    m = m / p.size();
    cout << m << endl;
    cout << "Changes to main." << endl;
  return 0;
}

/*#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <stdio.h>

#include "Human.h"
#include "Location.h"
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

using namespace std;

using std::cin;
using std::cout;
using std::cerr;
using std::endl;

*int main(int argc, const char * argv[]) {
    vector<Location> L;
    vector<Human> H;
    const gsl_rng_type T = gsl_rng_default;
    gsl_rng r = gsl_rng_alloc(T);

    int i , n = 10;
    double mu = 3;
    
    for (i=0; i<n; i++) {
        unsigned int k = gsl_ran_poisson(r,mu);
        printf(" %u",k);
    }
    
    printf("\n");
    gsl_rng_free(r);
    
    for(int i=0; i<10; i++) L.emplace_back(i); //10 locations
    
 
    for (int i=0; i<10; i++) { //4 humans per location
        for(int j=0;j<4;j++) {
            Location *p = &L[i];
            H.emplace_back(j,p,0);
        }
    }

    
for(auto h:H) print(cout, h) << endl; //print humans
return 0;
    
}*/
    
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
