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
char** split(const char* line, const char* delimiter, size_t* count) {
    char* pos = strdup(line);
    char* token = NULL;
    char** tokens = NULL;

    int tokensCount = 0;
    while ((token = strtok_r(pos, delimiter, &pos)) != NULL) {
        XREALLOC(char*, tokens, ++tokensCount)
        tokens[tokensCount - 1] = token;
    }

    *count = tokensCount;
    return tokens;
}


bool isDigit(char c) {
    return c >= '0' && c <= '9';
}

bool isNonDigit(char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || isSymbol(c);
}

bool isLetter(char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

bool isSymbol(char c) {
    return ('(' <= c && c <= '/') || (':' <= c && c <= '>');
}

bool isWhitespace(char c) {
    return ('\t' <= c && c <= CARRIAGE_RETURN) || (c == ' '); 
}

bool isNewline(char c) {
    return c == '\n';
}