#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <editline/readline.h>
#include <editline/history.h>

#include "mpc.h"

/* Create Enumeration of Possible lval Types */
typedef enum { LVAL_NUM, LVAL_ERR, LVAL_SYM, LVAL_SEXPR } lval_type;

/* Declare New lval Struct */
typedef struct {
	lval_type type;
	long num;
	/* Error and Symbol types have some string data */
	char* err;
	char* sym;
	/* Count and Pointer to a list of "lval*" */
	int count;
	struct lval** cell;
} lval;

/* Construct a pointer to a new Number lval */
lval* lval_num(long x) {
	lval* v = malloc(sizeof(lval));
	v->type = LVAL_NUM;
	v->num	= x;
	return v;
}

/* Construct a pointer to a new Error lval */
lval* lval_err(char* x) {
	lval* v = malloc(sizeof(lval));
	v->type	= LVAL_ERR;
	v->err	= malloc(strlen(m) + 1);
	strcpy(v->err, m);
	return v;
}

/* Construct a pointer to a new Symbol lval */
lval* lval_sym(char* s) {
	lval* v = malloc(sizeof(lval));
	v->type	= LVAL_SYM;
	v->sym	= malloc(strlen(s) + 1);
	strcpy(v->sym, s);
	return v;
}

/* A pointer to a new empty Sexpr lval */
lval* lval_sexpr(void) {
	lval* v		= malloc(sizeof(lval));
	v->type		= LVAL_SEXPR;
	v->count	= 0;
	v->cell		= NULL;
	return v;
}

void lval_del(lval* v) {
	switch (v->type) {
		/* Do nothing special for number type */
		case LVAL_NUM: break;

		/* For Err or Sym free the string data */
		case LVAL_ERR: free(v->err); break;
		case LVAL_SYM: free(v->symb); break;

		/* If Sexpr then delete all elements inside */
		case LVAL_SEXPR:
			for (int i = 0; i < v->count; i++) {
				lval_dell(v->cell[i]);
			}
			/* Also free the memory allocated to contain the pointers */
			free(v->cell);
		break;
	}

	free(v);
}

lval* lval_read_num(mpc_ast_t* t) {
	errno = 0;
	long x = strtol(t->contents, NULL, 10);
	return errno != ERANGE ?
		lval_num(x) : lval_err("invalid number");
}

lval* lval_read(mpc_ast_t* t) {
	
	/* If Symbol or Number terun conversion to that type */
	if (strstr(t->tag, "number")) { return lval_read_num(t); }
	if (strstr(t->tag, "symbol")) { return lval_sym(t->contents); }
	
	/* If root (>) or sexpr then create empty list */
	lval* x = NULL;
	if (strcmp(t->tag, ">") == 0) { x = lval_sexpr(); }
	if (strstr(t->tag, "sexpr"))  { x = lval_sexpr(); }

	/* Fill this list with any valid expression contained within */
	// TODO: Fill this out
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
			printf("Error: Invalid Symbol!");
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
	mpc_parser_t* Symbol	= mpc_new("symbol");
	mpc_parser_t* Sexpr		= mpc_new("sexpr");
	mpc_parser_t* Expr		= mpc_new("expr");
	mpc_parser_t* Lispy		= mpc_new("lispy");

	/* Define language based on parsers */
	mpca_lang(
		MPCA_LANG_DEFAULT,
		"															\
			number		:	/-?[0-9]+(\\.[0-9]+)?/ ;				\
			symbol		:	'+' | '-' | '*' | '/' | '%';			\
			sexpr		:	'(' <expr>* ')' ;						\
			expr		:	<number> | <symbol> | <sexpr> ;			\
			lispy		:	/^/ <operator> <expr>+ /$/ ;			\
		",
		Number, Symbol, Sexpr, Expr, Lispy
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
	mpc_cleanup(5, Number, Symbol, Sexpr, Expr, Lispy);
	
	return 0;
}
