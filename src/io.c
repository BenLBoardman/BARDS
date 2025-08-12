#include "io.h"

#define BUF_SZ 2048

char buf[BUF_SZ];

int readState(State *state, char *stateAbbr, int year) {
    char fileName[256] = {0};
    char *tok, *pcts, *strtokPtr;
    FILE *fp = NULL;
    u_int64_t fileIndex, pctNum, tmpPop;
    Precinct *curr;

    memset(buf, 0, BUF_SZ);
    sprintf(fileName, "%s/%s_%d%s", BARD_DIRECTORY, stateAbbr, year, BARD_ARCHIVE_EXT);

    fp = fopen(fileName, "r");
    if(fp == NULL) {
        fprintf(stderr, "Failed to open file %s.\n", fileName);
        goto error;
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
            goto error;
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
            pctNum = (u_int64_t)curr->neighbors[j];
            curr->neighbors[j] = state->precincts[pctNum];
        }
    }


    fclose(fp);

    tmpPop = state->pop;
    state->perDistPop = malloc(sizeof(u_int32_t) * state->distCt);
    for(int i = 0; i < state->distCt; i++) {
        state->perDistPop[i] = state->pop / state->distCt;
        tmpPop -= state->perDistPop[i];
    }
    for(int i = 0; tmpPop > 0; tmpPop--) {
        state->perDistPop[i]++;
    }

    return 0;

error:
    fclose(fp);
    return -1;
}
