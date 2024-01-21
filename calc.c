#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

typedef enum {
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    UMINUS,
    POW,
    OP,
    CP,
    NUM,
} Token;

typedef struct _elem_ {
    Token tok;
    float val;
    struct _elem_* next;
} Elem;

static Elem* stack = NULL;
static Elem* queue = NULL;
static Elem* last = NULL;

/*
 * Convert the operator or the number to a token.
 */
Token convert(int ch) {

    switch(ch) {
        case '+': return ADD;
        case '-': return SUB;
        case '*': return MUL;
        case '/': return DIV;
        case '%': return MOD;
        case '^': return POW;
        case '(': return OP;
        case ')': return CP;
        default:
            if(isdigit(ch))
                return NUM;
            else {
                fprintf(stderr, "cannot scan character: '%c'\n", ch);
                exit(1);
            }
    }
}

const char* to_str(Token tok) {

    return (tok == ADD)? "+":
        (tok == SUB)? "-":
        (tok == MUL)? "*":
        (tok == DIV)? "/":
        (tok == MOD)? "%":
        (tok == UMINUS)? "_":
        (tok == POW)? "^":
        (tok == OP)? "(":
        (tok == CP)? ")":
        (tok == NUM)? "NUM": "UNKNOWN";
}

/*
 * Convert a single digit number to a float.
 */
float num(int ch) {

    return (float)(ch - '0');
}

/*
 * Allocate memory for a stack or a queue item. No attempt to free because
 * this simply runs once and exists.
 */
Elem* new_elem(Token tok, float val) {

    Elem* e = malloc(sizeof(Elem));
    e->tok = tok;
    e->val = val;
    e->next = NULL;

    return e;
}

/*
 * Free the queue and reset it to empty.
 */
void destroy_list(Elem* first) {

    Elem *tmp, *next;
    for(tmp = first; tmp != NULL; tmp = next) {
        next = tmp->next;
        free(tmp);
    }
}

/*
 * Push an element on the stack.
 */
void push(Token tok, float val) {

    Elem* e = new_elem(tok, val);

    e->next = stack;
    stack = e;
}

/*
 * Pop an element from the stack.
 */
Elem* pop() {

    Elem* val;

    if(stack != NULL) {
        val = stack;
        stack = stack->next;
    }
    else {
        val = NULL;
    }

    return val;
}

/*
 * Peek the top of the stack.
 */
Elem* peek() {

    //printf("peek: %s\n", to_str(stack->tok));
    if(stack != NULL)
        return stack;
    else
        return NULL;
}

/*
 * If the stack is empty, then return 1, else return 0. This is needed because
 * the evaluate stack could contain a negative number.
 */
int empty() {

    //printf("empty: %s\n", (stack == NULL)? "TRUE": "FALSE");
    return (stack == NULL)? 1: 0;
}

/*
 * Append to the queue.
 */
void append(Elem* elem) {

    Elem* e = new_elem(elem->tok, elem->val);

    if(last != NULL)
        last->next = e;
    else
        queue = e;

    last = e;
}

/*
 * Return true if the tok is an operator.
 */
int operator(Token tok) {

    switch(tok) {
        case ADD:
        case SUB:
        case MUL:
        case DIV:
        case MOD:
        case POW:
        case UMINUS:
        case OP:
        case CP:
            return 1;
        default:
            return 0;
    }

    // happy compiler...
    return 0;
}

/*
 * Return the preceidence of an operator.
 */
int prec(Token tok) {

    switch(tok) {
        case ADD:
        case SUB:
            return 1;
        case MUL:
        case DIV:
        case MOD:
            return 2;
        case POW:       // pow()
            return 3;
        case UMINUS:
            return 4;   // unary '-'
        default:
            return 0;   // no precidence
    }

    return 0;
}

/*
 * Return true if the operator is left-associative.
 */
int assoc(Token tok) {

    if(tok == UMINUS || tok == POW)
        return 0;
    else
        return 1;
}

/*
 * Parse the string into the queue data structure. All numbers are one
 * character and no spaces are allowed. Operators are one character.
 */
void parse(const char* str) {

    int ch;
    int idx = 0;
    int len = strlen(str);
    int flag = 1;

    while(idx < len) {
        ch = str[idx];

        if(isdigit(ch)) {
            append(new_elem(convert(ch), num(ch)));
            idx++;
            flag = 0;
        }
        // parse the operator
        else if(operator(convert(ch))) {
            ch = convert(ch);

            // unary test
            if((ch == SUB) && flag) {
                ch = UMINUS;
            }
            else if((ch != OP) && flag) {
                fprintf(stderr, "%s operator cannot be unary\n", to_str(ch));
                exit(1);
            }
            flag = 1;

            // handle parens
            if(ch == OP) {
                push(OP, 0.0);
                idx++;
            }
            else if(ch == CP) {
                //printf("seen: ')'\n");
                while(1) {
                    Elem* v = pop();
                    if(empty()) {
                        fprintf(stderr, "missing '('\n");
                        exit(1);
                    }
                    else if(v->tok == OP)
                        break;
                    else
                        append(v);
                }

                idx++;
                flag = 0;
            }
            // is left assoc
            else if(assoc(ch)) {
                while(!empty() && (prec(peek()->tok) >= prec(ch))) {
                    append(pop());
                }

                push(ch, 0.0);
                idx++;
            }
            // else is right assoc
            else {
                //printf("here '%s'\n", to_str(ch));
                while(!empty() && (prec(peek()->tok) > prec(ch))) {
                    append(pop());
                }

                push(ch, 0.0);
                idx++;
            }
        }
        else {
            fprintf(stderr, "invalid operator\n");
            exit(1);
        }
    }

    while(!empty()) {
        Elem* e = pop();
        append(e);

        if(e->tok == OP) {
            fprintf(stderr, "too many '('\n");
            exit(1);
        }
    }
}

/*
 * Solve the postfix expr in the queue and return the float value.
 */
float solve() {

    stack = NULL;

    for(Elem* e = queue; e != NULL; e = e->next) {
        switch(e->tok) {
            case ADD: {
                    Elem* right = pop();
                    Elem* left = pop();
                    push(NUM, left->val + right->val);
                }
                break;
            case SUB: {
                    Elem* right = pop();
                    Elem* left = pop();
                    push(NUM, left->val - right->val);
                }
                break;
            case MUL: {
                    Elem* right = pop();
                    Elem* left = pop();
                    push(NUM, left->val * right->val);
                }
                break;
            case DIV: {
                    Elem* right = pop();
                    Elem* left = pop();
                    push(NUM, left->val / right->val);
                }
                break;
            case MOD: {
                    Elem* right = pop();
                    Elem* left = pop();
                    push(NUM, fmodf(left->val, right->val));
                }
                break;
            case POW: {
                    Elem* right = pop();
                    Elem* left = pop();
                    push(NUM, powf(left->val, right->val));
                }
                break;
            case UMINUS: {
                    Elem* val = pop();
                    push(NUM, -val->val);
                }
                break;
            default:
                // value
                push(e->tok, e->val);
                break;
        }
    }

    return pop()->val;
}

int main() {

    //const char* expr = "1*-(2^2+3)*4+5"; // result = -23
    //const char* expr = "2^5"; // result = 32
    const char* expr = "9*5/2"; // result = 22.5

    printf("input: %s\n", expr);

    parse(expr);

    printf("output: ");
    for(Elem* e = queue; e != NULL; e = e->next) {
        if(e->tok == NUM)
            printf("%d", (int)e->val);
        else
            printf("%s", to_str(e->tok));
    }
    printf("\n");

    printf("result: %f\n", solve());

    return 0;
}
