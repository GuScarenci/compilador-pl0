#include <stdlib.h>
#include <stdint.h>

#include "lexer.h"
#include "IO_utils.h"

extern const char const* restrict type_map[] = {
    "identifier", "integer literal", "comment"
};


TokenStream* token_stream_init(const char* restrict source_path) {
    TokenStream* tok_stream;

    XALLOC(TokenStream, tok_stream, 1);
    OPEN_FILE(*tok_stream, source_path, "r");

    return tok_stream;
}

void token_stream_free(TokenStream* tok_stream) {
    if (tok_stream != NULL) {
        fclose(*tok_stream);
    }

    free(tok_stream);
}

Token* get_next_token(TokenStream* tok_stream) {
    if (tok_stream == NULL) {
        ABORT_PROGRAM("Cannot fetch token from vacuous stream");
    }

    // TODO
    // Implementar máquina de estados aqui

    return NULL;
}