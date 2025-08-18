#include "cmd.h"

#include "redistrict.h"

int processGeoJSON(int argc, char *argv[]) {
    char pyName[64];
    memset(pyName, 0, 64);
    strcpy(pyName, PROC_PATH);

    strcpy(&pyName[strlen(pyName)], PROC_NAME);

    if(argc != 2) {
        fprintf(stderr, "Usage: ./bards.exe process <state abbr.> <year>\n");
        return -1;    
    }


    if(execlp("python3", "python3", pyName, "proc", argv[0], argv[1], NULL) == -1) {
        fprintf(stderr, "Error entering python");
        return -1;
    }
    return 0;    
}

int redistrict(int argc, char *argv[]) { //usage - ./bards.exe redistrict <state abbr.> <year> --n <num. dists> --o <name>
    redistrict_input_t redistrictInput;

    __uint8_t flags = 0b00;

    if(argc < 2) {
        goto badArgs;
    }

    redistrictInput.state = upperCaseStr(argv[0]);
    redistrictInput.year = atoi(argv[1]);

    for(int i = 2; i < argc; i++) {
        if(strcmp(argv[i], "--n") == 0) {
            i++;
            flags = flags | MKDIST_FLAG_NUMDIST;
            if(i >= argc) {
                goto badArgs;
            }
            redistrictInput.numDists = atoi(argv[i]);
        }
        else if(strcmp(argv[i], "--o") == 0) {
            i++;
            flags = flags | MKDIST_FLAG_NAME;
            if(i >= argc) {
                goto badArgs;
            }
            redistrictInput.name = argv[i];
        }
        else {
            goto badArgs;
        }
    }
    
    

    return makeDists(redistrictInput, flags);

badArgs:
    fprintf(stderr, "Usage: ./bards.exe process <state abbr.> <year> (--n <numDists>) (--o <name>)\n");
    return -1; 
}


char *upperCaseStr(char *str) {
    for(int i = 0; i < strlen(str); i++) {
        str[i] = toupper(str[i]);
    }
    return str;
}

int pdistrict(int argc, char *argv[]) {
    char pyName[64];
    memset(pyName, 0, 64);
    strcpy(pyName, PROC_PATH);

    strcpy(&pyName[strlen(pyName)], PROC_NAME);

    if(argc != 3) {
        fprintf(stderr, "Usage: ./bards.exe process <state abbr.> <year> < num>\n");
        return -1;    
    }


    if(execlp("python3", "python3", pyName, "simple", argv[0], argv[1], argv[2], NULL) == -1) {
        fprintf(stderr, "Error entering python");
        return -1;
    }
    return 0;    
}