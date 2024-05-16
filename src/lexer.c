#include <stdlib.h>
#include <stdint.h>

#include "lexer.h"
#include "IO_utils.h"


TokStream* token_stream_init(const char* restrict source_path) {
    TokStream* tok_stream = NULL;

    OPEN_FILE(tok_stream, source_path, "r");

    return tok_stream;
}

void token_stream_free(TokStream** tok_stream) {
    if (tok_stream != NULL) {
        fclose(*tok_stream);
    }

    *tok_stream = NULL;
}

Token* get_next_token(TokStream* tok_stream) {
    if (tok_stream == NULL) {
        ABORT_PROGRAM("Vacuous token stream");
    }

    // TODO
    // Implementar máquina de estados aqui

    return NULL;
}