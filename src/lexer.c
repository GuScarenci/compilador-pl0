#include <stdlib.h>
#include <stdint.h>
#include <errno.h>

#include "lexer.h"
#include "IO_utils.h"
#include "str_utils.h"
#include "state_machine.h"


TokStream* token_stream_init(const char* restrict source_path) {
    TokStream* tok_stream = NULL;

    XCALLOC(TokStream, tok_stream, 1)
    OPEN_FILE(tok_stream->src_code, source_path, "r");
    initializeStateMachine(&(tok_stream->dfa));

    return tok_stream;
}

void token_stream_free(TokStream** tok_stream) {
    if (tok_stream != NULL) {
        fclose((*tok_stream)->src_code);
    }

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
    while ((tok_stream->dfa.current_state)->type != reeturn) {
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
        if (tok_stream->dfa.current_state->type == error) {
            fprintf(stderr, "%s", tok_stream->dfa.current_state->output);
        }

        if (transition->shift == GO_BACK) {
            fseek(tok_stream->src_code, -1, SEEK_CUR); // Head goes backwards.
        } else {
            if (token_len + 1 == token_buff_len) {
                token_buff_len *= TOKEN_GROWTH_FACTOR;
                XREALLOC(char, token->token_str, token_buff_len)
            }

            token->token_str[token_len] = next_char;
            token_len++;
        }
    }

    token->type = strdup(tok_stream->dfa.current_state->output);
    token->token_str[token_len] = '\0';
    tok_stream->dfa.current_state = tok_stream->dfa.initial_state;

    return token;
}
