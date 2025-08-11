#include "io.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "state.h"

#define BUF_SZ 2048

char buf[BUF_SZ];

int readState(State *state, char *stateAbbr, int year) {
    char fileName[256] = {0};
    char cwd[256] = {0};
    char *tok;
    FILE *fp = NULL;
    int fileIndex;


    memset(buf, 0, BUF_SZ);
    getcwd(cwd, sizeof(cwd));
    sprintf(fileName, "%s/%s/%s_%d%s", cwd, BARD_DIRECTORY, stateAbbr, year, BARD_ARCHIVE_EXT);

    fp = fopen(fileName, "r");
    if(fp == NULL) {
        fprintf(stderr, "Failed to open file %s.\n", fileName);
        return -1;
    }

    fgets(buf, BUF_SZ, fp);
    printf("%s\n", buf);

    //TODO process filled state header
    tok = strtok(buf, "|");
    tok = strtok(NULL, "|");
    tok = strtok(NULL, "|");
    state->precinctCt = atoi(tok);
    state->precincts = malloc(sizeof(Precinct) * state->precinctCt);

    printf("Precinct ct: %d\n", state->precinctCt);

    for(int i = 0; i < state->precinctCt; i++) {
        fgets(buf, BUF_SZ, fp);
        printf("Precinct: %s\n", buf);

        tok = strtok(buf, "|");
        fileIndex = atoi(tok);
        if(fileIndex != i) {
            fprintf(stderr, "File precinct no. does not match with calculated.\n");
            return -1;
        }
    }

    return 0;
}
