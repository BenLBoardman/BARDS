#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cmd.h"

struct {
    char *cmd;
    int (*f)(int, char*[]); //function ptr
}cmds[] = {
    { "process", processGeoJSON }
};


int main(int argc, char *argv[]) {
    char *cmd;
    int reslt;
    if(argc < 2) {
        goto abort;
    }

    cmd = argv[1];

    for(int i = 0; i < NUM_CMDS; i++) {
        if(strcmp(cmd, cmds[i].cmd) == 0) {
            reslt = cmds[i].f(argc - 2, &argv[2]);
            exit(reslt);
        }
    }

abort:
    printf("Usage: BARDS <CMD> <params>\n");
    exit(0);
}