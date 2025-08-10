#include "redistrict.h"

#include "state.h"
#include "io.h"


int makeDists(redistrict_input_t input, __uint8_t flags) {
    State target;

    //stubs ensuring flags are set up properly
    if(flags & MKDIST_FLAG_NUMDIST) {
        printf("Number of districts set to %d.\n", input.numDists);
    }

        if(flags & MKDIST_FLAG_NAME) {
        printf("Output map should be named %s.\n", input.name);
    }

     
    readState(&target, input.state, input.year);

    return 0;
}