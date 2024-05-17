#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"
#include "str_utils.h"

int main(int argc, char** argv){
    char a[] = "a,string,separated,by,commas";
    const char* delim = ",";
    int size;

    char** words = split(a, delim, &size);
    printf("Found %d tokens\n", size);

    for (int i = 0; i < size; i++) {
        printf("Token: %s\n", words[i]);
    }

    free(words);
}