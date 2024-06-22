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
*                 Arquivo rdp.c                             *
************************************************************/
#include <string.h>

#include "lexer.h"
#include "rdp.h"

static TokStream* token_stream;
static Token* current_token = NULL;
static FILE* out_file = NULL;
static ErrorListHead error_list = {0, NULL};

void add_error(ErrorListHead* error_list, char* error_message, Token error_token){
    error_list->error_count++;
    ErrorListNode* new_error = (ErrorListNode*)malloc(sizeof(ErrorListNode));
    new_error->error_message = error_message;
    new_error->error_line = error_token.line;
    new_error->token_start_pos = error_token.first_char_pos;
    new_error->token_size = error_token.size;
    new_error->next = NULL;
    if(error_list->first_error == NULL)
        error_list->first_error = new_error;
    else{
        ErrorListNode* current = error_list->first_error;
        while(current->next != NULL)
            current = current->next;
        current->next = new_error;
    }
}

char* read_line_from_file(const char* filename, size_t line_number) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    size_t current_line = 1;

    while ((read = getline(&line, &len, file)) != -1) {
        if (current_line == line_number) {
            fclose(file);
            return line;
        }
        current_line++;
    }

    fclose(file);
    free(line); // Line wasn't found, free the memory
    return NULL;
}

void print_line_with_highlight(FILE* out_fp, const char* line, size_t start_char, size_t length) {
    size_t line_len = strlen(line);
    for (size_t i = 0; i < line_len; i++) {
        if (i == start_char) {
            fprintf(out_fp, ANSI_COLOR_RED);
        }
        if (i == start_char + length) {
            fprintf(out_fp, ANSI_COLOR_RESET);
        }
        fputc(line[i], out_fp);
    }
    fprintf(out_fp, ANSI_COLOR_RESET);
}

void rdp(TokStream* b, FILE* out_fp){
    token_stream = b;
    current_token = get_next_token(token_stream);
    out_file = out_fp;
    programa();
    if (error_list.error_count == 0) {
        fprintf(out_file, "Code compiled successfully!\n");
    } else {
        fprintf(out_file, "Code did not compile. ");
        fprintf(out_file, ANSI_COLOR_RED);
        fprintf(out_file, "%ld", error_list.error_count);
        fprintf(out_file, ANSI_COLOR_RESET);
        fprintf(out_file, " errors found.\n");

        ErrorListNode *current = error_list.first_error;
        while (current != NULL) {
            fprintf(out_file, "%s:%ld:%ld:", token_stream->source_path, current->error_line, current->token_start_pos);
            fprintf(out_file, ANSI_COLOR_RED);
            fprintf(out_file, " error: ");
            fprintf(out_file, ANSI_COLOR_RESET);
            fprintf(out_file, "%s\n", current->error_message);

            fprintf(out_file, ANSI_COLOR_RESET); // Ensure the reset code is printed at the beginning
            fprintf(out_file, "\t%ld | ", current->error_line);
            char *source_path = token_stream->source_path;
            char* line = read_line_from_file(source_path, current->error_line);
            if (line) {
                print_line_with_highlight(out_fp, line, current->token_start_pos, current->token_size);
                free(line); // Free the memory allocated by getline
            }

            current = current->next;
        }
    }
}

int part_of(char* string, SyncTokens sync_tokens){
    for(size_t i = 0; i < sync_tokens.num_tokens; i++){
        if(!strcmp(sync_tokens.token_types[i], string)){
            return 1;
        }
    }
    return 0;
}

int match_function(int field, char* comp_type, char *error_msg, SyncTokens immediate_tokens, SyncTokens parent_tokens){
    int match_expected = 0;
    if(field == FIELD_TYPE)
        match_expected = !strcmp(current_token->type, comp_type);
    else
        match_expected = !strcmp(current_token->token_str, comp_type);

    if(match_expected){
        current_token = get_next_token(token_stream);
        return SUCCESS;
    } else{
        bool lexical_error = false;

        if(current_token->is_error){
            lexical_error = true;
            add_error(&error_list, current_token->type, *current_token);
        }else{
            add_error(&error_list, error_msg, *current_token);
        }

        if((immediate_tokens.num_tokens + parent_tokens.num_tokens) == 0)
            return SYNC_ERROR;

        while(current_token != NULL){
            if(part_of(current_token->type, immediate_tokens)){
                if(lexical_error)
                    return LEXICAL_ERROR;
                return IMMEDIATE;
            }
            if(part_of(current_token->type, parent_tokens)){
                if(lexical_error)
                    return LEXICAL_ERROR;
                return PARENT;
            }

            current_token = get_next_token(token_stream);
        }
        return SYNC_ERROR;
    }
}

