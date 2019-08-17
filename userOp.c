/*
 * userOp.c
 *
 *  Created on: 4 Aug 2019
 *      Author: itait
 *
 *
 *Module purpose:
 *	 Includes all operations defined in the game.
 *	 The public function user_op receives the game pointer, flags and strings
 *	 and operates upon the current board or prints to user the appropriate error message.
 *	 The module is implemented by implementing the various operations as functions and
 *	 helper functions, using the file i/o module and solver module.
 *	 The general functions of operations are then stored in array of
 *	 function pointers, and the public function user_op is using that
 *	 array to call the appropriate operation from the array on the board.
 *
 *	 No need to check if mode is valid, being checked in parse module
 */

#include "auxi.h"
#include "gameStruct.h"
#include "fileIO.h"

/* every operation-function receives as arguments the game pointer, flags int array
 * and strings */
# define ARGS_DEF_FUNC game *gptr,int *flags,char **strings
# define NUM_OPS 16

typedef void (*f)(game*, int*, char**);





/* ----------------------------------------
 * ----------------------------------------
 * ----------------------------------------
 * ---------    USER OP FUNCTIONS ---------
 * ----------------------------------------
 * ----------------------------------------*/


/* Free game pointer
 * TODO maybe move to other module like gamestruct */
void free_game_pointer(game *gptr) {
	int i;

	/* free row pointers*/
	for (i = 0; i < gptr->sideLength; i++) {
		free(gptr->user[i]);
		free(gptr->flag[i]);
	}
	/* free array pointers */
	free(gptr->user);
	free(gptr->flag);
}

/* TODO - is there a function that does it? -
 * Checks if there are erroneous fixed cells, to be used in
 * edit and solve mode */
int is_fixed_erron(game *gptr){

	return 0;
}

/*TODO - use file-I/O module to load game from file,
 * update game mode to solve*/
void solve(ARGS_DEF_FUNC) {

}


/* Edit command -
 * if no path provided, load new empty 9X9 board*/
void edit(ARGS_DEF_FUNC) {

	game *local_gptr;

	/* if edit with no path, free current board init with empty 9X9 board*/
	if (flags[USER_COMMAND] == EDIT_NO_PATH) {

		free_game_pointer(gptr);
		init_board(gptr, 3, 3);
		return;
	}
	/* if path provided:
	 * 		- try to load board
	 * 		- if board loaded, check it's validity:
	 * 			+ a fixed cell cannot be erroneous - i.e, there cannot
	 * 			be two fixed cells that contradict each other.
	 * 			check for erroneous fixed cells and fail if true  */
	else {

		/* create local board with loaded board */
		local_gptr = load_board(ARGS_PASS_FUNC);

		/* if loading failed, return error */
		if (local_gptr == 0) {
			flags[INVALID_USER_COMMAND] = 1;
			return;
		}

		/* check if fixed cells in loaded board are erroneous.
		 * if they are, return error and free board memory */
		if (is_fixed_erron(local_gptr)) {
			free_game_pointer(local_gptr);
			flags[INVALID_USER_COMMAND] = 1;
			return;
		}

		/* TODO need to check if board is solvable? */

		/* if board is valid and not erroneous, free old gptr memory and plug new
		 * board to gptr */
		free_game_pointer(gptr);
		gptr = local_gptr;
	}

}

/* TODO changing the mark_errors bit */
void mark_errors(ARGS_DEF_FUNC) {

	/*TODO need a parse_int function, to be called on ARGS
	 * need to return some error message according to hirarchy*/

}

/*TODO - print_board already defined in gameStruct ??*/
void print_board(ARGS_DEF_FUNC) {

}

/*TODO - */
void set(ARGS_DEF_FUNC) {

}

/*TODO - using solver module */
void validate(ARGS_DEF_FUNC) {

}

/*TODO - */
void guess(ARGS_DEF_FUNC) {

}

/*TODO - */
void generate(ARGS_DEF_FUNC) {

}

/*TODO - */
void undo(ARGS_DEF_FUNC) {

}

/*TODO - */
void redo(ARGS_DEF_FUNC) {

}

/*TODO - I/O module */
void save(ARGS_DEF_FUNC) {

}

/*TODO -  */
void hint(ARGS_DEF_FUNC) {

}

/*TODO -  */
void guess_hint(ARGS_DEF_FUNC) {

}

/*TODO -  */
void num_solutions(ARGS_DEF_FUNC) {

}

/*TODO -  */
void autofill(ARGS_DEF_FUNC) {

}

/*TODO -  */
void reset(ARGS_DEF_FUNC) {

}

/*TODO - is "exit" operation necessary?*/

/*initialize function pointer array with NULL values*/
f ops[NUM_OPS] = { &solve, &edit, &mark_errors, &print_board, &set, &validate,
		&guess, &generate, &undo, &redo, &save, &hint, &guess_hint,
		&num_solutions, &autofill, &reset };

/* public function using the function array  */
void user_op(ARGS_DEF_FUNC) {

	ops[flags[USER_COMMAND]](ARGS_PASS_FUNC);

}
