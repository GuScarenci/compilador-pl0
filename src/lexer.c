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
*           Arquivo lexer.c                                  *
************************************************************/

#include <stdlib.h>
#include <stdint.h>
#include <errno.h>

#include "lexer.h"
#include "IO_utils.h"
#include "str_utils.h"
#include "state_machine.h"

static const char* restrict first_line_keywords_dsv = "Keyword|Type";

void loadKeywords(const char* restrict filename, TokStream *tok_stream) {
    FILE* file;
    OPEN_FILE(file, filename, "r")

    char *line = NULL;
    readLine(&line, file);
    if (strcmp(line, first_line_keywords_dsv) != 0) {
        ABORT_PROGRAM("Transitions DSV %s has no header. The first line must be \"%s\".", 
                      filename, first_line_keywords_dsv)
    }

    free(line);
    while (!feof(file)) {
        readLine(&line, file);
        size_t num_fields;
        char** fields = split(line, "|", &num_fields);
        if (num_fields != NUM_FIELDS_KEYWORDS_DSV) {
            ABORT_PROGRAM("Malformed line on %s: %s\n"
                          "DSV must have %d fields per line.", 
                          filename, line, NUM_FIELDS_KEYWORDS_DSV)
        }

        XREALLOC(Keyword, tok_stream->keywords, tok_stream->num_keywords + 1)
        tok_stream->keywords[tok_stream->num_keywords].keyword = strdup(fields[0]);
        tok_stream->keywords[tok_stream->num_keywords].type = strdup(fields[1]);
        tok_stream->num_keywords++;

        free(line);
        free(*fields);
        free(fields);
    }

    fclose(file);
}

TokStream* token_stream_init(const char* restrict source_path) {
    TokStream* tok_stream = NULL;

    XCALLOC(TokStream, tok_stream, 1)
    OPEN_FILE(tok_stream->src_code, source_path, "r");
    initializeStateMachine(&(tok_stream->dfa));
    loadKeywords("res/keywords.csv", tok_stream);
    tok_stream->current_line = 1;

    return tok_stream;
}

void token_stream_free(TokStream** tok_stream) {
    if (tok_stream != NULL) {
        fclose((*tok_stream)->src_code);
    }

    for (size_t i = 0; i < (*tok_stream)->num_keywords; i++) {
        free((*tok_stream)->keywords[i].type);
        free((*tok_stream)->keywords[i].keyword);
    }

    free((*tok_stream)->keywords);
    freeStateMachine(&((*tok_stream)->dfa));
    free(*tok_stream);

    *tok_stream = NULL;
}

Token* get_next_token(TokStream* tok_stream) {
    if (tok_stream == NULL) {
        ABORT_PROGRAM("Vacuous token stream");
    }

    size_t token_buff_len = INIT_TOKEN_LEN;
    size_t token_len = 0;
    Token* token;
    XALLOC(Token, token, 1)
    XALLOC(char, token->token_str, token_buff_len)

    StateTransition* transition;
    while (((tok_stream->dfa.current_state)->type != reeturn) &&
           ((tok_stream->dfa.current_state)->type != error)) {
        char next_char = fgetc(tok_stream->src_code);
        if (feof(tok_stream->src_code)) {
            free(token->token_str);
            free(token);
            return NULL;
        }
        transition = getNextState(tok_stream->dfa.current_state, next_char);
        if (transition == NULL) {
            ABORT_PROGRAM("Reached undefined transition. Please define all transitions.")
        }

        (tok_stream->dfa).current_state = getState(&tok_stream->dfa, transition->nextState);

        if (transition->shift == GO_BACK) {
            fseek(tok_stream->src_code, -1, SEEK_CUR); // Head goes backwards.
        } else {
            if (token_len + 1 == token_buff_len) {
                token_buff_len *= TOKEN_GROWTH_FACTOR;
                XREALLOC(char, token->token_str, token_buff_len)
            }

            if(!isWhitespace(next_char)) { // Ignore whitespace and newline characters
                token->token_str[token_len] = next_char;
                token_len++;
            }else{
                if(next_char == '\n')
                    tok_stream->current_line++;
            }
        }

        if((tok_stream->dfa.current_state)->output == NULL)
            continue;

        if(strcmp((tok_stream->dfa.current_state)->output, "comment") == 0) {
            // Ignore comments
            free(token->token_str);
            free(token);
            token_buff_len = INIT_TOKEN_LEN;
            token_len = 0;
            XALLOC(Token, token, 1)
            XALLOC(char, token->token_str, token_buff_len)
        }
    }

    token->token_str[token_len] = '\0';

    char* output = tok_stream->dfa.current_state->output;
    if(strcmp(tok_stream->dfa.current_state->output, "identifier") == 0) {
        for (size_t i = 0; i < tok_stream->num_keywords; i++) {
            if (strcmp(token->token_str, tok_stream->keywords[i].keyword) == 0) {
                output = tok_stream->keywords[i].type;
                break;
            }
        }
    }

    token->type = strdup(output);
    tok_stream->dfa.current_state = tok_stream->dfa.initial_state;

    return token;
}
