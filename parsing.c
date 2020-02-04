#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <editline/readline.h>
#include <editline/history.h>

#include "mpc.h"

/* Create Enumeration of Possible lval Types */
typedef enum { LVAL_NUM, LVAL_ERR } lval_type;

/* Create Enumeration of Possible Error Types */
typedef enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM } lval_error;

/* Declare New lval Struct */
typedef struct {
	lval_type type;
	long num;
	lval_error err;
} lval;

/* Create a new number type lval */
lval lval_num(long x) {
	lval v;
	v.type = LVAL_NUM;
	v.num = x;
	return v;
}

/* Create a new error type lval */
lval lval_err(lval_error x) {
	lval v;
	v.type = LVAL_ERR;
	v.err = x;
	return v;
}

/* Print an "lval" */
void lval_print(lval v) {
	switch (v.type) {
	/* In the case of the type is a number print it */
	case LVAL_NUM: printf("%li", v.num); break;
	
	/* Print error message ine case of error */
	case LVAL_ERR:
		/* Check what type of error it is and print it */
		if (v.err == LERR_DIV_ZERO) {
			printf("Error: Division by Zero!");
		} else if (v.err == LERR_BAD_OP) {
			printf("Error: Invalid Operator!");
		} else if (v.err == LERR_BAD_NUM) {
			printf("Error: Invalid Number!");
		}
		break;
	}
}

/* Print an "lval" in a seperate line */
void lval_println(lval v) {
	lval_print(v);
	putchar('\n');
}

lval eval_op(lval x, char* op, lval y) {
	if (x.type == LVAL_ERR) { return x; }
	if (y.type == LVAL_ERR) { return y; }

	if (!strcmp(op, "+")) { return lval_num(x.num + y.num); }
	if (!strcmp(op, "-")) { return lval_num(x.num - y.num); }
	if (!strcmp(op, "*")) { return lval_num(x.num * y.num); }
	if (!strcmp(op, "/")) {
		return y.num == 0
			? lval_err(LERR_DIV_ZERO)
			: lval_num(x.num / y.num);
	}
	if (!strcmp(op, "%")) { 
		return y.num == 0
		? lval_err(LERR_DIV_ZERO)
		: lval_num(x.num % y.num);
	}
	return lval_err(LERR_BAD_OP);
}

lval eval(mpc_ast_t* t) {
	if (strstr(t->tag, "number")) {
		/* Check if there is some error in conversion */
		errno = 0;
		long x = strtol(t->contents, NULL, 10);
		return errno != ERANGE ? lval_num(x) : lval_err(LERR_BAD_NUM);
	}

	/* The operator is always second child */
	char* op = t->children[1]->contents;
	
	/* We store the third child in `x` */
	lval x = eval(t->children[2]);

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
	mpc_parser_t* Number	= mpc_new("number");
	mpc_parser_t* Operator	= mpc_new("operator");
	mpc_parser_t* Expr		= mpc_new("expr");
	mpc_parser_t* Lispy		= mpc_new("lispy");

	/* Define language based on parsers */
	mpca_lang(
		MPCA_LANG_DEFAULT,
		"															\
			number		:	/-?[0-9]+(\\.[0-9]+)?/ ;				\
			operator	:	'+' | '-' | '*' | '/' | '%';			\
			expr		:	<number> | '(' <operator> <expr>+ ')' ; \
			lispy		:	/^/ <operator> <expr>+ /$/ ;			\
		",
		Number, Operator, Expr, Lispy
	);

	/* Print Version and Exit Information */
	puts("Lispy Version very, very early");
	puts("Press ctrl+c to Exit\n");

	/* A never ending loop */
	while(1) {
		
		/* Output our prompt and get input*/
		char* input = readline("lispy> ");

		/* Add input to history */
		add_history(input);
		
		/* Attempt to Parse the user Input */
		mpc_result_t r;
		if (mpc_parse("<stdin>", input, Lispy, &r)) {
			/* On Success Print Result */
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
	
	/* Undefine and Delete our Parsers */
	mpc_cleanup(4, Number, Operator, Expr, Lispy);
	
	return 0;
}
