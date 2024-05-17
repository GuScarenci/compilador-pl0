#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "str_utils.h"
#include "IO_utils.h"


// Function to trim newline characters from a string
char* trimNewline(char* str) {
    char* pos;
    if ((pos = strchr(str, '\n')) != NULL) {
        *pos = '\0';
    }

    if ((pos = strchr(str, '\r')) != NULL) {
        *pos = '\0';
    }

    return str;
}


// Function to split a line into tokens by a delimiter
char** split(char* line, const char* delimiter, int* count) {
    char** tokens = NULL;
    char* token = strtok(line, delimiter);
    int tokensCount = 0;

    while (token) {
        XREALLOC(char*, tokens, ++tokensCount)

        tokens[tokensCount - 1] = token;
        token = strtok(NULL, delimiter);
    }

    *count = tokensCount;
    return tokens;
}


bool isDigit(char c) {
    return c >= '0' && c <= '9';
}

bool isNonDigit(char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_';
}