#ifndef __STR_UTILS_H__
#define __STR_UTILS_H__

#include <stdbool.h>

#define CARRIAGE_RETURN 13    // Ascii code for carriage return

// Function to trim newline characters from a string
char* trimNewline(char* str);

// Function to split a line into tokens by a delimiter
char** split(const char* line, const char* delimiter, int* count);

bool isDigit(char c);

bool isNonDigit(char c);

bool isSymbol(char c);

bool isWhitespace(char c);

#endif