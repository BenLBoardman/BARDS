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
    char *tok, *pcts, *strtokPtr;
    FILE *fp = NULL;
    int fileIndex, pctNum;
    Precinct *curr;

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
    state->pop = atoi(tok);
    tok = strtok(NULL, "|");
    state->precinctCt = atoi(tok);
    state->precincts = malloc(sizeof(Precinct*) * state->precinctCt);

    printf("Precinct ct: %d\n", state->precinctCt);

    for(int i = 0; i < state->precinctCt; i++) {
        state->precincts[i] = malloc(sizeof(Precinct));
        curr = state->precincts[i];
        fgets(buf, BUF_SZ, fp);

        tok = strtok(buf, "|");
        fileIndex = atoi(tok);
        if(fileIndex != i) {
            fprintf(stderr, "File precinct no. does not match with calculated.\n");
            return -1;
        }

        tok = strtok(NULL, "|");
        strncpy(curr->name, tok, NAME_LEN);

        tok = strtok(NULL, "|");
        strncpy(curr->county, tok, NAME_LEN);

        tok = strtok(NULL, "|");
        curr->pop = atoi(tok);

        tok = strtok(NULL, "|");
        curr->neighborCnt = atoi(tok);

        curr->neighbors = malloc(sizeof(Precinct*) * curr->neighborCnt);
        tok = strtok(NULL, "|");

        strtokPtr = tok;
        for(int j = 0; j < curr->neighborCnt; j++) {
            pcts = strtok(strtokPtr, ", ");
            strtokPtr = NULL;

            pctNum = atoi(pcts);
            curr->neighbors[j] = pctNum;
        }
    }

    for(int i = 0; i < state->precinctCt; i++) {
        curr = state->precincts[i];
        for(int j = 0; j < curr->neighborCnt; j++) {
            pctNum = curr->neighbors[j];
            curr->neighbors[j] = state->precincts[pctNum];
        }
    }

    return 0;
}
