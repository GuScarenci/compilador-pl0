#ifndef __STATE_MACHINE_H__
#define __STATE_MACHINE_H__

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define NUM_FIELDS_TRANSITION_CSV 4
#define GO_RIGHT 1
#define GO_LEFT -1

#define HASH_GROWTH_FACTOR 2
#define PRIME 37
#define INIT_TABLE_SIZE 256

#define STATE_HAS_EMPTY_OUTPUT(type) (type == initial || type == regular)
#define STATE_HAS_NON_EMPTY_OUTPUT(type) (type == error || type == final)

typedef enum state_type {
    initial = 0,
    error = 1,
    final = 2,
    regular = 3
} StateType;

typedef struct transition_t {
    char* input;
    char* nextState;
    int8_t shift;
} StateTransition;

typedef struct state_t {
    StateTransition* transitions;
    size_t transitionCount;
    char* stateName;
    StateType type;
    char* output;
} State;

typedef struct two_way_moore_machine_t {
    State* states_hash;
    size_t stateCount;
    size_t hash_size;
} StateMachine;

/*
 * Initializes the state machine hash table.
 */
void init_hash(StateMachine* sm);

/*
 * Initializes the state machine.
 */
void initializeStateMachine(StateMachine* sm);

/*
 * Frees resources utilized by the state machine.
 */
void freeStateMachine(StateMachine* sm);

/*
 * Inserts a state into the state machine.
 */
void insertState(StateMachine* sm, State* state);

/*
 * Retrieves a state from the state machine by name.
 */
State* getState(StateMachine* sm, char* key);

/*
 * Loads states from a CSV file.
 */
void loadStates(const char* restrict filename, StateMachine* sm);

/*
 * Loads transitions from a CSV file.
 */
void loadTransitions(const char* restrict filename, StateMachine* sm);

/*
 * Gets the next state based on the current state and input.
 */
char* getNextState(State* currentState, char input);

#endif
