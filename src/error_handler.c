/************************************************************
*             Trabalho 2 - SCC0605                          *
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
*      Data de última atualizacao: 22/06/2024               *
*      Ambiente: VSCode 1.89.1                              *
*                                                           *
*           Arquivo error_handler.c                         *
************************************************************/
#include "error_handler.h"

static size_t error_count = 0;
static size_t warning_count = 0;

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

void print_line_with_highlight(FILE* out_fp, const char* line, size_t start_char, size_t length, char *color) {
    size_t line_len = strlen(line);
    for (size_t i = 0; i < line_len; i++) {
        if (i == start_char) {
            fprintf(out_fp, "%s", color);
        }
        if (i == start_char + length) {
            fprintf(out_fp, ANSI_COLOR_RESET);
        }
        fputc(line[i], out_fp);
    }
    fprintf(out_fp, ANSI_COLOR_RESET);
}

void print_message(FILE *out_file, char* error_message, char *color){
    error_count++;
    fprintf(out_file, "%s", color);
    fprintf(out_file, "%s\n", error_message);
    fprintf(out_file, ANSI_COLOR_RESET);
}

void print_error(FILE *out_file, char* error_message, Token error_token){
    error_count++;
    fprintf(out_file, "%s:%ld:%ld:", error_token.source_path, error_token.line, error_token.first_char_pos);
    fprintf(out_file, ANSI_COLOR_RED);
    fprintf(out_file, " error: ");
    fprintf(out_file, ANSI_COLOR_RESET);
    fprintf(out_file, "%s\n", error_message);

    fprintf(out_file, ANSI_COLOR_RESET);
    fprintf(out_file, "\t%ld | ", error_token.line);
    char *source_path = error_token.source_path;
    char* line = read_line_from_file(source_path, error_token.line);
    if (line) {
        print_line_with_highlight(out_file, line, error_token.first_char_pos, error_token.size, ANSI_COLOR_RED);
        free(line);
    }
}

void print_warning(FILE *out_file, char *warning_message, Token warning_token){
    warning_count++;
    fprintf(out_file, "%s:%ld:%ld:", warning_token.source_path, warning_token.line, warning_token.first_char_pos);
    fprintf(out_file, ANSI_COLOR_YELLOW);
    fprintf(out_file, " warning: ");
    fprintf(out_file, ANSI_COLOR_RESET);
    fprintf(out_file, "%s\n", warning_message);

    fprintf(out_file, ANSI_COLOR_RESET);
    fprintf(out_file, "\t%ld | ", warning_token.line);
    char *source_path = warning_token.source_path;
    char* line = read_line_from_file(source_path, warning_token.line);
    if (line) {
        print_line_with_highlight(out_file, line, warning_token.first_char_pos, warning_token.size, ANSI_COLOR_YELLOW);
        free(line);
    }
}

void print_final_message(FILE* out_file){
    if (error_count == 0){
        fprintf(out_file, ANSI_COLOR_GREEN);
        fprintf(out_file, "\nCompilation successful ");
        fprintf(out_file, ANSI_COLOR_RESET);
        if(warning_count == 0){
            fprintf(out_file, "\n");
        } else {
            fprintf(out_file, ANSI_COLOR_YELLOW);
            fprintf(out_file, "with %ld warnings\n", warning_count);
            fprintf(out_file, ANSI_COLOR_RESET);
        }
    } else {
        fprintf(out_file, ANSI_COLOR_RESET);
        fprintf(out_file, "\nCompilation failed with ");
        fprintf(out_file, ANSI_COLOR_RED);
        fprintf(out_file, "%ld errors", error_count);
        fprintf(out_file, ANSI_COLOR_RESET);

        if(warning_count == 0){
            fprintf(out_file, "!\n");
        } else {
            fprintf(out_file, ANSI_COLOR_RESET);
            fprintf(out_file, " and ");
            fprintf(out_file, ANSI_COLOR_YELLOW);
            fprintf(out_file, "%ld warnings\n", warning_count);
        }
    }
}
