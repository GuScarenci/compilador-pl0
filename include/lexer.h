#ifndef __LEXER_H__
#define __LEXER_H__

#include <stdio.h>
#include <stdint.h>

/*
 * Token Stream is an abtract data type that encapsulates
 * a single source file and its stream of token pairs.
 * It fulfills the task of lexical analysis, returning a stream
 * of tokens and their types, and any lexical errors, if they occur.
 */
typedef FILE TokStream;

/*
 * A token pair is an abstract data type that encapsulates a
 * token and its type, conformant to PL/0's grammar rules.
 * If the token in ill-formed,the type field must point to
 * the appropriate error code.
 */
typedef struct token_pair_t {
    char* token_str;
    uint32_t type;
} Token;


/*
 * Returns the Token Stream that reads from the source
 * file whose name is the string pointed to by 'source_path'.
 * The tokens are analysed in conformity to PL/0's grammar.
 */
TokStream* token_stream_init(const char* restrict source_path);

/*
 * Frees resources utilized by Token Stream.
 */
void token_stream_free(TokStream** tok_stream);

/*
 * Returns the next token in the stream.
 */
Token* get_next_token(TokStream* tok_stream);


#endif