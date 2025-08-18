# BARDS
**B**en's **A**lgorithmic **R**e**D**istrict **S**oftware

A programming package for algorithmic redistricting.

Data primarily from DRA's database - https://github.com/dra2020/vtd_data/tree/master. This program should work with GeoJSON data in that repository, but is not guaranteed to work using data in other formats.

## States Supported
Nevada, New Hampshire, New Jersey

## Installing and Running
For full functionality, the following software is required:
    - Python3
    - GeoPandas 
Cloning the repository contains all the necessary code otherwise.

To run BARDS, use the command python3 BARDS.py <algo> <state> <year> [other...]

Below is a more detailed explanation of required and optional arguments

`algo` refers to the algorithm to use. See "Algorithms" below for a detailed explanation of all options
`state` is the two-letter abbreviation for thestate to draw the map for. See "States Supported" above to learn which states yhave precinct shapefiles included in the repository.
`year` is the census data year for which data should be use. At present, 2020 is the only accepted option

**Optional Arguments**
`--o name`
`--d numDists`: Make the map with a specified number of districts instead of the default number for the state.

## Algorithms
Currently, BARDS supports one algorithm. Algorithms may be added periodically as pull requests containing new ones are approved.
- SimpleBFS (Author: @BenLBoardman) - A simple BFS-based algorithm that creates concentric ring districts originating from a specific precinct. This algorithm is partisanship-blind and does not consider contiguity. It is intended to show how the BARDS infrastructure can be used to build an algorithm.

## Current Roadmap
The basic version of BARDS is not complete. Below is a list of currently-planned features and their implementation status.
NOTE: This list of features only reflects the features necessary for a minimal functional program. more features will be added as these are completed.
- processor - load data - COMPLETE
- main app - basic runtime structure - COMPLETE
- simple algorithm - add precincts to districts - COMPLETE
- processor - precinct neighbor analysis - COMPLETE
- processor - combine district geometries - COMPLETE
- processor - output to geoJSON - COMPLETE
- Remove unnecessary C code - COMPLETE
- simple algorithm - random starting precinct - COMPLETE
- main app - improvements & parameter checking - IN PROGRESS
- ALL - expand documentation - IN PROGRESS
- simple algorithm - console messages - IN PROGRESS
- data - get other state shapefiles - IN PROGRESS
- contiguous simple algorithm - contiguity guarantee - NOT STARTED
- simple redistrict - better population balancing - NOT STARTED

