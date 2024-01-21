# syard
very simple shunting yard implementation

This is as simple as possible shunting yard implementation. It accepts a single
expression on the command line and solves it. Expressions can us single digit
numbers with the operators +,-,*,/,%,^,(,), and unary -. The result printed is
the postfix and infix expression as interpreted, as well as a floating point
result.

## Detailed algorithm
(http://www.neocomputer.org/projects/lang/infix.html)

* Using a single stack for operators and a stream of input tokens:
    * For each token in the input stream:
        * If the token is an operand:
            * Output the token
        * If the token is a left-associative binary operator:
            * Pop any operators of greater or equal precedence off the stack and output them
            * Push the operator on to stack
        * If the token is a right-associative binary operator:
            * Pop any operators of greater precedence off the stack and output them
            * Push the operator on to stack
        * If the token is a prefix unary operator:
            * Push the operator on to stack
        * If the token is a postfix unary operator:
            * Output the token
        * If the token is an open parenthesis
            * push it on to the stack
        * If the token is a close parenthesis:
            * Pop any operators off the stack and output them until an open parenthesis is on the top of the stack.
            * Pop and discard the open parenthesis.
    * When there are no more tokens:
        * Pop any remaining tokens off the stack and output them
