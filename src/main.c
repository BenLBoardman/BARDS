#include <stdio.h>
#include <stdlib.h>
#include "state.h"

int main(int argc, char *argv) {
    char *abbr;
    State state;
    if(argc != 2) {
        goto abort;
    }

    abbr = argv[1];

    for(int i = 0; i < STATE_CNT; i++) {
        if(strcmp(abbr, stateAbbrs[i]) == 0) {
            state = getState(abbr);
            buildPrecincts(state);
            //todo generate
        }
    }

abort:
    printf("Usage: BARDS <state abbr.>");
    exit(0);
}