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
*           Arquivo state_machine.c                         *
************************************************************/

#include "state_machine.h"
#include "IO_utils.h"
#include "str_utils.h"

static const char* restrict first_line_state_dsv = "Name|Type|Output";
static const char* restrict first_line_transitions_dsv = "CurrentState|Input|NextState|Direction";

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

void loadStates(const char* restrict filename, StateMachine* sm) { //TODO IMPLEMENT
    FILE* file;
    OPEN_FILE(file, filename, "r")

    char* line;
    readLine(&line, file);
    if (strcmp(line, first_line_state_dsv) != 0) {
        ABORT_PROGRAM("State DSV %s has no header. The first line must be \"%s\".", 
                      filename, first_line_state_dsv)
    }

    free(line);
    while (!feof(file)) {
        readLine(&line, file);
        size_t num_fields;
        char** fields = split(line, "|", &num_fields);
        if (num_fields < 2 || num_fields > 3) {
            ABORT_PROGRAM("Malformed line on %s: %s\n"
                          "DSV must have 2 or 3 fields per line."
                          " The third one is optional and contains the state's output.", 
                          filename, line)
        }

        StateType cur_type = atoi(fields[1]);
        State new_state;
        new_state.type = cur_type;
        new_state.transitions = NULL;
        new_state.transitionCount = 0;
        new_state.output = NULL;
        
        if (STATE_HAS_EMPTY_OUTPUT(cur_type)) {
            new_state.stateName = strdup(fields[0]);
            insertState(sm, &new_state);
        } else if (STATE_HAS_NON_EMPTY_OUTPUT(cur_type)) {
            new_state.stateName = strdup(fields[0]);
            new_state.output = strdup(fields[2]);
            insertState(sm, &new_state);
        } else {
            ABORT_PROGRAM("Malformed line on %s: %s\n"
                          "the second field must be a mapped state type, "
                          "a number from 0 to 4", filename, line)
        }

        if (new_state.type == initial) {
            if(sm->initial_state != NULL) {
                ABORT_PROGRAM("There can only be one initial state")
            }
            State *initial_state = getState(sm, new_state.stateName);
            sm->initial_state = initial_state;
            sm->current_state = initial_state;
        }

        free(line);
        free(*fields);
        free(fields);
    }

    fclose(file);
}

void loadTransitions(const char* restrict filename, StateMachine* sm) { //TODO IMPLEMENT
    FILE* file;
    OPEN_FILE(file, filename, "r")

    char* line;
    char** fields;
    size_t count;

    readLine(&line, file);
    if (strcmp(line, first_line_transitions_dsv) != 0) {
        ABORT_PROGRAM("Transitions DSV %s has no header. The first line must be \"%s\".", 
                      filename, first_line_transitions_dsv)
    }
    free(line);

    while (!feof(file)) {
        readLine(&line, file);
        fields = split(line, "|", &count);
        if (count != NUM_FIELDS_TRANSITION_CSV) {
            ABORT_PROGRAM("Ill-formed line: %s\n"
                          "DSV must have exactly %d fields per line,"
                          " separated by spaces", line, NUM_FIELDS_TRANSITION_CSV);
        }

        // Add or update state
        State* state = getState(sm, fields[0]);
        XREALLOC(StateTransition, state->transitions, ++state->transitionCount)
        state->transitions[state->transitionCount - 1].input = strdup(fields[1]);
        state->transitions[state->transitionCount - 1].nextState = strdup(fields[2]);

        if (strcmp(fields[3], "F") == 0) {
            state->transitions[state->transitionCount - 1].shift = GO_FOWARD;
        } else if (strcmp(fields[3], "B") == 0) {
            state->transitions[state->transitionCount - 1].shift = GO_BACK;
        } else {
            ABORT_PROGRAM("Ill-formed line: %s\nLast field must be \"F\" or \"B\": "
                          "whether the head moves left or right on the tape")
        }

        free(*fields);
        free(fields);
        free(line);
    }

    fclose(file);
}

StateTransition* getNextState(State* currentState, char input) { //TODO IMPLEMENT
    for (size_t i = 0; i < currentState->transitionCount; i++) {
        StateTransition* transition = &currentState->transitions[i];

        int is_digit = (strcmp(transition->input, "digito") == 0 && isDigit(input));
        int is_symbol = (strcmp(transition->input, "simbolo") == 0 && isSymbol(input));
        int is_letter = (strcmp(transition->input, "letra") == 0 && isLetter(input));
        int is_whitespace = (strcmp(transition->input, "branco") == 0 && isWhitespace(input));
        int is_newline = (strcmp(transition->input, "quebra-linha") == 0 && '\n' == input);
        int is_single_char = (strlen(transition->input) == 1 && transition->input[0] == input);

        if (is_digit || is_symbol || is_letter || is_whitespace || is_newline || is_single_char) {
            return transition;
        }
    }

    // If no matching transition is found, check for "Outro"
    for (size_t i = 0; i < currentState->transitionCount; i++) {
        if (strcmp(currentState->transitions[i].input, "outro") == 0) {
            return &(currentState->transitions[i]);
        }
    }

    return NULL; // No valid transition found
}

void init_hash(StateMachine* sm) {
    XCALLOC(State, sm->states_hash, INIT_TABLE_SIZE)
    sm->hash_size = INIT_TABLE_SIZE;
}

void initializeStateMachine(StateMachine* sm) {
    sm->states_hash = NULL;
    sm->stateCount = 0;

    //XALLOC(State, sm->initial_state, 1)
    init_hash(sm);

    loadStates(states_file, sm);
    loadTransitions(transitions_file, sm);
}

void freeStateMachine(StateMachine* sm) {
    for (size_t i = 0; i < sm->hash_size; i++) {
        for (size_t j = 0; j < (sm->states_hash)[i].transitionCount; j++) {
            free((sm->states_hash)[i].transitions[j].input);
            free((sm->states_hash)[i].transitions[j].nextState);
        }

        free((sm->states_hash)[i].transitions);
        free((sm->states_hash)[i].stateName);
        free((sm->states_hash)[i].output);
    }

    free(sm->states_hash);
}