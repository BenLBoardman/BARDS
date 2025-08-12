#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "struct/state.h"

#define BARD_DIRECTORY "data/processed"
#define BARD_ARCHIVE_EXT ".brd"

int readState(State *, char*, int);