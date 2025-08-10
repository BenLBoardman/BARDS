#include "io.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "state.h"

#define BUF_SZ 2048

char buf[BUF_SZ];

int readState(State *state, char *stateAbbr, int year) {
    char fileName[256] = {0};
    char cwd[256] = {0};
    FILE *fp = NULL;

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
    //TODO process state header

    readPrecinct

    return 0;
}
