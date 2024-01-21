#include <stdio.h>
#include <stdlib.h>

#include "calc.h"

int main(int argc, char** argv) {

    //const char* expr = "1*-(2^2+3)*4+5"; // result = -23
    //const char* expr = "2^5"; // result = 32
    //const char* expr = "9*5/2"; // result = 22.5
    //const char* expr = "2.5*-(0.3+4)";
    char* expr;

    if(argc > 1)
        expr = argv[1];
    else {
        fprintf(stderr, "use: %s [expression]\n", argv[0]);
        return 1;
    }

    printf("input: %s\n", expr);

    parse(expr);

    /*
    printf("output: ");
    for(Token* e = queue; e != NULL; e = e->next) {
        if(e->tok == NUM)
            printf("%d", (int)e->val);
        else
            printf("%s", to_str(e->tok));
    }
    printf("\n");
    */

    printf("result: %0.3f\n", solve());

    return 0;
}