void programa(){
    SyncTokens parent_tokens = {0, NULL};
    SyncTokens immediate_tokens = {0, NULL};
    bloco();
    MATCH(FIELD_TYPE, "period", "Expected a period at the end of the program");
    //epsilon
}

void bloco(){
    //sync_tokens = {"period", "semicolon"};
    declaracao();
    comando();
    //epsilon
}

void declaracao(){
    //sync_tokens = {"keyword_call", "keyword_begin", "keyword_if", "identifier", "keyword_while", "period", "semicolon"}
    constante();
    variavel();
    procedimento();
    //epsilon
}

void constante(){
    SyncTokens parent_tokens = {9, (char*[]){"keyword_var", "keyword_proc", "keyword_call", "keyword_begin", "keyword_if", "identifier", "keyword_while", "period", "semicolon"}};
    SyncTokens immediate_tokens = {0, NULL};
    
    if(!strcmp(current_token->type, "keyword_const")){
        immediate_tokens = (SyncTokens){1, (char*[]){"identifier"}};
        MATCH(FIELD_TYPE, "keyword_const", "Expected CONST declaration");

        immediate_tokens = (SyncTokens){1, (char*[]){"rel_op"}};
        MATCH(FIELD_TYPE, "identifier", "Expected identifier in CONST declaration");

        immediate_tokens = (SyncTokens){1, (char*[]){"integer_literal"}};
        MATCH(FIELD_STR, "=", "Use '=' for CONST assignment");

        immediate_tokens = (SyncTokens){2, (char*[]){"semicolon", "comma"}};
        MATCH(FIELD_TYPE, "integer_literal", "Expected number after '=' in CONST declaration");

        mais_const();

        immediate_tokens = (SyncTokens){0, NULL};
        MATCH(FIELD_TYPE, "semicolon", "Missing ';'");
    }
    //epsilon
}

void mais_const(){
        SyncTokens parent_tokens = {1, (char*[]){"semicolon"}};
    SyncTokens immediate_tokens = {0, NULL};

    if(!strcmp(current_token->type,"comma")){
        immediate_tokens = (SyncTokens){1, (char*[]){"identifier"}};
        MATCH(FIELD_TYPE, "comma", "Expected ',' in CONST declaration");

        immediate_tokens = (SyncTokens){1, (char*[]){"rel_op"}};
        MATCH(FIELD_TYPE, "identifier", "Expected identifier after ',' in CONST declaration");

        immediate_tokens = (SyncTokens){1, (char*[]){"integer_literal"}};
        MATCH(FIELD_STR, "=", "Use '=' for CONST assignment");

        immediate_tokens = (SyncTokens){2, (char*[]){"semicolon", "comma"}};
        MATCH(FIELD_TYPE, "integer_literal", "Expected number after '=' in CONST declaration");

        mais_const();
    }
    //epsilon
}

void variavel(){
    SyncTokens parent_tokens = {8, (char*[]){"keyword_proc", "keyword_call", "keyword_begin", "keyword_if", "identifier", "keyword_while", "period", "semicolon"}};
    SyncTokens immediate_tokens = {0, NULL};

    if(!strcmp(current_token->type, "keyword_var")){
        immediate_tokens = (SyncTokens){1, (char*[]){"identifier"}};
        MATCH(FIELD_TYPE, "keyword_var", "Expected VAR declaration");

        immediate_tokens = (SyncTokens){2, (char*[]){"comma", "semicolon"}};
        MATCH(FIELD_TYPE, "identifier", "Expected identifier in VAR declaration");

        mais_var();

        immediate_tokens = (SyncTokens){0, NULL};
        MATCH(FIELD_TYPE, "semicolon", "Missing ';'");
    }
    //epsilon
}

void mais_var(){
    SyncTokens parent_tokens = {1, (char*[]){"semicolon"}};
    SyncTokens immediate_tokens = {0, NULL};

    if(!strcmp(current_token->type,"comma")){
        immediate_tokens = (SyncTokens){1, (char*[]){"identifier"}};
        MATCH(FIELD_TYPE, "comma", "Expected ',' in VAR declaration");

        immediate_tokens = (SyncTokens){2, (char*[]){"comma", "semicolon"}};
        MATCH(FIELD_TYPE, "identifier", "Expected identifier after ',' in VAR declaration");

        mais_var();
    }
    //epsilon
}


