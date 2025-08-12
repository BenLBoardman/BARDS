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

int buildDists(State *state) {

}