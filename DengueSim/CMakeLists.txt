# To use CMake to build DengueSim, create a new subdirectory alongside the source directory (assumed here
# to be named DengueSim), e.g., "build", then run the following commands:
#
#   cd build
#   cmake ../DengueSim
#   make -j10
#
# This will make a Release build, with optimization and without debugging symbols, by default.
# The built executables will be placed in the build directory upon successful completion. The optional
# -j argument specifies the number of threads to use for the build, which should be a value related 
# to the number of cores available on your machine. Typically this value does not exceed the number of cores.

# 
# You can also explicitly make a Release build; this is typically done in a directory named "Release"
# instead of "build":
#
#   mkdir Release
#   cd Release
#   cmake -D CMAKE_BUILD_TYPE=Release ../DengueSim
#   make 
#
# Or you can make a Debug build (without optimization, with debugging symbols):
#
#   mkdir Debug
#   cd Debug
#   cmake -D CMAKE_BUILD_TYPE=Debug ../DengueSim
#   make 
#

# In all cases the concept is the same: make a build directory of some name, cd into it, run cmake
# to set up the build (with a CMAKE_BUILD_TYPE flag if desired, otherwise Release will be used by
# default), then run make to actually do the build.  This setup (1) keeps all build products out of
# your source tree, which is generally a good idea, and (2) allows you to have both Release and
# Debug builds going simultaneously.
#
# You can do "make VERBOSE=1" instead of just "make" to see the full command lines used.  To
# rebuild all of cmake's internal caches etc. (which is generally a good idea after a "git pull",
# for example, or after the addition or removal of source files), the simplest thing is generally
# to touch the CMakeLists.txt file in the source tree top-level directory:
#
#   touch ../DengueSim/CMakeLists.txt
#
# Then you can just do "make"; cmake will automatically be re-run by make since the CMakeLists.txt
# file has changed.


cmake_minimum_required (VERSION 2.8.12 FATAL_ERROR)


project(DengueSim)

# Make a Release build by default
if(NOT CMAKE_BUILD_TYPE) 
    set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_BUILD_TYPE)

# Report the build type
message(STATUS "CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}")


# Use the flags below for [all / Debug / Release] builds; these flags are built in to cmake
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wno-attributes -Wunused-label -Wunused-variable -Wunused-value -Wno-pragmas -Wempty-body -Wshadow -Wparentheses -Wswitch -Wsign-compare -Wno-sign-conversion -Wuninitialized")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -Og -DDEBUG=1")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")


set(TARGET_NAME denguesim)
file(GLOB_RECURSE DENGUESIM_SOURCES  ${PROJECT_SOURCE_DIR}/*.cpp)
add_executable(${TARGET_NAME} ${DENGUESIM_SOURCES})
target_include_directories(${TARGET_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/DengueSim" "/opt/local/include/" "/opt/homebrew/Cellar/gsl/2.7.1/include/" "/usr/local/Cellar/gsl/2.7.1/include/")

# old code
#target_link_directories(${TARGET_NAME} PUBLIC "/opt/local/lib/" "/opt/homebrew/Cellar/gsl/2.7.1/lib/" "/usr/local/Cellar/gsl/2.7.1/lib/")

find_library(myGSL gsl PATHS "/opt/local/lib/" "/opt/homebrew/Cellar/" "/usr/local/Cellar/")

if (${myGSL} STREQUAL myGSL-NOTFOUND)
	message(FATAL_ERROR "GSL not found")
else()
	message(STATUS "GSL found at ${myGSL}")
endif()

find_library(myBLAS gslcblas PATHS "/lib64/")

if (${myBLAS} STREQUAL myBLAS-NOTFOUND)
	message(FATAL_ERROR "BLAS bot found")
else()
	message(STATUS "BLAS found at ${myBLAS}")
endif()

target_link_libraries(${TARGET_NAME} PUBLIC ${myGSL} ${myBLAS})

install(TARGETS denguesim DESTINATION bin)



