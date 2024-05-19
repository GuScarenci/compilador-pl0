#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"
#include "str_utils.h"

int main(void){
    TokStream* b = token_stream_init("res/program.pl0");
    Token* t = NULL;

    while ((t = get_next_token(b))) {
        printf("%s\t, %s\n", t->token_str, t->type);
        free(t);
    }

    token_stream_free(&b);
}