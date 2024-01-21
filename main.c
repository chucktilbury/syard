#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "calc.h"

char line_buf[1024];
int line_no = 1;

static void get_line() {

    char prompt[10];
    snprintf(prompt, sizeof(prompt), "(%d) >> ", line_no);

    while(1) {
        char* ptr = readline(prompt);
        if(ptr && *ptr) {
            strncpy(line_buf, ptr, sizeof(line_buf));
            add_history(ptr);
            free(ptr);
            break;
        }
        else
            snprintf(prompt, sizeof(prompt), "(%d) >> ", line_no);
    }
}

int main(int argc, char** argv) {

    //const char* expr = "1*-(2^2+3)*4+5"; // result = -23
    //const char* expr = "2^5"; // result = 32
    //const char* expr = "9*5/2"; // result = 22.5
    //const char* expr = "2.5*-(0.3+4)";

    if(argc > 1) {
        printf("input: %s\n", argv[1]);
        parse(argv[1]);
        printf("result: %0.3f\n", solve());
    }
    else {
        while(1) {
            get_line();
            if(tolower(line_buf[0]) == 'q')
                break;
            else if(tolower(line_buf[0]) == 'h') {
                printf("\none character commands:\n");
                printf("  q = quit\n");
                printf("  h = help (this message)\n\n");
            }
            else {
                printf("input: %s\n", line_buf);
                parse(line_buf);
                printf("result: %0.3f\n", solve());
            }
            line_no ++;
        }
    }

    return 0;
}
