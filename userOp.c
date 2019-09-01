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
#include "autocomplete.h"
#include "stackTracking.h"
#include "undoRedo.h"

/* every operation-function receives as arguments the game pointer, flags int array,
 * strings array and currentMove pointer */
# define NUM_OPS 17

/* Every operation receives as arguments gptr, flags, strings and pointer to current node*/
typedef void (*f)(game*, int*, char**, node**);

/* ----------------------------------------
 * ----------------------------------------
 * ----------------------------------------
 * --------- UserOp general Functions------
 * in this part will be written functions
 * that multiple operations are using
 * ----------------------------------------
 * ----------------------------------------*/

/*
 * Checks if there are erroneous fixed cells, to be used in
 * edit and solve mode.
 * Implemented using a local copy of the board that includes
 * only the fixed values, checks their validity.
 * (the alternative was to change the "check col/row/block for errors"
 * functions, which seemed like more work than changing the
 * "copy_board" function */
int is_fixed_erroneous(game *gptr) {

	/* initialize local board */
	game local_copy;
	game *local_copy_gptr;
	local_copy_gptr = &local_copy;

	init_board(local_copy_gptr, gptr->rows, gptr->cols);

	/* copy to local copy only fixed values */
	copy_board(gptr, local_copy_gptr->user, 0, 1);

	/* check for errors on local board */
	update_board_errors(local_copy_gptr);

	/* if there's a fixed and erroneous cell, return true*/
	if (board_has_errors(local_copy_gptr)) {
		free_game_pointer(local_copy_gptr);
		return 1;
	}

	/* return false otherwise */
	free_game_pointer(local_copy_gptr);
	return 0;

}

/* Helper function to pring X, Y or Z in error messages */
char* print_arg_name(int arg) {
	if (arg == 1) {
		return "X";
	}
	if (arg == 2) {
		return "Y";
	}

	if (arg == 3) {
		return "Z";
	}
	return 0;
}

/* Parse argument to int */
int get_int_from_str(ARGS_DEF_FUNC, int arg) {
	int get_int;

	/* check if string is zero */
	if (load_is_str_zero(strings[arg])) {
		return 0;
	}

	/* try to convert to int with atoi */
	get_int = atoi(strings[arg]);

	/* if non zero, conversions successful, return int*/
	if (get_int) {
		return get_int;
	}

	/* else, return -1, error value*/
	printf("Error, invalid argument %s for command %s. Please enter int\n",
			print_arg_name(arg), strings[USER_COMMAND_NAME]);

	flags[INVALID_USER_COMMAND] = 1;
	return NULL;

}

/* Frees all nodes currently in list, including current one
 * commit first node in list with new board
 * To be used in Edit X and Solve X ops */
void init_new_undoRedo(game *gptr, node **current, int *flags) {

	/* free all nodes after and before current node */
	terminate_all(*current);

	/* set current as NULL pointer - TODO check */
	(*current) = NULL;

	/* commit move - create list with new node
	 * currentMove now points to newly created node */
	commit_move(current, gptr, flags, 1);

	/* TODO remove safeguard at end */
	if ((*current) == NULL) {
		printf("ERROR, (*current) IS NULL, EXITING...\n");
		exit(0);
	}

}

/*------------Game Operations-----------
 * In each part of the file are defined
 * helper functions for a specific game
 * operation and the game opertaion itself
 *  */

/*-------------Solve---------------*/

/* Using file-I/O module to load game from file,
 * update game mode to solve*/
void solve(ARGS_DEF_FUNC) {

	/* init local game pointer and try to load board */
	game local_game;
	game *local_gptr;
	local_gptr = &local_game;
	load_board(local_gptr, flags, strings, currentMove);

	/* if board loading failed, return */
	if (flags[INVALID_USER_COMMAND]) {
		return;
	}

	/* if loading successful */
	/* check if fixed cells in loaded board are erroneous.
	 * if they are, return error and free board memory
	 * TODO - need to do? can you load an erroneous board
	 * in solve mode?*/
	if (is_fixed_erroneous(local_gptr)) {
		/* if true, free locally allocated game pointer and return error */
		printf("Error, loaded board contains erroneous fixed cells\n");
		free_game_pointer(local_gptr);
		flags[INVALID_USER_COMMAND] = 1;
		return;
	}

	/* (need to check if solvable? TODO) */

	/* if board is valid and not erroneous, free old gptr memory and copy new
	 * data from loaded game */
	free_game_pointer(gptr);
	assign_game_pointer(gptr, local_gptr);

	/* set mode - BEFORE creating new list -
	 * otherwise the first node is recorded with mode flag zero */
	flags[MODE] = MODE_SOLVE;

	/* initialize new empty undoRedo list with new board */
	init_new_undoRedo(gptr, currentMove, flags);

	/* print board after operation - TODO need to?*/
	print_board_aux(gptr, flags);

}

