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

#define MAX_LINE_LENGTH 1024

typedef struct {
    char *input;
    char *nextState;
} StateTransition;

typedef struct {
    char *stateName;
    StateTransition *transitions;
    int transitionCount;
    int isFinal;
} State;

typedef struct {
    State *states;
    int stateCount;
} StateMachine;

// Function to trim newline characters from a string
char* trimNewline(char *str) {
    char *pos;
    if ((pos = strchr(str, '\n')) != NULL) {
        *pos = '\0';
    }
    if ((pos = strchr(str, '\r')) != NULL) {
        *pos = '\0';
    }
    return str;
}

// Function to split a line into tokens by a delimiter
char** split(char *line, const char *delimiter, int *count) {
    char **tokens = NULL;
    char *token = strtok(line, delimiter);
    int tokensCount = 0;

    while (token) {
        tokens = realloc(tokens, sizeof(char*) * ++tokensCount);
        if (tokens == NULL) {
            perror("realloc");
            exit(EXIT_FAILURE);
        }
        tokens[tokensCount - 1] = token;
        token = strtok(NULL, delimiter);
    }
    *count = tokensCount;
    return tokens;
}

int isDigit(char c) {
    return c >= '0' && c <= '9';
}

int isNonDigit(char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_';
}

void loadTransitions(const char *filename, StateMachine *sm) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE_LENGTH];
    char **tokens;
    int count;

    // Read the header line
    if (fgets(line, sizeof(line), file) == NULL) {
        perror("fgets");
        exit(EXIT_FAILURE);
    }

    while (fgets(line, sizeof(line), file)) {
        trimNewline(line);
        tokens = split(line, " ", &count);
        if (count != 3) {
            fprintf(stderr, "Malformed line: %s\n", line);
            exit(EXIT_FAILURE);
        }

        // Add or update state
        State *state = NULL;
        for (int i = 0; i < sm->stateCount; i++) {
            if (strcmp(sm->states[i].stateName, tokens[0]) == 0) {
                state = &sm->states[i];
                break;
            }
        }

        if (state == NULL) {
            sm->states = realloc(sm->states, sizeof(State) * ++sm->stateCount);
            if (sm->states == NULL) {
                perror("realloc");
                exit(EXIT_FAILURE);
            }
            state = &sm->states[sm->stateCount - 1];
            state->stateName = strdup(tokens[0]);
            state->transitions = NULL;
            state->transitionCount = 0;

            if (tokens[0][0] == 'O' || tokens[0][0] == 'P'){
                state->isFinal = 1;
            } else {
                state->isFinal = 0;
            }
        }

        state->transitions = realloc(state->transitions, sizeof(StateTransition) * ++state->transitionCount);
        if (state->transitions == NULL) {
            perror("realloc");
            exit(EXIT_FAILURE);
        }
        state->transitions[state->transitionCount - 1].input = strdup(tokens[1]);
        state->transitions[state->transitionCount - 1].nextState = strdup(tokens[2]);
    }

    fclose(file);
}

State* findStateByName(StateMachine *sm, char *name) {
    for (int i = 0; i < sm->stateCount; i++) {
        if (strcmp(sm->states[i].stateName, name) == 0) {
            return &sm->states[i];
        }
    }
    return NULL;
}

char* getNextState(State *currentState, char input) {
    for (int i = 0; i < currentState->transitionCount; i++) {
        if ((strcmp(currentState->transitions[i].input, "Digito") == 0 && isDigit(input)) ||
            (strcmp(currentState->transitions[i].input, "Nao-Digito") == 0 && isNonDigit(input)) ||
            (strlen(currentState->transitions[i].input) == 1 && currentState->transitions[i].input[0] == input)) {
            return currentState->transitions[i].nextState;
        }
    }
    return NULL;
}

void runStateMachine(StateMachine *sm, char *currentStateName, char *inputString) {
    State *currentState = findStateByName(sm, currentStateName);
    if (currentState == NULL) {
        printf("Invalid initial state\n");
        return;
    }

    int inputLength = strlen(inputString);
    for (int i = 0; i < inputLength; i++) {
        printf("Current State: %s, Input: %c\n", currentState->stateName, inputString[i]);
        char *nextStateName = getNextState(currentState, inputString[i]);
        if (nextStateName == NULL) {
            printf("Invalid input '%c' from state '%s'\n", inputString[i], currentState->stateName);
            return;
        }

        currentState = findStateByName(sm, nextStateName);
        if (currentState == NULL) {
            printf("Next state '%s' not found\n", nextStateName);
            return;
        }
    }

    printf("Final State: %s\n", currentState->stateName);
}

StateMachine sm;
void initializeStateMachine() {
    sm.states = NULL;
    sm.stateCount = 0;
    loadTransitions("res/transitions.csv", &sm);
}

void freeStateMachine() {
    for (int i = 0; i < sm.stateCount; i++) {
        for (int j = 0; j < sm.states[i].transitionCount; j++) {
            free(sm.states[i].transitions[j].input);
            free(sm.states[i].transitions[j].nextState);
        }
        free(sm.states[i].transitions);
        free(sm.states[i].stateName);
    }
    free(sm.states);
}

Token* get_next_token(TokenStream* tok_stream) {
    if (tok_stream == NULL) {
        ABORT_PROGRAM("Cannot fetch token from vacuous stream");
    }

    // TODO
    // Implementar máquina de estados aqui


    return NULL;
}