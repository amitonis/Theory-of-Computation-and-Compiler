#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_EXPR_LEN 100

typedef struct {
    char data[MAX_EXPR_LEN];
    int top;
} Stack;

void push(Stack *s, char value) {
    if (s->top < MAX_EXPR_LEN - 1) {
        s->data[++s->top] = value;
    }
}

char pop(Stack *s) {
    if (s->top > -1) {
        return s->data[s->top--];
    }
    return '\0'; 
}

char peek(Stack *s) {
    if (s->top > -1) {
        return s->data[s->top];
    }
    return '\0';
}

int precedence(char op) {
    if (op == '*' || op == '/') return 2;
    if (op == '+' || op == '-') return 1;
    return 0;
}

const char* get_stack_instruction(char op) {
    switch (op) {
        case '+': return "ADD";
        case '-': return "SUB";
        case '*': return "MUL";
        case '/': return "DIV";
        default: return NULL;
    }
}

int infix_to_postfix(const char *infix, char *postfix) {
    Stack operator_stack = {.top = -1};
    int i, j = 0;
    
    for (i = 0; infix[i] != '\0'; i++) {
        char token = infix[i];

        if (isspace(token)) {
            continue;
        }
        else if (isalpha(token) || isdigit(token)) {
            if (j > 0 && (isalpha(postfix[j-1]) || isdigit(postfix[j-1]))) {
                postfix[j++] = ' ';
            }
            postfix[j++] = token;
        }
        else if (token == '(') {
            push(&operator_stack, token);
        }
        else if (token == ')') {
            while (peek(&operator_stack) != '(' && peek(&operator_stack) != '\0') {
                postfix[j++] = ' ';
                postfix[j++] = pop(&operator_stack);
            }
            if (pop(&operator_stack) != '(') {
                printf("\nError: Mismatched parentheses.\n");
                return 0;
            }
        }
        else if (get_stack_instruction(token) != NULL) {
            // Operator
            while (precedence(peek(&operator_stack)) >= precedence(token)) {
                postfix[j++] = ' ';
                postfix[j++] = pop(&operator_stack);
            }
            postfix[j++] = ' '; // Space before pushing new operator
            push(&operator_stack, token);
        } else {
            printf("\nError: Invalid character in expression: %c\n", token);
            return 0;
        }
    }

    while (peek(&operator_stack) != '\0') {
        if (peek(&operator_stack) == '(') {
            printf("\nError: Mismatched parentheses.\n");
            return 0;
        }
        postfix[j++] = ' ';
        postfix[j++] = pop(&operator_stack);
    }

    postfix[j] = '\0';
    return 1;
}

void generate_code(const char *postfix) {
    printf("\n--- Target Stack Assembly Code ---\n");
    
    char temp_postfix[MAX_EXPR_LEN * 2];
    strcpy(temp_postfix, postfix);
    
    char *token = strtok(temp_postfix, " ");
    
    while (token != NULL) {
        if (strlen(token) > 0) {
            
            if (strlen(token) == 1 && get_stack_instruction(token[0]) != NULL) {
                printf(" %s\n", get_stack_instruction(token[0]));
            } else {
                printf(" PUSH %s\n", token);
            }
        }
        token = strtok(NULL, " ");
    }
    printf("--------------------------------\n");
}

int main() {
    char infix_expr[MAX_EXPR_LEN];
    char postfix_expr[MAX_EXPR_LEN * 2]; 

    printf("Code Generator: Arithmetic Expression to Stack Machine\n");
    printf("Enter arithmetic expression (e.g., (a+b)*c + 5/d): ");
    
    if (fgets(infix_expr, MAX_EXPR_LEN, stdin) == NULL) {
        return 0;
    }
    infix_expr[strcspn(infix_expr, "\n")] = 0; 

    printf("Input Expression: %s\n", infix_expr);

    if (infix_to_postfix(infix_expr, postfix_expr)) {
        printf("Postfix Notation: %s\n", postfix_expr);
        generate_code(postfix_expr);
    } else {
        printf("Code generation failed due to expression error.\n");
    }

    return 0;
}