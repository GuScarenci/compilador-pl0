/************************************************************
*             Trabalho 1 - SCC0605                          *
*                                                           *
*      Nome: Artur Brenner Weber                            *
*      nUSP: 12675451                                       *
*      Nome: Gabriel Franceschi Libardi                     *
*      nUSP: 11760739                                       *
*      Nome: Guilherme Castanon Silva Pereira               *
*      nUSP: 11801140                                       *
*      Nome: Gustavo Scarenci de Carvalho Pereira           *
*      nUSP: 12547792                                       *
*      Nome: Matheus Henrique Dias Cirillo                  *
*      nUSP: 12547750                                       *
*      Data de última atualizacao: 19/5/2024                *
*      Ambiente: VSCode 1.89.1                              *
*                                                           *
*           Arquivo lex_types.h                             *
************************************************************/

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