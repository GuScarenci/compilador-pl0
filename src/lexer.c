#include <stdlib.h>
#include <stdint.h>

#include "lexer.h"
#include "IO_utils.h"
#include "str_utils.h"

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
