# BARDS
### Now in C++!
**B**en's **A**lgorithmic **R**e**D**istricting **S**oftware



This is a programming package/library to support and use as a baseline for algorithmic redistricting.

Data comes primarily from DRA's database - https://github.com/dra2020/vtd_data/tree/master. This program should work with GeoJSON data in that repository, but is not guaranteed to work using data in other formats.

## States Supported
BARDS currently has 2020 precinct data for California, Colorado, Hawaii, New Hampshire, Vermont, and Wyoming. 2020 Precinct data for other states and 2010 precinct data will be added at a later date. This data is stored in the `data/<year>` directory. 

## Output
Completed maps are placed in `output/stateAbbr_<name>/`. The map itself is stored in `name.csv` within that directory.

## Installing and Running
If you are using the source code, all that is needed is a C++ compiler and Make. A precompiled build on github should be natively runnable if you have the right architecture.

To compile and run, one of the following makefile commands is recommended:
`make` / `make all` — Builds the release binary (same as running make compile).
`make compile` — Compiles all source files with optimizations and links them into bin/BARDS.
`make debug` — Compiles all source files with debug symbols and no optimization and links them into bin/BARDS_debug.
`make gdb` — Builds the debug binary (if needed) and launches it inside gdb for interactive debugging.
`make run` — Builds the release binary (if needed) and immediately executes it.
`make clean` — Deletes all compiled object files and both binaries (BARDS and BARDS_debug).

To run BARDS, use the command `make run <state> <year> {optional args}`. Command line arguments are planned as a future feature.

Below is a more detailed explanation of planned required and optional arguments:

- `state` is the two-letter abbreviation for thestate to draw the map for. See "States Supported" above to learn which states yhave precinct shapefiles included in the repository.
- `year` is the census data year for which data should be use. At present, 2020 is the only accepted option

**Optional Arguments**
- `name=<name>`: Give the output geoJSON a specific file name.
- `districts=<numDists>`: Make the map with a specified number of districts instead of the default number for the state.
- `info=<file>`: Output all info/debug logging to a file with a given name and the `.log` extension. If this is `log=console`, it will be outputted to the console. If no value is given, it will output to `info.log`.
- `report=<file>`: Output the summary report to a custom filename. If this is `report=console`, it will be outputted to the console. If no value is given, it will output to `report.log`.
- `map-count=<num>`: If this is set, BARDS will generate `num` maps with the same state data and will output summary statistics across all maps in the report instead of individual map statistics. This is intended to be used to benchmark different algorithms on various values. If this is set, the map itself will not be outputted.

## Known Issues
- (KI.1) There is currently no way to account for non-contiguous states. Districts bridging non-contiguous parts of a state (such as the different Hawaiian islands) will always show with contiguity checks failing & therefore incalculable compactness/area/other cached values. 

## Algorithms
Currently, BARDS supports one algorithm. Algorithms may be added periodically as pull requests containing new ones are approved. As they are, they will be described here.

## Current Features
The basic BARDS is functional but not complete. Below is a list of currently-planned features and their implementation status.

Initial Features
- District-level demographic/electoral data - complete
- Population deviation calculation - complete
- Compactness calculation - complete
- Command-line argument processor - complete
- Basic partisan fairness calculation - complete
- More command line arguments - complete
- Algorithm selection system - complete
- Better logging framework - complete
- Report generation (file containing population balance, partisan fairness, compactness info) - complete
- Fully realized BFS-based algorithm - complete
- Well established documentation - complete
- Generate multiple maps at once - complete
- All state data - complete


Future Goals
- More algorithms
- County tracking
- Report customizability
- Default configs
- More report metrics
- Multithreaded support
- Multithreaded precinct load
- Better contiguity checks (fixing KI.1)
- Better error messages

## Planned Algorithms
As the first developer, there are more algorithms I plan to add as well. These include (but may not be limited to) the following:

## Adding an algorithm
To add an algorithm, follow the instructions in `algo/example_algorithm.info`.