/*--------------Edit---------------*/

/* Edit operation -
 * if no path provided, load new empty 9X9 board
 * else load board from file, if exists and not erroneous */
void edit(ARGS_DEF_FUNC) {
	/* create local game for loading from file and testing */
	game local_game;
	game *local_gptr;
	local_gptr = &local_game;

	/* if edit with no path, free current board init with empty 9X9 board*/
	if ((flags[USER_COMMAND] == EDIT) && (strings[PATH] == NULL)) {
		free_game_pointer(gptr);
		init_board(gptr, 3, 3);
	}

	/* if path provided */
	else {
		/* create local board with loaded board */
		load_board(local_gptr, flags, strings, currentMove);

		/* if loading failed, return error */
		if (flags[INVALID_USER_COMMAND]) {
			return;
		}

		/* check if fixed cells in loaded board are erroneous.
		 * if they are, return error and free board memory */
		if (is_fixed_erroneous(local_gptr)) {
			printf("Error, loaded board contains erroneous fixed cells.\n");
			free_game_pointer(local_gptr);
			flags[INVALID_USER_COMMAND] = 1;
			return;
		}
		/* if board is valid and not erroneous, free old gptr memory and plug
		 * newly loded board to gptr */
		free_game_pointer(gptr);
		assign_game_pointer(gptr, local_gptr);/* TODO - discuss - do we want to change the structure
		 so gptr will be a double pointer (**gptr), so the value of (*gptr) will be */

	}

	/* set mode - BEFORE creating new list -
	 * otherwise the first node is recorded with mode flag zero */
	flags[MODE] = MODE_EDIT;

	/* initialize new empty undoRedo list with new board */
	init_new_undoRedo(gptr, currentMove, flags);

	update_board_errors(gptr);/* TODO - need?*/
}

/*------------Mark errors----------*/
/* TODO changing the mark_errors bit */
void mark_errors(ARGS_DEF_FUNC) {
	int val;

	/* try to read first argument as int */
	val = get_int_from_str(ARGS_PASS_FUNC, 1);

	/* if fails, return */
	if (val == -1) {
		return;
	}

	/* check if 0 or 1 */

	if ((val != 0) && (val != 1)) {
		printf("Error, command 'mark errors' receives 0 or 1 as argument\n");
		flags[INVALID_USER_COMMAND] = 1;
		return;
	}

	/* TODO when does errors are updated? */

	if (val == 1) {
		update_board_errors(gptr);/* TODO need to update here?*/
		flags[MARK_ERRORS_FLAG] = 1;
	}

	if (val == 0) {
		flags[MARK_ERRORS_FLAG] = 0;
	}

	/* TODO undoRedo functionality? need to change flags only*/
	commit_move(currentMove, gptr, flags, 0);

}

/*------------Print board----------*/

/* Calls print board from gameStruct */
void print_board(ARGS_DEF_FUNC) {
	print_board_aux(gptr, flags);
}

/*------------Set-----------------*/

int set_val_range(int arg, int argind, int lower, int upper) {
	if ((arg < lower) || (arg > upper)) {
		printf("Error, argument %s range is %d<=%s<=%d\n",
				print_arg_name(argind), lower, print_arg_name(argind), upper);
		return 0;
	}
	return 1;
}

