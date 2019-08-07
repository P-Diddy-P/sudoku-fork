/*
 * userOp.c
 *
 *  Created on: 4 Aug 2019
 *      Author: itait
 *
 *
 *module purpose:
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


/* every operation-function receives as arguments the game pointer, flags int array
 * and strings */
# define ARGS_DEF_FUNC game *gptr,int *flags,char **strings
# define NUM_OPS 16


typedef int (*f)(game*,int*,char**);


/*TODO - use file-I/O module to load game from file,
 * update game mode to solve*/
int solve(ARGS_DEF_FUNC){

	return 0;
}


/*TODO - use file-I/O module to load game from file,
 * update game mode to edit
 *
 * common functionality for "solve" and "edit" of
 * reading data from file and loading it into board*/
int edit(ARGS_DEF_FUNC){

	/* if edit with no path, init with empty 9X9 board*/
	if (flags[USER_COMMAND]==EDIT_NO_PATH){
		gptr = initBoard(gptr,3,3);
		/*TODO - return?*/
	}
	else {
		/* TODO load from file:
		 * 	1 - try to find file and open
		 * 	2 - if found, check for validity of parameters m & n
		 * 	and check for validity of data, in regard with:
		 * 		+ type of chars and legal tokens - ints only
		 * 		+ number of tokens
		 * 		if all of the above is OK,
		 * 		+ in edit mode:
		 * 			board need to have a solution - validate first
		 * 			board can't be erroneous
		 * 			all values should be saved as fixed ('.')
		 * 		+ in solve mode:
		 * 			board can be erroneous
		 * 			fixed values can't be erroneous (board can't get to that
		 * 			state anyway with valid moves. still need to check that
		 * 			fixed values are erroneous)
		 * 	-
		 * 	*/


	}


	return 0;
}

/* TODO changing the mark_errors bit */
int mark_errors(ARGS_DEF_FUNC){

	/*TODO need a parse_int function, to be called on ARGS
	 * need to return some error message according to hirarchy*/

	return 0;
}


/*TODO - */
int print_board(ARGS_DEF_FUNC){

	return 0;
}

/*TODO - */
int set(ARGS_DEF_FUNC){

	return 0;
}

/*TODO - using solver module */
int validate(ARGS_DEF_FUNC){

	return 0;
}

/*TODO - */
int guess(ARGS_DEF_FUNC){

	return 0;
}

/*TODO - */
int generate(ARGS_DEF_FUNC){

	return 0;
}

/*TODO - */
int undo(ARGS_DEF_FUNC){

	return 0;
}

/*TODO - */
int redo(ARGS_DEF_FUNC){

	return 0;
}

/*TODO - I/O module */
int save(ARGS_DEF_FUNC){

	return 0;
}

/*TODO -  */
int hint(ARGS_DEF_FUNC){

	return 0;
}

/*TODO -  */
int guess_hint(ARGS_DEF_FUNC){

	return 0;
}

/*TODO - exhaustive backtrack */
int num_solutions(ARGS_DEF_FUNC){

	return 0;
}

/*TODO - for each cell, enumerate all possibilities for valid values - how?
 * for each cell, create array with all m*n=N possibilities,
 * pass on row, column and block, and erase from the array any non-zero value
 * encountered - those are illegal values for that cell.
 * might be common functionality with stacktracking.
 * fill those with  */
int autofill(ARGS_DEF_FUNC){

	return 0;
}

/*TODO -  */
int reset(ARGS_DEF_FUNC){

	return 0;
}

/*TODO - is "exit" operation necessary?*/

/*initialize function pointer array with NULL values*/
f ops[NUM_OPS] = {&solve,&edit,&mark_errors,&print_board,&set,
					&validate,&guess,&generate,&undo,&redo,
					&save,&hint,&guess_hint,&num_solutions,
					&autofill,&reset};


/* public function using the function array  */
int user_op(ARGS_DEF_FUNC){

	ops[flags[USER_COMMAND]](ARGS_PASS_FUNC);

	return 0;
}
