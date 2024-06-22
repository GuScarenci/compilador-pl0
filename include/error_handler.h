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
*           Arquivo error_handler.c                         *
************************************************************/
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "lexer.h"
#include "IO_utils.h"

#ifndef __ERROR_HANDLER_H__
#define __ERROR_HANDLER_H__

void print_message(FILE *out_file, char* message, char *color);

void print_error(FILE *out_file, char* error_message, Token error_line);

void print_warning(FILE *out_file, char *warning_message, Token warning_token);

void print_final_message(FILE* out_file);

#endif