int set_check(int *col, int *row, int *val, ARGS_DEF_FUNC) {

	/* parse and check arguments, return if invalid
	 * error messages are printed from functions */
	*col = get_int_from_str(ARGS_PASS_FUNC, 1);
	if (flags[INVALID_USER_COMMAND]) {
		return 0;
	}

	if (!set_val_range(*col, 1, 1, gptr->sideLength)) {
		flags[INVALID_USER_COMMAND] = 1;
		return 0;
	}

	*row = get_int_from_str(ARGS_PASS_FUNC, 2);
	if (flags[INVALID_USER_COMMAND]) {
		return 0;
	}

	if (!set_val_range(*row, 2, 1, gptr->sideLength)) {
		flags[INVALID_USER_COMMAND] = 1;
		return 0;
	}
	*val = get_int_from_str(ARGS_PASS_FUNC, 3);

	if (flags[INVALID_USER_COMMAND]) {
		return 0;
	}
	if (!set_val_range(*val, 3, 0, gptr->sideLength)) {
		flags[INVALID_USER_COMMAND] = 1;
		return 0;
	}
	return 1;
}

/* Set a value in board, check if solved */
void set(ARGS_DEF_FUNC) {

	/* init ints to get, pass pointers to parsing and range check */
	int col, row, val;

	/* if error in set values or range, return on error */
	if (!set_check(&col, &row, &val, ARGS_PASS_FUNC)) {
		flags[INVALID_USER_COMMAND] = 1;
		return;
	}

	/* if fixed in solve mode, return on error*/
	if ((gptr->flag[row - 1][col - 1] == FIXED)
			&& (flags[MODE] == MODE_SOLVE)) {
		printf("Cannot update a fixed cell in solve mode\n");
		flags[INVALID_USER_COMMAND] = 1;
		return;

	}

	/* set value at X=col, Y=row, if not fixed or in edit mode*/
	gptr->user[row - 1][col - 1] = val;


	update_board_errors(gptr);

	/* TODO  - create function
	 * if it is the last cell to be filled
	 * check for errors
	 */
	if (is_board_full(gptr)) {
		/* update errors - TODO - maybe check for errors without updating? */
		/* if board doesn't have any errors, the board is solved */
		if (!board_has_errors(gptr)) {
			flags[IS_SOLVED] = 1;
			print_board_aux(gptr, flags);
			printf("Puzzle was solved successfully!\nMode is Now Init\n");

			/* set mode to init */
			flags[MODE] = MODE_INIT;

			return;
			/* TODO - outer logic sees IS_SOLVED flag and operates?
			 * change back to INIT mode?*/

			/* note - cannot undo after valid solution of puzzle,
			 * no need to update undoRedo list */
		}
		/* board is full and erroneous */
		printf("Board is full but erroneous\n");
	}


	/* print board after operation */
		print_board_aux(gptr, flags);

	/* commit move to list with old board */
	commit_move(currentMove, gptr, flags, 0);
}

/*------------Validate-----------------*/

/*TODO - using solver module */
void validate(ARGS_DEF_FUNC) {

}

/*------------Guess-----------------*/

/* Parse argument to float */
double get_float_from_str(ARGS_DEF_FUNC, int arg) {
	double get_float;

	/* check if string is zero */
	if (load_is_str_zero(strings[arg])) {
		return 0;
	}

	/* try to convert to int with atoi */
	get_float = atof(strings[arg]);

	/* if non zero, conversions successful, return int*/
	if (!get_float) {
		return get_float;
	}

	/* else, return -1, error value*/
	printf("Error, invalid argument for command %s. Please enter float\n",
			strings[USER_COMMAND_NAME]);
	return -1;

}

/*TODO - */
void guess(ARGS_DEF_FUNC) {

	/* TODO implement*/

	/* commit move to list with old board */
	commit_move(currentMove, gptr, flags, 0);

}

/*------------Generate-----------------*/

/*TODO - */
void generate(ARGS_DEF_FUNC) {

}

/*------------Undo-----------------*/

/* Goes back one step in list, if exists */
void undo(ARGS_DEF_FUNC) {
	/* call undoRedo */
	undoRedo(gptr, currentMove, flags, 1);

	/* print board if undo called */
	if (!flags[INVALID_USER_COMMAND]) {
		print_board_aux(gptr, flags);
	}

}

/*------------Redo-----------------*/

/* Goes forward one step in list, if exists */
void redo(ARGS_DEF_FUNC) {
	/* call undoRedo */
	undoRedo(gptr, currentMove, flags, 0);

	/* print board if undo called */
	if (!flags[INVALID_USER_COMMAND]) {
		print_board_aux(gptr, flags);
	}

}

/*-------------Save---------------*/

/* TODO - Use solver module for determining if board is solvable */
int is_solvable(game *gptr) {
	return 0;
}

