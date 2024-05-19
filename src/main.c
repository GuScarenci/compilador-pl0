#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"
#include "str_utils.h"

int main(void){
    TokStream* b = token_stream_init("res/a.pl0");
    Token* t = NULL;

    while (t = get_next_token(b)) {
        printf("Found token %s of type %s", t->token_str, t->type);
        free(t);
    }

    token_stream_free(&b);
}