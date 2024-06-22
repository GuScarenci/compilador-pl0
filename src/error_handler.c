#include "error_handler.h"

static size_t error_count = 0;

char* rea_line_from_file(const char* filename, size_t line_number) {
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

void prin_line_with_highlight(FILE* out_fp, const char* line, size_t start_char, size_t length) {
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
    char* line = rea_line_from_file(source_path, error_token.line);
    if (line) {
        prin_line_with_highlight(out_file, line, error_token.first_char_pos, error_token.size);
        free(line);
    }
}

void print_final_message(FILE* out_file){
    if (error_count == 0){
        fprintf(out_file, ANSI_COLOR_GREEN);
        fprintf(out_file, "Compilation successful\n");
        fprintf(out_file, ANSI_COLOR_RESET);
    } else {
        fprintf(out_file, ANSI_COLOR_RED);
        fprintf(out_file, "Compilation failed with %ld errors\n", error_count);
        fprintf(out_file, ANSI_COLOR_RESET);
    }
}