/* Using fileIO, saves current board if possible */
void save(ARGS_DEF_FUNC) {

	/* if in edit mode */
	if (flags[MODE] == MODE_EDIT) {

		/* check if board is erroneous or unsolvable, return error if true*/
		if (board_has_errors(gptr)) {
			printf("Error, erroneous board cannot be saved in Edit mode\n");
			flags[INVALID_USER_COMMAND] = 1;
			return;
		}

		if (!is_solvable(gptr)) {
			printf("Error, unsolvable board cannot be saved in Edit mode\n");
			flags[INVALID_USER_COMMAND] = 1;
			return;
		}

		/* if not erroneous or unsolvable, try to save */
		save_board(ARGS_PASS_FUNC);
		return;
	}

	/* if in solve mode, save board */
	save_board(ARGS_PASS_FUNC);

}

/*------------Hint-----------------*/

/*TODO -  */
void hint(ARGS_DEF_FUNC) {

}

/*------------Guess hint-----------------*/

/*TODO -  */
void guess_hint(ARGS_DEF_FUNC) {

	/* TODO implement*/

}

/*------------Num Solutions-----------------*/

/* Using stackTracking module, print number of
 * possible solutions */
void num_solutions(ARGS_DEF_FUNC) {
	printf("Number of possible solutions: %d\n", stack_tracking(gptr));
}

/*------------Autofill-----------------*/

/* Using autocomplete module, fill all obvious cells  */
void autofill(ARGS_DEF_FUNC) {
	/* update board errors*/
	update_board_errors(gptr);

	/* check if board is erroneous */
	if (board_has_errors(gptr)) {
		printf("Error, cannot run autofill on an erroneous board\n");
		flags[INVALID_USER_COMMAND] = 1;
		return;
	}



	/* run autocomplete on board*/
	iterative_auto_complete(gptr);

	/*TODO - add solution and game
	 * end mechanism like set
	 * Create new helper function for
	 * autofill, set and guess*/

	/* commit move to list with old board */
	commit_move(currentMove, gptr, flags, 0);
}

/*------------Reset-----------------*/

/* While there are previous moves to be undone, call undoRedo
 * directly from undoRedo module, because undo from userOp
 * also prints the board after undo */
void reset(ARGS_DEF_FUNC) {
	while ((*currentMove)->prev != NULL) {
		undoRedo(gptr, currentMove, flags, 1);
	}

}

void Exit(ARGS_DEF_FUNC) {
	printf("Exiting game...\n");
	terminate_all(*currentMove);
	free_game_pointer(gptr);

}

/*------------Function pointer array-----------------*/
/*initialize function pointer array with NULL values*/
f ops[NUM_OPS] = { &solve, &edit, &mark_errors, &print_board, &set, &validate,
		&guess, &generate, &undo, &redo, &save, &hint, &guess_hint,
		&num_solutions, &autofill, &reset, &Exit };

/* public function using the function array  */
void user_op(ARGS_DEF_FUNC) {

	if (flags[BLANK_ROW] || flags[INVALID_USER_COMMAND]) {
		return;
	}

	/* USER_COMMAND macro shifted +1,
	 * call command -1 from ops function array */
	ops[flags[USER_COMMAND] - 1](ARGS_PASS_FUNC);

}

void print_art() {
	printf(
			"\t\t\t+-+-+-+-+-+-+-+ +-+-+\n\t\t\t|W|E|L|C|O|M|E| |T|O|\n\t\t\t+-+-+-+-+-+-+-+ +-+-+\n");
	printf("	 _______           ______   _______  _                \n");
	printf("	(  ____ \\|\\     /|(  __  \\ (  ___  )| \\    /\\|\\     /|\n");
	printf("	| (    \\/| )   ( || (  \\  )| (   ) ||  \\  / /| )   ( |\n");
	printf("	| (_____ | |   | || |   ) || |   | ||  (_/ / | |   | |\n");
	printf("	(_____  )| |   | || |   | || |   | ||   _ (  | |   | |\n");
	printf("	      ) || |   | || |   ) || |   | ||  ( \\ \\ | |   | |\n");
	printf("	/\\____) || (___) || (__/  )| (___) ||  /  \\ \\| (___) |\n");
	printf("	\\_______)(_______)(______/ (_______)|_/    \\/(_______)\n");
}
