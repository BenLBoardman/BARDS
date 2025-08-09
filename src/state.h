#include "precinct.h"

typedef struct State {
    char abbr[3];
    char *name;
    int nameLen;
    int pop;
    int tgt_pop;


    int precinctCt;
    Precinct *precincts;


    int distCt;
    District *districts;
} State;

typedef struct District {

};

#define STATE_CNT 1

char *stateAbbrs[3] = {
    "NH\0"
};