void procedimento(){
    SyncTokens parent_tokens = {8, (char*[]){"keyword_call", "keyword_begin", "keyword_if", "identifier", "keyword_while", "period", "semicolon", "keyword_proc"}};
    SyncTokens immediate_tokens = {0, NULL};

    if(!strcmp(current_token->type, "keyword_proc")){
        immediate_tokens = (SyncTokens){1, (char*[]){"identifier"}};
        MATCH(FIELD_TYPE, "keyword_proc", "Expected PROCEDURE declaration");

        immediate_tokens = (SyncTokens){1, (char*[]){"semicolon"}};
        MATCH(FIELD_TYPE, "identifier", "Expected identifier in PROCEDURE declaration");

        immediate_tokens = (SyncTokens){9, (char*[]){"keyword_const", "keyword_var", "keyword_proc", "identifier", "keyword_call", "keyword_begin", "keyword_if", "keyword_while", "semicolon"}};
        MATCH(FIELD_TYPE, "semicolon", "Missing ';'");

        bloco();

        immediate_tokens = (SyncTokens){8, (char*[]){"keyword_proc","keyword_call", "keyword_begin", "keyword_if", "identifier", "keyword_while", "period", "semicolon"}};
        MATCH(FIELD_TYPE, "semicolon", "Missing ';'");

        procedimento();
    }
    //epsilon
}

void comando(){
    SyncTokens parent_tokens = {3, (char*[]){"keyword_end", "period", "semicolon"}};
    SyncTokens immediate_tokens = {0, NULL};

    if(!strcmp(current_token->type, "identifier")){
        immediate_tokens = (SyncTokens){1, (char*[]){"assign_op"}};
        MATCH(FIELD_TYPE, "identifier", "Expected identifier in COMMAND");

        immediate_tokens = (SyncTokens){5, (char*[]){"identifier", "integer_literal", "left_par", "simb_plus", "simb_minus"}};
        MATCH(FIELD_STR, ":=", "Use ':=' for assignment");

        expressao();
    } else if (!strcmp(current_token->type, "keyword_call")){
        immediate_tokens = (SyncTokens){1, (char*[]){"identifier"}};
        MATCH(FIELD_TYPE, "keyword_call", "Expected CALL");

        immediate_tokens = (SyncTokens){0, NULL};
        MATCH(FIELD_TYPE, "identifier", "Expected identifier after CALL");
    } else if (!strcmp(current_token->type, "keyword_begin")){ //BEGIN
        immediate_tokens = (SyncTokens){5, (char*[]){"keyword_call", "keyword_begin", "keyword_if", "identifier", "keyword_while"}};
        MATCH(FIELD_TYPE, "keyword_begin", "Expected BEGIN");

        comando();
        mais_cmd();

        immediate_tokens = (SyncTokens){0, NULL};
        MATCH(FIELD_TYPE, "keyword_end", "Expected END in BEGIN");
    } else if (!strcmp(current_token->type, "keyword_if")){ //IF
        immediate_tokens = (SyncTokens){6, (char*[]){"keyword_odd", "identifier", "integer_literal", "left_par", "simb_plus", "simb_minus"}};
        MATCH(FIELD_TYPE, "keyword_if", "Expected IF");

        condicao();

        immediate_tokens = (SyncTokens){5, (char*[]){"keyword_call", "keyword_begin", "keyword_if", "identifier", "keyword_while"}};
        MATCH(FIELD_TYPE, "keyword_then", "Expected a THEN in IF condition");

        comando();
    } else if (!strcmp(current_token->type, "keyword_while")){
        immediate_tokens = (SyncTokens){6, (char*[]){"keyword_odd", "identifier", "integer_literal", "left_par", "simb_plus", "simb_minus"}};
        MATCH(FIELD_TYPE, "keyword_while", "Expected WHILE");

        condicao();

        immediate_tokens = (SyncTokens){5, (char*[]){"keyword_call", "keyword_begin", "keyword_if", "identifier", "keyword_while"}};
        MATCH(FIELD_TYPE, "keyword_do", "Expected a DO in WHILE condition");

        comando();
    }
    //epsilon
}

void mais_cmd(){
    SyncTokens parent_tokens = {2, (char*[]){"keyword_end", "semicolon"}};
    SyncTokens immediate_tokens = {0, NULL};

    if(!strcmp(current_token->type, "semicolon")){
        immediate_tokens = (SyncTokens){5, (char*[]){"keyword_call", "keyword_begin", "keyword_if", "identifier", "keyword_while"}};
        MATCH(FIELD_TYPE, "semicolon", "Missing ';");

        comando();

        mais_cmd();
    }
    //epsilon
}

