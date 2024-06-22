/************************************************************
*             Trabalho 1 - SCC0605                          *
*                                                           *
*      Nome: Artur Brenner Weber                            *
*      nUSP: 12675451                                       *
*      Nome: Gabriel Franceschi Libardi                     *
*      nUSP: 11760739                                       *
*      Nome: Guilherme Castanon Silva Pereira               *
*      nUSP: 11801140                                       *
*      Nome: Gustavo Scarenci de Carvalho Pereira           *
*      nUSP: 12547792                                       *
*      Nome: Matheus Henrique Dias Cirillo                  *
*      nUSP: 12547750                                       *
*      Data de última atualizacao: 22/06/2024               *
*      Ambiente: VSCode 1.89.1                              *
*                                                           *
*           Arquivo str_utils.c                             *
************************************************************/

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

bool isLetter(char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

bool isSymbol(char c) {
    return ('(' <= c && c <= '/') || (':' <= c && c <= '>');
}

bool isWhitespace(char c) {
    return ('\t' <= c && c <= CARRIAGE_RETURN) || (c == ' '); 
}
