#include <stdlib.h>
#include <stdint.h>

#include "lexer.h"
#include "IO_utils.h"
#include "str_utils.h"

static const char* restrict first_line_state_dsv = "Name|Type|Output";
static const char* restrict first_line_transitions_dsv = "CurrentState|Input|NextState";

static const char* restrict states_file = "res/state_map.csv";
static const char* restrict transitions_file = "res/transitions.csv";

size_t hash_map(char* key, size_t hash_size) {
    size_t index = 0;
    size_t pow = 1;

    while (*key != '\0') {
        index += (*key)*pow;
        pow *= PRIME;
        key++;
    }

    return index%hash_size;
}

void insertState(StateMachine* sm, State* state) {
    size_t index = hash_map(state->stateName, sm->hash_size);

    // Linear probing for collision resolution
    while (sm->states_hash[index].stateName != NULL) {
        index = (index + 1)%sm->hash_size;
    }

    sm->states_hash[index] = *state;
    sm->stateCount++;
}

State* getState(StateMachine* sm, char* key) {
    size_t index = hash_map(key, sm->hash_size);

    // Linear probing for collision resolution
    while (sm->states_hash[index].stateName != NULL) {
        if (strcmp(sm->states_hash[index].stateName, key) == 0) {
            return &sm->states_hash[index];
        }

        index = (index + 1)%sm->hash_size;
    }

    return NULL; // Not found
}

void resize_hash(StateMachine* sm) {
    size_t new_hash_size = sm->hash_size*HASH_GROWTH_FACTOR;
    State* new_states_hash;
    XCALLOC(State, new_states_hash, new_hash_size)

    // Rehash all existing states
    for (size_t i = 0; i < sm->hash_size; i++) {
        if (sm->states_hash[i].stateName != NULL) {
            char* key = sm->states_hash[i].stateName;
            size_t index = hash_map(key, new_hash_size);

            while (new_states_hash[index].stateName != NULL) {
                index = (index + 1)%new_hash_size;
            }

            new_states_hash[index] = sm->states_hash[i];
        }
    }

    // Free old hash table and update StateMachine
    free(sm->states_hash);
    sm->states_hash = new_states_hash;
    sm->hash_size = new_hash_size;
}

void loadStates(const char* restrict filename, StateMachine* sm) {
    FILE* file;
    OPEN_FILE(file, filename, "r")

    char* line;
    readLine(&line, file);
    if (strcmp(line, first_line_state_dsv) != 0) {
        ABORT_PROGRAM("State DSV %s has no header. The first line must be \"Name|Type|Output\".", filename)
    }

    free(line);
    while (!feof(file)) {
        readLine(&line, file);
        int num_fields;
        char** fields = split(line, "|", &num_fields);
        if (num_fields < 2 || num_fields > 3) {
            ABORT_PROGRAM("Malformed line on %s: %s\n"
                          "DSV must have 2 or 3 fields per line."
                          " The third one is optional and contains the state's output.", 
                          filename, line)
        }

        StateType cur_type = atoi(fields[1]);
        if (STATE_HAS_EMPTY_OUTPUT(cur_type)) {

        } else if (STATE_HAS_NON_EMPTY_OUTPUT(cur_type)) {

        } else {
            ABORT_PROGRAM("Malformed line on %s: %s\n"
                          "the second field must be a mapped state type, "
                          "a number from 0 to 3", filename, line)
        }
    }
}


void loadTransitions(const char* restrict filename, StateMachine* sm) {
    FILE* file;
    OPEN_FILE(file, filename, "r")

    char* line;
    char** tokens;
    int count;

    do {
        readLine(&line, file);
        tokens = split(line, "|", &count);
        if (count != NUM_FIELDS_CSV) {
            ABORT_PROGRAM("Malformed line: %s\n"
                          "DSV must have exactly 3 fields per line,"
                          " separated by spaces", line);
        }

        // Add or update state
        State* state = NULL;
        for (size_t i = 0; i < sm->stateCount; i++) {
            if (strcmp(sm->states_hash[i].stateName, tokens[0]) == 0) {
                state = &sm->states_hash[i];
                break;
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
        if (strcmp(sm->states_hash[i].stateName, name) == 0) {
            return &sm->states_hash[i];
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

void init_hash(StateMachine* sm) {
    XCALLOC(State, sm->states_hash, INIT_TABLE_SIZE)
    sm->hash_size = INIT_TABLE_SIZE;
}

void initializeStateMachine(StateMachine* sm) {
    sm->states_hash = NULL;
    sm->stateCount = 0;

    init_hash(sm);
    loadStates(states_file, sm);
    loadTransitions(transitions_file, sm);
}

void freeStateMachine(StateMachine* sm) {
    for (size_t i = 0; i < sm->stateCount; i++) {
        for (size_t j = 0; j < (sm->states_hash)[i].transitionCount; j++) {
            free((sm->states_hash)[i].transitions[j].input);
            free((sm->states_hash)[i].transitions[j].nextState);
        }

        free((sm->states_hash)[i].transitions);
        free((sm->states_hash)[i].stateName);
    }

    free(sm->states_hash);
}

TokStream* token_stream_init(const char* restrict source_path) {
    TokStream* tok_stream = NULL;

    XCALLOC(TokStream, tok_stream, 1)
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