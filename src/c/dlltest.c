#include "dlltest.h"
#include <string.h>
#include <stdio.h>

void print() {
    printf("This is a DLL!\n");
}

void bigger(char * str, int dat) {
    printf("Integer data is %d...\n", dat);
    printf("Str is %s...\n", str);
    for(int i = 0; i < strlen(str); i++) {
        if(str[i] >= 'a' && str[i] <= 'z')
            str[i] -= 32;
    }
}