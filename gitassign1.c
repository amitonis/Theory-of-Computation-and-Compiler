#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_INSTRUCTIONS 20
#define VAR_LEN 10

typedef struct {
    char result[VAR_LEN];
    char op1[VAR_LEN];
    char op2[VAR_LEN];
    char op; 
    int active; 
} Instruction;

Instruction code[MAX_INSTRUCTIONS];
int NUM_INSTRUCTIONS = 0; 

void print_code(const char *title);
void constant_folding();
void strength_reduction();
void copy_propagation_and_dead_code_elimination();
void read_input_code();

int is_numeric(const char *s) {
    if (!s || *s == '\0') return 0;
    
    int i = 0;
    if (s[0] == '-' || s[0] == '+') {
        i = 1;
    }
    
    if (s[i] == '\0') return 0; 
    
    while (s[i]) {
        if (!isdigit((unsigned char)s[i])) return 0;
        i++;
    }
    return 1;
}

void read_input_code() {
    char line[100];
    char res[VAR_LEN], op1[VAR_LEN], op2[VAR_LEN];
    char eq_sign[5];
    char operator_char[5];
    int count = 0;

    printf("--- Enter Three-Address Code Instructions (Up to %d max) ---\n", MAX_INSTRUCTIONS);
    printf("Format 1 (Binary Op): result = op1 op op2 (e.g., x = a + b)\n");
    printf("Format 2 (Assignment): result = op1 (e.g., y = x)\n");
    printf("Type 'END' and press Enter to finish input.\n\n");

    while (count < MAX_INSTRUCTIONS) {
        printf("Instruction %d: ", count + 1);
        if (fgets(line, sizeof(line), stdin) == NULL) break;

        line[strcspn(line, "\n")] = 0;

        if (strcmp(line, "END") == 0) break;
        if (line[0] == '\0') continue;
        
        int scanned = sscanf(line, "%10s %5s %10s %5s %10s", res, eq_sign, op1, operator_char, op2);

        if (scanned == 5 && strcmp(eq_sign, "=") == 0 && (operator_char[0] == '+' || operator_char[0] == '-' || operator_char[0] == '*' || operator_char[0] == '/')) {
            strncpy(code[count].result, res, VAR_LEN - 1);
            strncpy(code[count].op1, op1, VAR_LEN - 1);
            strncpy(code[count].op2, op2, VAR_LEN - 1);
            code[count].op = operator_char[0];
            code[count].active = 1;
            count++;
        } 
        else {
            scanned = sscanf(line, "%10s %5s %10s", res, eq_sign, op1);
            
            if (scanned == 3 && strcmp(eq_sign, "=") == 0) {
                strncpy(code[count].result, res, VAR_LEN - 1);
                strncpy(code[count].op1, op1, VAR_LEN - 1);
                code[count].op2[0] = '\0';
                code[count].op = ' '; 
                code[count].active = 1;
                count++;
            } 
            else {
                printf("Error: Invalid instruction format. Please re-enter.\n");
            }
        }
    }
    
    NUM_INSTRUCTIONS = count;
    printf("\nRead %d instructions.\n\n", NUM_INSTRUCTIONS);
}

void constant_folding() {
    for (int i = 0; i < NUM_INSTRUCTIONS; i++) {
        if (!code[i].active) continue;

        if (is_numeric(code[i].op1) && is_numeric(code[i].op2) && code[i].op != ' ') {
            int val1 = atoi(code[i].op1);
            int val2 = atoi(code[i].op2);
            int result_val;
            
            switch (code[i].op) {
                case '*': result_val = val1 * val2; break;
                case '+': result_val = val1 + val2; break;
                case '-': result_val = val1 - val2; break;
                case '/': 
                    if (val2 != 0) { result_val = val1 / val2; } else continue;
                    break;
                default: continue;
            }

            sprintf(code[i].op1, "%d", result_val);
            code[i].op2[0] = '\0';
            code[i].op = ' ';
        }
    }
}

void strength_reduction() {
    for (int i = 0; i < NUM_INSTRUCTIONS; i++) {
        if (!code[i].active || code[i].op == ' ') continue;

        if (code[i].op == '*' && (strcmp(code[i].op1, "1") == 0 || strcmp(code[i].op2, "1") == 0)) {
            char *source = (strcmp(code[i].op1, "1") == 0) ? code[i].op2 : code[i].op1;
            strcpy(code[i].op1, source);
            code[i].op2[0] = '\0';
            code[i].op = ' ';
            continue;
        }

        if (code[i].op == '+' && (strcmp(code[i].op1, "0") == 0 || strcmp(code[i].op2, "0") == 0)) {
            char *source = (strcmp(code[i].op1, "0") == 0) ? code[i].op2 : code[i].op1;
            strcpy(code[i].op1, source);
            code[i].op2[0] = '\0';
            code[i].op = ' ';
            continue;
        }

        if (code[i].op == '-' && strcmp(code[i].op2, "0") == 0) {
            code[i].op2[0] = '\0';
            code[i].op = ' ';
            continue;
        }
    }
}

void copy_propagation_and_dead_code_elimination() {
    for (int i = 0; i < NUM_INSTRUCTIONS; i++) {
        if (code[i].active && code[i].op == ' ') {
            char *source_var = code[i].op1;
            char *target_var = code[i].result;

            for (int j = i + 1; j < NUM_INSTRUCTIONS; j++) {
                if (!code[j].active) continue;

                if (strcmp(code[j].op1, target_var) == 0) {
                    strcpy(code[j].op1, source_var);
                }
                if (strcmp(code[j].op2, target_var) == 0) {
                    strcpy(code[j].op2, source_var);
                }
            }
        }
    }

    for (int i = 0; i < NUM_INSTRUCTIONS; i++) {
        if (!code[i].active) continue;
        
        char *defined_var = code[i].result;
        int is_used = 0;
        
        for (int j = i + 1; j < NUM_INSTRUCTIONS; j++) {
            if (!code[j].active) continue;
            if (strcmp(code[j].op1, defined_var) == 0 || strcmp(code[j].op2, defined_var) == 0) {
                is_used = 1;
                break;
            }
        }
        
        if (i == NUM_INSTRUCTIONS - 1) {
            is_used = 1; 
        }

        if (is_used == 0) {
            code[i].active = 0; 
        }
    }
}

void print_code(const char *title) {
    if (NUM_INSTRUCTIONS == 0) {
        printf("%s: (No instructions)\n", title);
        return;
    }
    printf("%s:\n", title);
    printf("---------------------------\n");
    int active_count = 0;
    for (int i = 0; i < NUM_INSTRUCTIONS; i++) {
        if (code[i].active) {
            active_count++;
            if (code[i].op == ' ') {
                printf(" %s = %s\n", code[i].result, code[i].op1);
            } else {
                printf(" %s = %s %c %s\n", code[i].result, code[i].op1, code[i].op, code[i].op2);
            }
        }
    }
    if (active_count == 0) {
        printf(" (All code eliminated)\n");
    }
    printf("---------------------------\n");
}

int main() {
    read_input_code();
    
    if (NUM_INSTRUCTIONS == 0) {
        printf("Exiting simulation.\n");
        return 0;
    }

    print_code("Input Code");

    
    constant_folding();
    print_code("After Constant Folding");

    strength_reduction();
    print_code("After Strength Reduction (Algebraic Simplification)");

    copy_propagation_and_dead_code_elimination();
    print_code("Output Code (After Copy Propagation & Dead Code Elimination)");

    return 0;
}
