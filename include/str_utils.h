/************************************************************
*             Trabalho 2 - SCC0605                          *
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

#ifndef __STR_UTILS_H__
#define __STR_UTILS_H__

#include <stdbool.h>

#define CARRIAGE_RETURN 13    // Ascii code for carriage return

// Function to trim newline characters from a string
char* trimNewline(char* str);

// Function to split a line into tokens by a delimiter
char** split(const char* line, const char* delimiter, size_t* count);

/*
 * This function returns if a character is a numerical digit.
 * In our mini-regex notation, this recognizes \d.
 */
bool isDigit(char c);

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

#endif