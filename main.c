/*
 * main.c
 *
 *  Created on: 31 Jul 2019
 *      Author: itait
 */
# include "SPBufferset.h"
# include "userOp.h"
# include "parser.h"

/**/
int main() {
	int flags[NUM_FLAGS] = { 0 }, error;
	char *strings[NUM_STRINGS] = { 0 };

	game *gptr;
	node *currentMove;
	GRBenv *env = NULL;

	SP_BUFF_SET();
	srand(10);

	gptr = calloc(1, sizeof(game));
	memory_alloc_error();
	currentMove = calloc(1, sizeof(node));
	memory_alloc_error();

	error = GRBloadenv(&env, NULL);
	if (error) {
		printf("ERROR %d GRBloadenv: %s\n", error, GRBgeterrormsg(env));
		return -1;
	} else {
		errno = 0;
	}

	error = GRBsetintparam(env, GRB_INT_PAR_LOGTOCONSOLE, 0);
	if (error) {
		printf("ERROR %d GRBsetintattr(): %s\n", error, GRBgeterrormsg(env));
		return -1;
	}

	flags[MODE] = MODE_INIT;
	flags[MARK_ERRORS_FLAG] = 1; /* default value for mark errors is 1 */
	print_art();

	while (1) {

		parse_user(flags, strings);


		user_op(gptr, flags, strings, &currentMove, env);



		if (flags[EOF_EXIT]) {
			break;
		}

	}
	return 0;
}
