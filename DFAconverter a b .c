#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define NUM_STATES 4
#define ALPHABET_SIZE 2

const int transition_table[NUM_STATES][ALPHABET_SIZE] = {
    {1,0},
    {1,2},
    {1,3},
    {1,0}
};

const bool is_final[NUM_STATES] = {
    false,
    false,
    false,
    true
};

void print_transition_table() {
    printf("DFA Transition Table for Regular Expression: (a|b)*abb\n");
    printf("====================================================\n");
    printf("| State | 'a' (0) | 'b' (1) | Final? |\n");
    printf("|-------|---------|---------|--------|\n");

    for (int i = 0; i < NUM_STATES; i++) {
        printf("| S%d%s   | %7d | %7d | %s |\n",
               i,
               (i == 0 ? " (Init)" : "      "),
               transition_table[i][0],
               transition_table[i][1],
               (is_final[i] ? "YES" : "NO "));
    }
    printf("====================================================\n\n");
}

bool check_string_against_dfa(const char *input) {
    int current_state = 0;

    printf("Simulating string: \"%s\"\n", input);
    printf("Start State: S%d\n", current_state);

    for (int i = 0; i < strlen(input); i++) {
        char symbol = input[i];
        int input_index;

        if (symbol == 'a') {
            input_index = 0;
        } else if (symbol == 'b') {
            input_index = 1;
        } else {
            printf("Error: Invalid symbol '%c' encountered.\n", symbol);
            return false;
        }

        int next_state = transition_table[current_state][input_index];
        printf("Read '%c' (S%d -> S%d)\n", symbol, current_state, next_state);
        current_state = next_state;
    }

    bool accepted = is_final[current_state];

    printf("End State: S%d. Result: %s\n", current_state, accepted ?"ACCEPTED" : "REJECTED");
    return accepted;
}

int main() {
    print_transition_table();

    check_string_against_dfa("ababb");
    printf("\n");
    check_string_against_dfa("aaba");
    printf("\n");
    check_string_against_dfa("abb");
    printf("\n");
    check_string_against_dfa("bba");

    return 0;
}
