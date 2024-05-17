#ifndef __LEXER_H__
#define __LEXER_H__

#include <stdio.h>
#include <stdint.h>

#define MAX_LINE_LENGTH 1024
#define NUM_FIELDS_CSV 3

typedef enum TokenTypes {
    ident = 0,       // Identifier.
    num,             // Integer literals (a.k.a constants).
    comment,         // Inline comments.
    comma,       
    semicolon,
    left_par,        // Left parenthesis.
    right_par,       // Right parenthesis.
    assign_op,       // Assignment operator, i.e. := 
    rel_op,          // Relational operators: <>, =, <, <=, => and >.
    simb_plus,       // Plus sign, i.e. +
    simb_minus,      // Minus sign, i.e. -
    simb_mult,       // Multiply sign, i.e. *
    simb_div,        // Divide sign, i.e. /
    keyword_const,   // Keywords are case sensitive and all caps.
    keyword_var,    
    keyword_proc,    // PROCEDURE keyword
    keyword_call,   
    keyword_begin, 
    keyword_end,    
    keyword_if,     
    keyword_then,
    keyword_while,  
    keyword_do,
    keyword_odd,
    invalid_ident,   // Found invalid char on otherwise valid ident.
    invalid,         // Found invalid char
    invalid_num,     // Found invalid char on otherwise valid integer literal.
    unexp_break      // Found unexpected line break on inline comment.
} TokenType;

typedef struct transition_t {
    char* input;
    char* nextState;
} StateTransition;

typedef struct state_t {
    char* stateName;
    StateTransition *transitions;
    int transitionCount;
    int isFinal;
} State;

typedef struct state_machine_t {
    State* states;
    int stateCount;
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
    TokenType type;
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