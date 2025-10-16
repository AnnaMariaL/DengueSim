# DengueSim

**DengueSim** is an **individual-based model (IBM)** designed to simulate disease dynamics.
It is very loosely inspired by dengue and incorporates some key features shaping dengue epidemics, such as social structure, human movement, and seasonality. 
It was developed for  the study "*Gaussian Process emulation for exploring complex individual-based epidemic models*", which is currently available as preprint: 

[medRxiv, 2024.11.28.24318136v2](https://www.medrxiv.org/content/10.1101/2024.11.28.24318136v2)

---

## Overview

This repository contains the full source code for **DengueSim**:

- C++ source files implementing the IBM:
  - **Human**, **Location**, **SocialGroup** classes
  - `main.cpp` driver 
- build and compilation infrastructure (`CMakeLists.txt`)  

The repository also includes `.xcodeproj` files that allow opening and building the project in **Xcode**.
These `.xcodeproj` files were **only used during development** for convenience. They are **not required** to compile or run DengueSim.
The recommended cross-platform method is to use **CMake**, which is compatible with macOS, Linux, and Windows.

### Conceptual overview of the IBM

DengueSim simulates the spread of a disease with lasting immunity in a human population. 
A detailed IBM description, motivated by the Overview, Design concepts, and Details (ODD) protocol for describing IMBs (Grimm *et al.* 2006), can be found in the [preprint](https://www.medrxiv.org/content/10.1101/2024.11.28.24318136v2).
In brief: 

- DengueSim simulates **individuals (humans)** that move among **locations**
- **locations** belong to **social groups**; the clustering into **social groups** influences individual movement patterns
- transmission and movement events are probabilistic, using GSL random number generators 
- the IBM allows for exploration of how seasonality, human movement, and social structure shape population-level epidemic dynamics.
  
---

## Compilation Instructions
The following compilation instructions reflect our experience on macOS and do not cover all platforms or setups. 
They are provided as a guide to help users get started and may require adaptation for other operating systems or environments.

### Prerequisites

You will need:

- A **C++ compiler** (C++11 or later), e.g., AppleClang, Clang, or GCC  
- **CMake** 
- **GSL** (GNU Scientific Library)  

#### macOS (via Homebrew)
```bash
brew install gsl cmake
````

### Build Instructions

1. **Clone the repository:**

```bash
git clone https://github.com/AnnaMariaL/DengueSim.git
cd DengueSim
```

2. **Create a build directory and navigate into it:**

```bash
mkdir build
cd build
```

3. **Configure the project with CMake:**

```bash
cmake -DCMAKE_POLICY_VERSION_MINIMUM=3.5 ../DengueSim
```

If you encounter GSL-related errors during compilation, make sure your `CMakeLists.txt` includes the **correct paths to the GSL headers**. For example:
 ```cmake
target_include_directories(${TARGET_NAME} PRIVATE
"${PROJECT_SOURCE_DIR}/DengueSim"
"/opt/local/include"
"/opt/homebrew/include"
"/opt/homebrew/opt/gsl/include"
"/usr/local/include"
)
 ```
This approach ensures that CMake searches common system locations, Homebrew symlinks, and local installations. 
You may need to **adjust these paths** to match where GSL is installed on your machine.

4. **Compile the executable:**

```bash
make -j10
```

This will produce `build/denguesim`. 

## Running the simulation

Once you have compiled `denguesim`, you can run it.

If no command-line parameters are provided, the IBM uses the following **default values**:

**Note:** A full explanation of all command-line arguments is provided in the next section.

```text
-seed: 42
-alpha: 0.025
-alphaSeasonality: 0
-seasonalityPhaseShift: 0
-infectionDuration: 2.55
-avgNumberVisits: 4
-pNumberVisits: 0.5
-proportionSocialVisits: 0.5
-locationsPerSocialGroup: 4.5
-locationCount: 1000
-releaseScenario: H1
-output: foo.txt
```

You can also override any of these defaults by providing arguments when executing the program:

```bash
./denguesim -alpha 0.01 -avgNumberVisits 5 -locationCount 10000 -output myoutput.txt
```

### Example output

Running the IBM with default parameters produces a tab-separated text file (here, `foo.txt`) with daily counts of susceptible, exposed, infected, and recovered individuals:

```
1	6050	0	1	0
2	6050	0	1	0
3	6045	0	6	0
...
21	1	0	0	6050
```

Each row corresponds to a simulation day (or tick), with columns representing counts of susceptible, exposed, infected, and recovered individuals.

**Note**: Unless specified otherwise (by changing the variable `exposureDuration` in `main.cpp`), exposed individuals become infectious at the end of each day, effectively reducing the model to a Susceptible-Infectious-Recovered model, where the length of the infectious period is specified by the user.

##  Command-Line Arguments

The IBM can be customized using the command-line arguments listed below. Many parameters have been renamed in the manuscript for clarity. 

| Argument                   | Description                                                                                                                                                                                                                                                                                                                     | Manuscript Equivalent |
| -------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | --------------------- |
| `-alpha`                   | The average infection probability across a year, which removes the effect of seasonality                                                                                                                                                                                                                                        | Average infectivity   |
| `-alphaSeasonality`        | A scaling factor between 0 and 1 that controls the magnitude of seasonal variation in infection probability                                                                                                                                                                                                                     | Seasonality strength  |
| `-seasonalityPhaseShift`   | Determines the timing of the first case relative to the seasonal peak in infection probability                                                                                                                                                                                                                                  | First case timing     |
| `-infectionDuration`       | The average number of days an individual remains infectious, with actual days determined by probabilistically rounding to the nearest integers around the specified value                                                                                                                                                       | Infectious period     |
| `-avgNumberVisits`         | The average number of visits a human makes to locations per day, in addition to their family home                                                                                                                                                                                                                               | Average mobility      |
| `-pNumberVisits`           | The success probability in the negative binomial distribution that determines the number of visits a human makes per day; lower values result in greater variance in the daily visit count                                                                                                                                      | Mobility skewness     |
| `-proportionSocialVisits`  | The probability of a visit occurring within the family cluster of the individual moving                                                                                                                                                                                                                                         | Social structure      |
| `-locationsPerSocialGroup` | The average number of locations assigned to each cluster, with actual sizes determined by probabilistically rounding to the nearest integers around the specified value                                                                                                                                                         | Family cluster size   |
| `-locationCount`           | Total number of locations in the simulation                                                                                                                                                                                                                                                                                     | –                     |
| `-releaseScenario`         | Defines how the initial infection is seeded in the population. Options include: `H1` (seed infection in a single randomly selected individual, as used in the manuscript), `Hn` (seed infection in multiple randomly selected individuals), and `L1` (seed infection in all individuals in a single randomly selected location) | –                     |
| `-output`                  | Name of the output file where simulation results are written                                                                                                                                                                                                                                                                    | –                     |
| `-seed`                    | Random number seed controlling the stochasticity for reproducibility                                                                                                                                                                                                                                                            | –                     ||


**Note**: All parameters are optional. If omitted, the IBM uses default values (see the “Running the Simulation” section).
