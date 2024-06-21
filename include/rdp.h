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
*      Data de última atualizacao: 19/5/2024                *
*      Ambiente: VSCode 1.89.1                              *
*                                                           *
*                 Arquivo rdp.h                             *
************************************************************/

#ifndef __RDP_H__
#define __RDP_H__

#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "lexer.h"
#include "IO_utils.h"

#define SUCCESS 0
#define IMMEDIATE 1
#define PARENT 2
#define SYNC_ERROR 3

#define FIELD_TYPE 0
#define FIELD_STR 1

#define HANDLE_ERROR(error_message)                                                        \
    error_count++;                                                                         \
    fprintf(out_file, ANSI_COLOR_RESET "%s at line %ld: ",                                 \
                   token_stream->source_path, token_stream->current_line);                 \
    fprintf(out_file, ANSI_COLOR_RED "error: ");                                           \
    fprintf(out_file, ANSI_COLOR_RESET error_message "\n");
    

#define MATCH(field_type, str, error_message)                                              \
    do {                                                                                   \
        int32_t result = match_function(field_type, str, immediate_tokens, parent_tokens); \
        if(result != SUCCESS) {                                                            \
            HANDLE_ERROR(error_message)                                                    \
            if(result == PARENT || result == SYNC_ERROR) {                                 \
                return;                                                                    \
            }                                                                              \
        }                                                                                  \
    } while(false) //this is a way to avoid result redefinition


typedef struct sync_tokens {
    size_t num_tokens;
    char **token_types;
} SyncTokens;


void rdp(TokStream* b, FILE* out_fp);

int match(TokStream* b, char* comp_token);

void programa();

void bloco();

void declaracao();

void constante();

void mais_const();

void variavel();

void mais_var();

void procedimento();

void comando();

void mais_cmd();

void expressao();

void operador_unario();

void termo();

void mais_termos();

void fator();

void mais_fatores();

void condicao();

void relacional();

#endif
