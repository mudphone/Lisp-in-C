#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "mpc.h"

/* Declare New lval Struct */
typedef struct {
  int type;
  long num;
  int err;
} lval;

/* Create Enumeration of Possible lval Types */
enum { LVAL_NUM, LVAL_ERR };

/* Create Enumeration of Possible Error Types */
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

/* Create a new number type lval */
lval lval_num(long x) {
  lval v;
  v.type = LVAL_NUM;
  v.num = x;
  return v;
}

/* Create a new error type lval */
lval lval_err(int x) {
  lval v;
  v.type = LVAL_ERR;
  v.err = x;
  return v;
}

/* Print an "lval" */
void lval_print(lval v) {
  switch (v.type) {
    /* In the case the type is a number print it */
    /* Then 'break' out of the switch. */
    case LVAL_NUM: printf("%li", v.num); break;

    /* In the case the type is an error */
    case LVAL_ERR:
      /* Check what type of error it is and print it */
      if (v.err == LERR_DIV_ZERO) {
        printf("Error: Division By Zero!");
      }
      if (v.err == LERR_BAD_OP) {
        printf("Error: Invalid Operator!");
      }
      if (v.err == LERR_BAD_NUM) {
        printf("Error: Invalid Number!");
      }
      break;
  }
}

/* Print an "lval" followed by a newline */
void lval_println(lval v) { lval_print(v); putchar('\n'); }


/* Use operator string to see which operation to perform. */
lval eval_op(lval x, char* op, lval y) {

  /* If either value is an error return it */
  if (x.type == LVAL_ERR) { return x; }
  if (y.type == LVAL_ERR) { return y; }

  /* Otherwise do maths on the number values */
  if (strcmp(op, "+") == 0) { return lval_num(x.num + y.num); }
  if (strcmp(op, "-") == 0) { return lval_num(x.num - y.num); }
  if (strcmp(op, "*") == 0) { return lval_num(x.num * y.num); }
  if (strcmp(op, "^") == 0) { return lval_num(pow(x.num, y.num)); }
  if (strcmp(op, "min") == 0) { return lval_num(x.num > y.num ? y.num : x.num); }
  if (strcmp(op, "max") == 0) { return lval_num(x.num > y.num ? x.num : y.num); }
  if (strcmp(op, "/") == 0 || strcmp(op, "%") == 0) {
    /* If second operand is zero return error */
    if (y.num == 0) {
      return lval_err(LERR_DIV_ZERO);
    } else {
      if (strcmp(op, "/") == 0) { return lval_num(x.num / y.num); }
      if (strcmp(op, "%") == 0) { return lval_num(x.num % y.num); }
    }
  }
  
  return lval_err(LERR_BAD_OP);
}


lval eval(mpc_ast_t* t) {

  /* If tagged as number return it directly. */
  if (strstr(t->tag, "number")) {
    /* Check if there is some error in the conversion */
    errno = 0;
    long x = strtol(t->contents, NULL, 10);
    return errno != ERANGE ? lval_num(x) : lval_err(LERR_BAD_NUM);
  }

  /* The operator is always the second child. */
  char* op = t->children[1]->contents;

  /* We store the third child in `x` */
  lval x = eval(t->children[2]);

  /* Negate if only one param and is subtraction operator. */
  if (t->children_num < 5 && strcmp(op, "-") == 0) { return lval_num(-x.num); }
  
  /* Iterate the remaining children and combining. */
  int i = 3;
  while (strstr(t->children[i]->tag, "expr")) {
    x = eval_op(x, op, eval(t->children[i]));
    i++;
  }

  return x;
}


int main(int argc, char** argv) {

  /* Create Some Parsers */
  mpc_parser_t* Number   = mpc_new("number");
  mpc_parser_t* Operator = mpc_new("operator");
  mpc_parser_t* Expr     = mpc_new("expr");
  mpc_parser_t* Lispy    = mpc_new("lispy");

  /* Define them with the following Language */
  mpca_lang(MPC_LANG_DEFAULT,
            "                                                     \
               number   : /-?[0-9]+/ ;                            \
               operator : '+' | '-' | '*' | '/' | '%' | '^' | /min/ | /max/ ; \
               expr     : <number> | '(' <operator> <expr>+ ')' ; \
               lispy    : /^/ <operator> <expr>+ /$/ ;            \
            ",
            Number, Operator, Expr, Lispy);

  
  /* Print Version and Exit Information */
  puts("Lispy Version 0.0.0.0.1");
  puts("Press Ctrl+c to Exit.\n");

  /* In a never-ending loop... */
  while (1) {

    /* Output prompt, & read a line of user input */
    char* input = readline("lispy> ");

    /* Add input to history */
    add_history(input);

    /* Attmpt to Parse the user Input */
    mpc_result_t r;
    if (mpc_parse("<stdin>", input, Lispy, &r)) {
      /* On Success Eval the AST */
      lval result = eval(r.output);
      lval_println(result);
      mpc_ast_delete(r.output);
    } else {
      /* Otherwise Print the Error */
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }

    /* Free retrieved input */
    free(input);
  }
  
  /* Understanding and Delete our Parsers */
  mpc_cleanup(4, Number, Operator, Expr, Lispy);

  return 0;
}
