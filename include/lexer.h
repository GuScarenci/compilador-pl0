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
*           Arquivo lexer.h                                 *
************************************************************/

#ifndef __LEXER_H__
#define __LEXER_H__

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>
#include "state_machine.h"

#define INIT_TOKEN_LEN 16
#define TOKEN_GROWTH_FACTOR 2
#define NUM_FIELDS_KEYWORDS_DSV 2

/*
 * A keyword pair is an abstract data type that encapsulates a
* keyword and its type, conformant to PL/0's grammar rules. 
*/
typedef struct keyword_pair_t{
    char* keyword;
    char* type;
} Keyword;

/*
 * Token Stream is an abstract data type that encapsulates
 * a single source file and its stream of token pairs.
 * It fulfills the task of lexical analysis, returning a stream
 * of tokens and their types, and any lexical errors, if they occur.
 */
typedef struct tok_stream_t {
    FILE* src_code;
    char* source_path;
    size_t current_line;

    StateMachine dfa;
    Keyword* keywords;
    size_t num_keywords;
} TokStream;

/*
 * A token pair is an abstract data type that encapsulates a
 * token and its type, conformant to PL/0's grammar rules.
 * If the token is ill-formed, the type field must point to
 * the appropriate error code.
 */
typedef struct token_pair_t {
    char* token_str;
    char* type;
} Token;

/*
 * Returns the Token Stream that reads from the source
 * file whose name is the string pointed to by 'source_path'.
 * The tokens are analysed in conformity to PL/0's grammar.
 */
TokStream* token_stream_init(const char* source_path);

/*
 * Frees resources utilized by Token Stream.
 */
void token_stream_free(TokStream** tok_stream);

/*
 * Returns the next token in the stream.
 */
Token* get_next_token(TokStream* tok_stream);

#endif
