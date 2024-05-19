#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"
#include "str_utils.h"

int main(int argc, char** argv){
    if(argc != 2){
        printf("Usage: make run ARGS=\"<source_file>\"\n");
        exit(EXIT_FAILURE);
    }
    char* source_path = argv[1];

    TokStream* b = token_stream_init(source_path);
    Token* t = NULL;

    while ((t = get_next_token(b))) {
        printf("%s \t, %s\n", t->token_str, t->type);
        free(t->token_str);
        free(t->type);
        free(t);
    }

    token_stream_free(&b);
}