#include <stdlib.h>
#include <stdio.h>

#include "cmd.h"
#include "struct/state.h"
#include "struct/stack.h"
#include "io.h"

#define MKDIST_FLAG_NUMDIST 0x01
#define MKDIST_FLAG_NAME 0x02

int makeDists(redistrict_input_t, __uint8_t);

int buildDists(State*);