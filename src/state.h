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
        int neighborCnt;
        int pop;
    } Precinct;

    typedef struct state_t {
        char abbr[3];
        char *name;
        int nameLen;
        int pop;
        int tgt_pop;


        int precinctCt;
        Precinct **precincts;


        int distCt;
        District *districts;
    } State;

#endif