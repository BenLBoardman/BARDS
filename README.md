# BARDS
**B**en's **A**lgorithmic **R**e**D**istrict **S**oftware

A programming package for algorithmic redistricting.

Installation:
1. For full functionality, the following software is required:
    - A C compiler (recommendation: GCC)
    - Python3
    - GeoPandas 

2. Compile the C portion of the code with the command `make bards` from the root of the project. The output will be stored in the executable file `bards`.


**Runtime**
To run BARDS, use the command ./bards <command> <args>

At present, there are two commands that are accepted, each with its own additional arguments.

*process S Y* converts a GeoJSON precinct file into a format expected by the BARDS redistrict command. It uses an external Python script.
    Process takes two additional arguments - a state name S and a year Y. It will read the GeoJSON file S_Y.geojson from the bards/data/geojson folder and output the file S_Y.brd in the bards/data/processed folder

*redistrict S Y* draws districts based on a set of precincts described in a `.brd` file. It expects these files to be located in bards/data/processed and takes the following arguments.
- S - the state to be redistricted. Required to find the `.brd` with information.
- Y - the census/data year to use. Required to find the `.brd` with information.
- In addition, it takes the following optional arguments:
- *--n <number>* - The number of districts to draw. Otherwise, it will default to a value read from the `*.brd` file
- *--o <name>* - A name to give to the output map.


**CURRENT ROADMAP**
The basic version of BARDS is not complete. Below is a list of currently-planned features and their implementation status.
NOTE: This list of features only reflects the features necessary for a minimal functional program. more features will be added as these are completed.
- processor - basic precinct data - COMPLETE
- processor - precinct neighbor data - COMPLETE
- main app - basic runtime structure - COMPLETE
- redistrict - read from .brd - TESTING
- processor - basic state-level data - IN PROGRESS
- processor - precinct demographic data - NOT STARTED
- processor - precinct electoral data - NOT STARTED
- redistrict - add precincts to districts - NOT STARTED
- redistrict - ensure districts have good population - NOT STARTED
- redistrict - output to .csv = NOT STARTED
