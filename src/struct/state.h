#include <sys/types.h>

#ifndef STATE_DEF
    #define STATE_DEF

    #define STATE_CNT 1
    #define NAME_LEN 256

    typedef struct District {

    } District;

    typedef struct precinct_t {
        char name[NAME_LEN];
        char county[NAME_LEN];
        struct precinct_t **neighbors;
        u_int32_t id;
        u_int16_t neighborCnt;
        u_int32_t pop;
        u_int16_t dist;
    } Precinct;

    typedef struct state_t {
        char abbr[3];
        char *name;
        u_int32_t pop;


        u_int32_t precinctCt;
        Precinct **precincts;


        u_int16_t distCt;
        u_int32_t *perDistTgt;
        u_int32_t *perDistPop;

        int weight; //CURRENTLY UNUSED - will eventually be used to pick precincts to put in a district together
    } State;


#endif