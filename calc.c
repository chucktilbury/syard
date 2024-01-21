#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "scanner.h"

/*
 * Stack used for both parsing and solving.
 */
static Token* stack = NULL;
/*
 * Output queue of the parser.
 */
static Token* queue = NULL;
static Token* last = NULL;

/*
 * Convert the operator or the number to a TokenType.
 *
TokenType convert(int ch) {

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
*/

/*
 * Convert a single digit number to a float.
 *
float num(int ch) {

    return (float)(ch - '0');
}
*/

/*
 * Allocate memory for a stack or a queue item. No attempt to free because
 * this simply runs once and exists.
 *
Token* new_token(TokenType tok, float val) {

    Token* e = malloc(sizeof(Token));
    e->tok = tok;
    e->val = val;
    e->next = NULL;

    return e;
}
*/

/*
 * Free the queue and reset it to empty.
 *
static void destroy_list(Token* first) {

    Token *tmp, *next;
    for(tmp = first; tmp != NULL; tmp = next) {
        next = tmp->next;
        free(tmp);
    }
}
 */

/*
 * Push an element on the stack.
 */
static void push(Token* tok) {

    //Token* e = new_token(tok, val);

    tok->next = stack;
    stack = tok;
}

/*
 * Pop an element from the stack and return the old top of stack.
 */
static Token* pop() {

    Token* val;

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
static Token* peek() {

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
static int empty() {

    //printf("empty: %s\n", (stack == NULL)? "TRUE": "FALSE");
    return (stack == NULL)? 1: 0;
}

/*
 * Append to the queue.
 */
static void append(Token* tok) {

    Token* t = copy_token(tok);

    if(last != NULL)
        last->next = t;
    else
        queue = t;

    last = t;
}

/*
 * Return true if the tok is an operator.
 */
static int operator(Token* tok) {

    switch(tok->tok) {
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
static int prec(Token* tok) {

    switch(tok->tok) {
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
static int assoc(Token* tok) {

    if(tok->tok == UMINUS || tok->tok == POW)
        return 0;
    else
        return 1;
}

/*
 * Parse the string into the queue data structure. All numbers are one
 * character and no spaces are allowed. Operators are one character.
 */
void parse(const char* str) {

    //int ch;
    //int idx = 0;
    //int len = strlen(str);
    //int flag = 1;
    init_scanner(str);
    Token* tok = get_token();

    while(token_type() != EOL) {
        tok = get_token();

        if(token_type() == NUM) {
            append(tok);
            consume_token();
        }
        // parse the operator
        else if(operator(tok)) {

            // handle parens
            if(token_type() == OP) {
                push(get_token());
                consume_token();
            }
            else if(token_type() == CP) {
                //printf("seen: ')'\n");
                while(1) {
                    Token* v = pop();
                    if(v->tok == OP)
                        break;
                    else if(empty()) {
                        fprintf(stderr, "missing '('\n");
                        exit(1);
                    }
                    else {
                        append(v);
                        consume_token();
                    }
                }
            }
            // is left assoc
            else if(assoc(get_token())) {
                while(!empty() && (prec(peek()) >= prec(get_token()))) {
                    append(pop());
                }

                push(get_token());
                consume_token();
            }
            // else is right assoc
            else {
                //printf("here '%s'\n", to_str(ch));
                while(!empty() && (prec(peek()) > prec(get_token()))) {
                    append(pop());
                }

                push(get_token());
                consume_token();
            }
        }
        else {
            fprintf(stderr, "invalid operator\n");
            exit(1);
        }
    }

    while(!empty()) {
        Token* e = pop();
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
double solve() {

    stack = NULL;

    for(Token* e = queue; e != NULL; e = e->next) {
        switch(e->tok) {
            case ADD: {
                    Token* right = pop();
                    Token* left = pop();
                    push(new_token(NUM, left->val + right->val));
                }
                break;
            case SUB: {
                    Token* right = pop();
                    Token* left = pop();
                    push(new_token(NUM, left->val - right->val));
                }
                break;
            case MUL: {
                    Token* right = pop();
                    Token* left = pop();
                    push(new_token(NUM, left->val * right->val));
                }
                break;
            case DIV: {
                    Token* right = pop();
                    Token* left = pop();
                    push(new_token(NUM, left->val / right->val));
                }
                break;
            case MOD: {
                    Token* right = pop();
                    Token* left = pop();
                    push(new_token(NUM, fmodf(left->val, right->val)));
                }
                break;
            case POW: {
                    Token* right = pop();
                    Token* left = pop();
                    push(new_token(NUM, powf(left->val, right->val)));
                }
                break;
            case UMINUS: {
                    Token* val = pop();
                    push(new_token(NUM, -val->val));
                }
                break;
            default:
                // value
                push(new_token(e->tok, e->val));
                break;
        }
    }

    return pop()->val;
}

