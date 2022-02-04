#!/bin/bash


echo "Starting configuration...."
# CXXFLAGS="-Wall -g -O0" ./waf configure
# CXXFLAGS="-Wall -g -O3" ./waf -d optimized configure
# CXXFLAGS="-Wall -g -O0" ./waf configure --enable-mpi
CXXFLAGS="-Wall -g -O3" ./waf configure
# CXXFLAGS="-Wall -g -O3 -fno-align-functions -fno-align-loops" ./waf configure
