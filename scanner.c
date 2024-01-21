/*
 * Accept a string and dole it out in tokens. First call init_scanner()
 * with a string to parse, then call get_token() until NULL is returned.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "scanner.h"

static int unary = 1;

static char* input = NULL;
static int bufidx = 0;
static int maxbuf = 0;

/* Global token pointer returned by get_token(). */
static Token* token;

/*
 * Return the current character.
 */
static int get_char() {

    return input[bufidx];
}

/*
 * Consume the current character and return the new current char.
 */
static int consume_char() {

    if(bufidx < maxbuf)
        bufidx++;

    return get_char();
}

/*
 * Add a character to the string buffer.
 */
static void add_char(int ch) {

    if(token->idx < (int)sizeof(token->str))
        token->str[token->idx] = (char)ch;
    else {
        fprintf(stderr, "token buffer overrun\n");
        exit(1);
    }
    token->idx++;
}

/*
 * Retrieve the part of a number past and including the e in a number.
 */
static TokenType get_exponent() {

    if(tolower(get_char()) == 'e') {
        add_char('e');
        consume_char();

        if((get_char() == '-') || (get_char() == '+')) {
            add_char(get_char());
            consume_char();
        }

        if(isdigit(get_char())) {
            while(isdigit(get_char())) {
                add_char(get_char());
                consume_char();
            }
        }
        else {
            fprintf(stderr, "expected a digit but got a '%c'\n", get_char());
            exit(1);
        }
    }

    token->val = strtod(token->str, NULL);
    return NUM;
}

/*
 * Retrieve the part of the number including and after a '.'. The dot has
 * been seen.
 */
static TokenType get_mantissa() {

    if(get_char() == '.') {
        add_char('.');
        consume_char();

        if(isdigit(get_char())) {
            while(isdigit(get_char())) {
                add_char(get_char());
                consume_char();
            }
        }
        else {
            fprintf(stderr, "expected a digit but got a '%c'\n", get_char());
            exit(1);
        }

        if(tolower(get_char()) == 'e')
            return get_exponent();
    }

    token->val = strtod(token->str, NULL);
    return NUM;
}

/*
 * Retrieve a number from the input stream.
 */
static TokenType get_number() {

    while(isdigit(get_char())) {
        add_char(get_char());
        consume_char();
    }

    if(get_char() == '.')
        return get_mantissa();
    else if(tolower(get_char()) == 'e')
        return get_exponent();
    else {
        token->val = strtod(token->str, NULL);
        return NUM;
    }
}

/*
 * Create a new token with the ERROR type and a buffer size of 64.
 */
Token* new_token(TokenType type, double val) {

    Token* tok = malloc(sizeof(Token));
    tok->tok = type;
    tok->val = val;
    memset(tok->str, 0, sizeof(tok->str));
    tok->idx = 0;

    return tok;
}

/*
 * Do a deep copy of a token.
 */
Token* copy_token(Token* tok) {

    Token* ntok = malloc(sizeof(Token));
    memcpy(ntok, tok, sizeof(Token));

    return ntok;
}

/*
 * Initialize the scanner with a string.
 */
void init_scanner(const char* str) {

    size_t len = strlen(str) + 1;

    input = malloc(len);
    memcpy(input, str, len);
    bufidx = 0;
    maxbuf = len-1;

    // prime the pipeline
    consume_token();
}

/*
 * Return a point to the most recent token.
 */
Token* get_token() {

    return token;
}

/*
 * Return the type of the current token.
 */
TokenType token_type() {

    return token->tok;
}


/*
 * Return the next token in the token stream.
 */
Token* consume_token() {

    token = new_token(ERROR, 0.0);

    while(1) {
        int ch = get_char();
        switch(ch) {
            case EOL:
                token->tok = EOL;
                return token;
            case '*':
                if(unary) {
                    fprintf(stderr, "'*' operator cannot be unary\n");
                    exit(1);
                }
                else
                    token->tok = MUL;
                consume_char();
                unary = 1;
                return token;
            case '/':
                if(unary) {
                    fprintf(stderr, "'/' operator cannot be unary\n");
                    exit(1);
                }
                else
                    token->tok = DIV;
                consume_char();
                unary = 1;
                return token;
            case '%':
                if(unary) {
                    fprintf(stderr, "'%%' operator cannot be unary\n");
                    exit(1);
                }
                else
                    token->tok = MOD;
                unary = 1;
                consume_char();
                return token;
            case '^':
                if(unary) {
                    fprintf(stderr, "'^' operator cannot be unary\n");
                    exit(1);
                }
                else
                    token->tok = POW;
                consume_char();
                unary = 1;
                return token;
            case '+':
                if(unary) {
                    fprintf(stderr, "'+' operator cannot be unary\n");
                    exit(1);
                }
                else
                    token->tok = ADD;
                consume_char();
                unary = 1;
                return token;
            case '-':
                if(unary)
                    token->tok = UMINUS;
                else
                    token->tok = SUB;
                consume_char();
                unary = 1;
                return token;
            case '(':
                unary = 1;
                token->tok = OP;
                consume_char();
                return token;
            case ')':
                unary = 0;
                token->tok = CP;
                consume_char();
                return token;
            case '0':
                add_char('0');
                consume_char();
                if(ERROR != (token->tok = get_mantissa())) {
                    unary = 0;
                }
                return token;
            default:
                if(isdigit(ch)) {
                    if(ERROR != (token->tok = get_number())) {
                        unary = 0;
                    }
                    return token;
                }
                else if(isspace(ch)) {
                    consume_char();
                }
                else {
                    fprintf(stderr, "unexpected character: '%c'\n", ch);
                    exit(1);
                }
        }
    }

    return token;
}

/*
 * Convert the token type to a string for display.
 */
const char* to_str(TokenType tok) {

    return (tok == ADD)? "ADD":
        (tok == SUB)? "SUB":
        (tok == MUL)? "MUL":
        (tok == DIV)? "DIV":
        (tok == MOD)? "MOD":
        (tok == UMINUS)? "UMINUS":
        (tok == POW)? "POW":
        (tok == OP)? "(":
        (tok == CP)? ")":
        (tok == EOL)? "EOL":
        (tok == ERROR)? "ERROR":
        (tok == NUM)? "NUM": "UNKNOWN";
}

/*
 * Print the token for testing.
 */
void print_token(Token* tok) {

    if(tok->tok == NUM)
        printf("token: '%s': %0.3f (%s)\n", to_str(tok->tok), tok->val, tok->str);
    else
        printf("token: '%s'\n", to_str(tok->tok));
}

#ifdef TEST_SCANNER

int main() {

    Token* tok;
    const char* input = "1.0 * -(2^10 + 3.14e-2) * 4 + 0.5";
    init_scanner(input);
    do {
        tok = get_token();
        print_token(tok);
        consume_token();
    } while(tok->tok != EOL);

    return 0;
}

#endif
