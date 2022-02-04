for build work properly, disable warnings as error

# configure as:

CXXFLAGS="-Wall -g -O0" ./waf configure

# run as:

./waf --run lte-v2x

* make sure there is lte-v2x directory and source files are present
in lte-v2x/scratch/ directory


# there are scripts that can configure and run:

Configure: ./configure.sh
Run simulation: ./run_simulation.sh


# to change gcc version
sudo update-alternatives --config gcc

