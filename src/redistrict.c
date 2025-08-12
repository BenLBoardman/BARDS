#include "redistrict.h"

#define TEMP_DISTRICT_DEFAULT 2

int makeDists(redistrict_input_t input, __uint8_t flags) {
    State target;

    //stubs ensuring flags are set up properly
    if(flags & MKDIST_FLAG_NUMDIST) {
        printf("Number of districts set to %d.\n", input.numDists);
        target.distCt = input.numDists;
    } else {
        target.distCt = TEMP_DISTRICT_DEFAULT;
    }

        if(flags & MKDIST_FLAG_NAME) {
        printf("Output map should be named %s.\n", input.name);
    }

     
    if(readState(&target, input.state, input.year) == -1) {
        fprintf(stderr, "Read state from file failure\n");
        return -1;
    }

    buildDists(&target); 
    return 0;
}



/*************************************************************** 
 *  The first and simplest district-builder algorithm
 *  A variant of a DFS, this algorithm will start from a random precinct and assign neighboring precincts in order of discovery until all precincts are assigned 
 *  to districts. When a given district has the correct population, it will increment the currDist value and start doing the next district. This will repeat 
 *  until every precinct has been assigned to a district. This is intentionally an incredibly naive algorithm.
 *  
 */
int buildDistsDFS(State *state) {
    Stack dfsStack = initStack(NULL);

    int currDist = 1;
}