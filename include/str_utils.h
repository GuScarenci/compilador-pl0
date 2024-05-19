#ifndef __STR_UTILS_H__
#define __STR_UTILS_H__

#include <stdbool.h>

#define CARRIAGE_RETURN 13    // Ascii code for carriage return

// Function to trim newline characters from a string
char* trimNewline(char* str);

// Function to split a line into tokens by a delimiter
char** split(const char* line, const char* delimiter, int* count);

/*
 * This function returns if a character is a numerical digit.
 * In our mini-regex notation, this recognizes \d.
 */
bool isDigit(char c);

/*
 * This function returns if a character is a letter or valid symbol in PL/0.
 * In our mini-regex notation, this recognizes \D.
 */
bool isNonDigit(char c);

/*
 * This function returns if a character is a valid symbol in PL/0.
 * In our mini-regex notation, this recognizes \p.
 */
bool isSymbol(char c);

/*
 * This function returns if a character is a letter.
 * In our mini-regex notation, this recognizes \l.
 */
bool isLetter(char c);

/*
 * This function returns if a character is a whitespace.
 * In our mini-regex notation, this recognizes \s.
 */
bool isWhitespace(char c);

/*
 * This function returns if a character is a newline. 
*/
bool isNewline(char c);

#endif