void expressao(){
    operador_unario();
    termo();
    mais_termos();
}

void operador_unario(){
    SyncTokens parent_tokens = {5, (char*[]){"identifier", "integer_literal", "left_par"}};
    SyncTokens immediate_tokens = {0, NULL};
    
    if(!strcmp(current_token->type, "simb_plus")){
        MATCH(FIELD_TYPE, "simb_plus", "Expected '+'");
    }else if(!strcmp(current_token->type, "simb_minus")){
        MATCH(FIELD_TYPE, "simb_minus", "Expected '-'");
    }
    //epsilon
}

void termo(){
    fator();
    mais_fatores();
}

void mais_termos(){
    SyncTokens parent_tokens = {7, (char*[]){"semicolon", "keyword_end", "period", "right_par", "rel_op", "keyword_then", "keyword_do"}};
    SyncTokens immediate_tokens = {0, NULL};

    if(!strcmp(current_token->type, "simb_plus")){
        immediate_tokens = (SyncTokens){3, (char*[]){"identifier", "integer_literal", "left_par"}};
        MATCH(FIELD_TYPE, "simb_plus", "Expected '+'");

        termo();
        
        mais_termos();
    } else if(!strcmp(current_token->type, "simb_minus")){
        immediate_tokens = (SyncTokens){3, (char*[]){"identifier", "integer_literal", "left_par"}};
        MATCH(FIELD_TYPE, "simb_minus", "Expected '-'");

        termo();
        mais_termos();
    }
    //epsilon
}

void fator(){
    SyncTokens parent_tokens = {11, (char*[]){"simb_mult", "simb_div", "simb_minus", "simb_plus", "semicolon", "keyword_end", "period", "right_par", "rel_op", "keyword_then", "keyword_do"}};
    SyncTokens immediate_tokens = {0, NULL};

    if(!strcmp(current_token->type, "identifier")){
        MATCH(FIELD_TYPE, "identifier", "Expected identifier in factor");
    }else if(!strcmp(current_token->type, "integer_literal")){
        MATCH(FIELD_TYPE, "integer_literal", "Expected number in factor");
    }else if(!strcmp(current_token->type, "left_par")){
        immediate_tokens = (SyncTokens){5, (char*[]){"identifier", "integer_literal", "left_par", "simb_plus", "simb_minus"}};
        MATCH(FIELD_TYPE, "left_par", "Expected opening parenthesis in expression");

        expressao();

        immediate_tokens = (SyncTokens){0, NULL};
        MATCH(FIELD_TYPE, "right_par", "Missing closing parenthesis in expression");
    }else{
        add_error(&error_list, "Expected identifier, number or expression", *current_token);
    }
}

void mais_fatores(){
    SyncTokens parent_tokens = {9, (char*[]){"simb_minus", "simb_plus", "semicolon", "keyword_end", "period", "right_par", "rel_op", "keyword_then", "keyword_do"}};
    SyncTokens immediate_tokens = {0, NULL};
    if(!strcmp(current_token->type, "simb_mult")){
        immediate_tokens = (SyncTokens){5, (char*[]){"identifier", "integer_literal", "left_par", "simb_plus", "simb_minus"}};
        MATCH(FIELD_TYPE, "simb_mult", "Expected multiply operator");
        fator();
        mais_fatores();
    }else if(!strcmp(current_token->type, "simb_div")){
        immediate_tokens = (SyncTokens){5, (char*[]){"identifier", "integer_literal", "left_par", "simb_plus", "simb_minus"}};
        MATCH(FIELD_TYPE, "simb_div", "Expected divide operator");
        fator();
        mais_fatores();
    }
    //epsilon
}

void condicao(){
    SyncTokens parent_tokens = {3, (char*[]){"keyword_then", "keyword_do"}};
    SyncTokens immediate_tokens = {0, NULL};
    if(!strcmp(current_token->type, "keyword_odd")){
        MATCH(FIELD_TYPE, "keyword_odd", "Expected ODD keyword");
    }else{
        expressao();
        relacional();
        expressao();
    }
    //epsilon
}

void relacional(){
    SyncTokens parent_tokens = {3, (char*[]){"simb_plus", "simb_minus", "identifier", "integer_literal","left_par"}};
    SyncTokens immediate_tokens = {0, NULL};
    if(!strcmp(current_token->type, "rel_op")){
        MATCH(FIELD_TYPE, "rel_op", "Expected a relational operator");
    }else{
        add_error(&error_list, "Expected a relational operator", *current_token);
    }
}


