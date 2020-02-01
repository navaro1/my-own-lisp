#include <stdio.h>
#include <stdlib.h>

#include <editline/readline.h>
#include <editline/history.h>

#include "mpc.h"

int main(int argc, char** argv) {
	/* Create Some Parsers */
	mpc_parser_t* Number	= mpc_new("number");
	mpc_parser_t* Operator	= mpc_new("operator");
	mpc_parser_t* Expr		= mpc_new("expr");
	mpc_parser_t* Lispy		= mpc_new("lispy");

	/* Define language based on parsers */
	

	/* Print Version and Exit Information */
	puts("Lispy Version very, very early");
	puts("Press ctrl+c to Exit\n");

	/* A never ending loop */
	while(1) {
		
		/* Output our prompt and get input*/
		char* input = readline("lispy> ");

		/* Add input to history */
		add_history(input);
		
		/* Echo input back to user */
		printf("No, you are a %s\n", input);

		/* Free retrieved input */
		free(input);
	}
	
	return 0;
}
