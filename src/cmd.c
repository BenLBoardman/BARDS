#include "cmd.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int processGeoJSON(int argc, char *argv[]) {
    char pyName[64];
    memset(pyName, 0, 64);
    strcpy(pyName, PROC_PATH);

    strcpy(&pyName[strlen(pyName)], PROC_NAME);


    if(execlp("python3", "python3", pyName, NULL) == -1) {
        fprintf(stderr, "Error entering python");
        return -1;
    }
    return 0;
}
