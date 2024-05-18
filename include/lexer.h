#ifndef __LEXER_H__
#define __LEXER_H__

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>


#define NUM_FIELDS_CSV 3
#define INIT_TABLE_SIZE 256

#define HASH_GROWTH_FACTOR 2
#define PRIME 37

typedef enum state_type {
    initial = 0,
    error = 1,
    final = 2,
    regular = 3
} StateType;

#define STATE_HAS_EMPTY_OUTPUT(type) (type == initial || type == regular)
#define STATE_HAS_NON_EMPTY_OUTPUT(type) (type == error || type == final)

typedef struct transition_t {
    char* input;
    char* nextState;
} StateTransition;

typedef struct state_t {
    StateTransition* transitions;
    size_t transitionCount;
    char* stateName;
    StateType type;
} State;

typedef struct state_machine_t {
    State* states_hash;
    size_t stateCount;
    size_t hash_size;
    char** token_strs;
} StateMachine;

/*
 * Token Stream is an abtract data type that encapsulates
 * a single source file and its stream of token pairs.
 * It fulfills the task of lexical analysis, returning a stream
 * of tokens and their types, and any lexical errors, if they occur.
 */
typedef struct tok_stream_t {
    FILE* src_code;
    StateMachine dfa;
} TokStream;


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

/*
 * Initializes the state machine that will be used to tokenize the input.
*/
void initializeStateMachine();


#endif