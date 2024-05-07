#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"

int main(int argc, char** argv){
    TokenStream* tok_stream = token_stream_init("a.pl0");
    token_stream_free(tok_stream);
}