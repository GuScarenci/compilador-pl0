#include <stdlib.h>
#include <stdint.h>

#include "lexer.h"
#include "IO_utils.h"
#include "str_utils.h"


void loadTransitions(const char* filename, StateMachine* sm) {
    FILE* file;
    OPEN_FILE(file, filename, "r")

    char* line;
    char** tokens;
    int count;

    do {
        readLine(&line, file);
        tokens = split(line, " ", &count);
        if (count != NUM_FIELDS_CSV) {
            ABORT_PROGRAM("Malformed line: %s\n"
                          "DSV must have exactly 3 fields per line,"
                          " separated by spaces", line);
        }

        // Add or update state
        State* state = NULL;
        for (size_t i = 0; i < sm->stateCount; i++) {
            if (strcmp(sm->states[i].stateName, tokens[0]) == 0) {
                state = &sm->states[i];
                break;
            }
        }

        if (state == NULL) {
            XREALLOC(State, sm->states, ++sm->stateCount)

            state = &sm->states[sm->stateCount - 1];
            state->stateName = strdup(tokens[0]);
            state->transitions = NULL;
            state->transitionCount = 0;

            if (tokens[0][0] == 'F' || tokens[0][0] == 'E') {
                state->isFinal = 1;
            } else {
                state->isFinal = 0;
            }
        }

        XREALLOC(StateTransition, state->transitions, ++state->transitionCount)
        state->transitions[state->transitionCount - 1].input = strdup(tokens[1]);
        state->transitions[state->transitionCount - 1].nextState = strdup(tokens[2]);

        free(line);
    } while (!feof(file));

    fclose(file);
}

State* findStateByName(StateMachine* sm, char* name) {
    for (size_t i = 0; i < sm->stateCount; i++) {
        if (strcmp(sm->states[i].stateName, name) == 0) {
            return &sm->states[i];
        }
    }

    return NULL;
}

char* getNextState(State* currentState, char input) {
    for (size_t i = 0; i < currentState->transitionCount; i++) {
        if ((strcmp(currentState->transitions[i].input, "Digito") == 0 && isDigit(input)) ||
            (strcmp(currentState->transitions[i].input, "Nao-Digito") == 0 && isNonDigit(input)) ||
            (strlen(currentState->transitions[i].input) == 1 && currentState->transitions[i].input[0] == input)) {
            return currentState->transitions[i].nextState;
        }
    }

    return NULL;
}

void runStateMachine(StateMachine* sm, char* currentStateName, char* inputString) {
    State* currentState = findStateByName(sm, currentStateName);
    if (currentState == NULL) {
        printf("Invalid initial state\n");
        return;
    }

    int inputLength = strlen(inputString);
    for (int i = 0; i < inputLength; i++) {
        printf("Current State: %s, Input: %c\n", currentState->stateName, inputString[i]);
        char* nextStateName = getNextState(currentState, inputString[i]);

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

void initializeStateMachine(StateMachine* sm) {
    sm->states = NULL;
    sm->stateCount = 0;
    loadTransitions("res/transitions.csv", sm);
}

void freeStateMachine(StateMachine* sm) {
    for (size_t i = 0; i < sm->stateCount; i++) {
        for (size_t j = 0; j < (sm->states)[i].transitionCount; j++) {
            free((sm->states)[i].transitions[j].input);
            free((sm->states)[i].transitions[j].nextState);
        }

        free((sm->states)[i].transitions);
        free((sm->states)[i].stateName);
    }

    free(sm->states);
}

TokStream* token_stream_init(const char* restrict source_path) {
    TokStream* tok_stream = NULL;

    XALLOC(TokStream, tok_stream, 1)
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