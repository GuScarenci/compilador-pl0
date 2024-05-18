#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"
#include "str_utils.h"

int main(void){
    TokStream* b = token_stream_init("res/a.pl0");
    token_stream_free(&b);
}