#ifndef _SCANNER_H
#define _SCANNER_H

/*
 * Symbolic token values.
 */
typedef enum {
    EOL,    // must be '\0'
    ERROR,
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
} TokenType;

/*
 * Tokens that are scanned from the input. Stored in both the stack and
 * the queue.
 */
typedef struct _token_ {
    TokenType tok;
    double val;
    char str[64];
    int idx;
    struct _token_* next;
} Token;

void init_scanner(const char*);
Token* get_token();
Token* consume_token();
TokenType token_type();
const char* to_str(TokenType);
void print_token(Token*);
Token* new_token(TokenType, double);
Token* copy_token(Token*);

#endif /* _SCANNER_H */
