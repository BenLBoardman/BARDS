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

To compile and run, one of the following makefile commands is recommended:
`make` / `make all` — Builds the release binary (same as running make compile).
`make compile` — Compiles all source files with optimizations and links them into bin/BARDS.
`make debug` — Compiles all source files with debug symbols and no optimization and links them into bin/BARDS_debug.
`make gdb` — Builds the debug binary (if needed) and launches it inside gdb for interactive debugging.
`make run` — Builds the release binary (if needed) and immediately executes it.
`make clean` — Deletes all compiled object files and both binaries (BARDS and BARDS_debug).

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

## Known Issues
- (KI.1) There is currently no way to account for non-contiguous states. Districts bridging non-contiguous parts of a state (such as the different Hawaiian islands) will always show with contiguity checks failing & therefore incalculable compactness/area/other cached values. 

## Algorithms
Currently, BARDS supports one algorithm. Algorithms may be added periodically as pull requests containing new ones are approved. As they are, they will be described here.

## Current Features
The basic BARDS is functional but not complete. Below is a list of currently-planned features and their implementation status.
<RELEASE 1 - INITIAL OPEN SOURCE>
- District-level demographic/electoral data - completed
- Population deviation calculation - completed
- Compactness calculation - completed
- Command-line argument processor - completed
- Partisan fairness calculation - in progress
- All state data - in progress
- Report generation (file containing population balance, partisan fairness, compactness info) - in progress
- More command line arguments - not started
- Algorithm selection system - not started
- Fully realized BFS-based algorithm - not started
- Better error messages - not started
- Well established documentation - not started
- Toggleable debug logging - not started
<FUTURE RELEASES>
- Multithreaded support - not started
- Multithreaded precinct load - not started
- Better contiguity checks (fixing KI.1) - not started

## Planned Algorithms
As the first developer, there are more algorithms I plan to add as well. These include (but may not be limited to) the following:

## Adding an algorithm
To add an algorithm, follow the instructions in `algo/example_algorithm.info`.