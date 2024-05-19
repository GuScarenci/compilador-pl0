#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"
#include "str_utils.h"
#include "IO_utils.h"

int main(int argc, char** argv){
    if(argc != 3){
        ABORT_PROGRAM("Usage: make run ARGS=\"<source_file> <output file>\"\n");
    }

    TokStream* b = token_stream_init(argv[1]);
    FILE* out_file;
    Token* t = NULL;

    OPEN_FILE(out_file, argv[2], "w")
    while ((t = get_next_token(b))) {
        fprintf(out_file, "%s \t, %s\n", t->token_str, t->type);
        free(t->token_str);
        free(t->type);
        free(t);
    }

    token_stream_free(&b);
    fclose(out_file);
}