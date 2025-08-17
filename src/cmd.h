#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifndef CMD_HEADER_H
    #define CMD_HEADER_H

    #define NUM_CMDS 2
    #define PROC_PATH "src/proc/"
    #define PROC_NAME "redistricter.py"

    int processGeoJSON(int, char *[]);
    int redistrict(int, char *[]);
    int pdistrict(int, char *[]);
    char *upperCaseStr(char *);

    typedef struct redistrict_input_t {
        char *state;
        int year;
        int numDists;
        char *name;
    } redistrict_input_t;

#endif //CMD_HEADER_H