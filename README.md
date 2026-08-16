# BARDS
### Now in C++!
**B**en's **A**lgorithmic **R**e**D**istrict **S**oftware



This is a programming package/library to support and use as a baseline for algorithmic redistricting.

Data comes primarily from DRA's database - https://github.com/dra2020/vtd_data/tree/master. This program should work with GeoJSON data in that repository, but is not guaranteed to work using data in other formats.

## States Supported
BARDS currently has 2020 precinct data for New Hampshire. 2020 Precinct data for other states and 2010 precinct data will be added at a later date. This data is stored in the `data/<year>` directory. 

## Output
Completed maps are placed in `output/stateAbbr.csv`. Better output systems are planned for the future. custom output locations are a possible future feature, but are not actively planned at present.

## Installing and Running
For full functionality, all that is needed is a C++ compiler and Make. Release builds may be available on github at a later time.

To compile, run `make` to build with the default parameters, or `make debug` to compile a version with extra functionality to use with a debugger. 

To run BARDS, use the command `make run <state> <year> {optional args}`. Command line arguments are planned as a future feature.

Below is a more detailed explanation of planned required and optional arguments:

-  NOT YET IMPLEMENTED `algo` refers to the algorithm to use. See "Algorithms" below for a detailed explanation of all options
- `state` is the two-letter abbreviation for thestate to draw the map for. See "States Supported" above to learn which states yhave precinct shapefiles included in the repository.
- `year` is the census data year for which data should be use. At present, 2020 is the only accepted option

**Optional Arguments**
- NOT YET IMPLEMENTED `name=<name>`: Give the output geoJSON a specific file name.
- `districts=<numDists>`: Make the map with a specified number of districts instead of the default number for the state.
- NOT YET IMPLEMENTED `debug=<file>`: Output all debug logging to a file with a given name and the `.log` extension. If this is instead `debug=true`, it will be outputted to `debug.log`.
- NOT YET IMPLEMENTED `report=<file>`: Output the summary report (a planned future feature) to a custom filename, instead of the default `report.log`.

## Algorithms
Currently, BARDS supports one algorithm. Algorithms may be added periodically as pull requests containing new ones are approved. As they are, they will be described here.

## Current Features
The basic BARDS is functional but not complete. Below is a list of currently-planned features and their implementation status.
<RELEASE 1 - INITIAL OPEN SOURCE>
- All state data - not started
- Compactness calculation - completed
- Command-line argument processor - completed
- Algorithm selection system - not started
- Fully realized BFS-based algorithm - not started
- Better error messages - not started
- Well established documentation - not started
- Population deviation calculation - not started
<RELEASE 2 - CONVENIENCE FEATURES>
- Partisan fairness calculation - not started
- Report generation (file containing population balance, partisan fairness, compactness info) - in progress
- Toggleable debug logging - not started


## Planned Algorithms
As the first developer, there are more algorithms I plan to add as well. These include (but may not be limited to) the following:

## Adding an algorithm
To add an algorithm, follow the instructions in `algo/example_algorithm.info`.