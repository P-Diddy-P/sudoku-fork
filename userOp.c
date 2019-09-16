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



# include "userOp.h"


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
int get_int_from_str(int *flags,char **strings, int arg) {
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
	return -1;
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
	commit_move(current, gptr, NULL, flags, 1);

	/* TODO remove safeguard at end */
	if ((*current) == NULL) {
		printf("ERROR, (*current) IS NULL, EXITING...\n");
		exit(0);
	}

}

/* checks if board is full and solved */
int is_game_over(game *gptr, int *flags) {

	/* if board is not full, return false */
	if (!is_board_full(gptr)) {
		return 0;
	}

	/* if board is full and not erroneous, game over */
	update_board_errors(gptr);
	if (!board_has_errors(gptr)) {
		flags[IS_SOLVED] = 1;
		print_board(gptr, flags);
		printf("Puzzle was solved successfully!\nMode is Now Init\n");

		/* set mode to init*/
		flags[MODE] = MODE_INIT;

		return 1;
	}
	/* if board has errors, it's full and erroneous */
	printf("Board is full but erroneous\n");
	return 0;
}

/*------------Game Operations-----------
 * In each part of the file are defined
 * helper functions for a specific game
 * operation and the game opertaion itself
 *  */

/*-------------Solve---------------*/

/* Using file-I/O module to load game from file,
 * update game mode to solve */
void solve(game *gptr, int *flags, char **strings, node **currentMove) {

	/* init local game pointer and try to load board */
	game local_game;
	game *local_gptr;
	local_gptr = &local_game;
	load_board(local_gptr, flags, strings);

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

	/* if board is valid and not erroneous, free old gptr memory and copy new
	 * data from loaded game */
	free_game_pointer(gptr);
	assign_game_pointer(gptr, local_gptr);

	/* set mode - BEFORE creating new list -
	 * otherwise the first node is recorded with mode flag zero */
	flags[MODE] = MODE_SOLVE;

	/* if loaded board is solved, free old_board and return */
	if (is_game_over(gptr, flags)) {
		return;
	}

	/* initialize new empty undoRedo list with new board */
	init_new_undoRedo(gptr, currentMove, flags);

	printf("Loaded board in Solve mode\n");
	print_board(gptr, flags);

	/* check if board is solved functionality*/
	is_game_over(gptr, flags);
}

/*--------------Edit---------------*/

/* Edit operation -
 * if no path provided, load new empty 9X9 board
 * else load board from file, if exists and not erroneous */
void edit(game *gptr, int *flags, char **strings, node **currentMove) {
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
		load_board(local_gptr, flags, strings);

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

	update_board_errors(gptr);

	printf("Loaded board in Edit mode\n");
	print_board(gptr, flags);

	/* unnecessary to check if the board is solved, no matter if it is full and
	 * solved or full but erroneous, we should still be able to edit it. */
}

/*------------Mark errors----------*/
/* TODO changing the mark_errors bit */
void mark_errors(game *gptr,int *flags,char **strings, node **currentMove) {
	int val;

	/* try to read first argument as int */
	val = get_int_from_str(flags, strings, 1);

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

	/* Despite there being no changes from previous move, still update the board for errors
	 * just to make sure */
	update_board_errors(gptr);

	if (val == 1) {
		flags[MARK_ERRORS_FLAG] = 1;
	} else if (val == 0) {
		flags[MARK_ERRORS_FLAG] = 0;
	}

	commit_move(currentMove, gptr, NULL, flags, 0);
}

/*------------Print board----------*/

/* Calls print board from gameStruct */
/* This operation will call print_board from gameStruct directly */

/*------------Set-----------------*/

int set_val_range(int arg, int argind, int lower, int upper) {
	if ((arg < lower) || (arg > upper)) {
		printf("Error, argument %s range is %d<=%s<=%d\n",
				print_arg_name(argind), lower, print_arg_name(argind), upper);
		return 0;
	}
	return 1;
}

int set_check(int *col, int *row, int *val, game *gptr, int *flags, char **strings) {

	/* parse and check arguments, return if invalid
	 * error messages are printed from functions */
	*col = get_int_from_str(flags, strings, 1);
	if (flags[INVALID_USER_COMMAND]) {
		return 0;
	}

	if (!set_val_range(*col, 1, 1, gptr->sideLength)) {
		flags[INVALID_USER_COMMAND] = 1;
		return 0;
	}

	*row = get_int_from_str(flags, strings, 2);
	if (flags[INVALID_USER_COMMAND]) {
		return 0;
	}

	if (!set_val_range(*row, 2, 1, gptr->sideLength)) {
		flags[INVALID_USER_COMMAND] = 1;
		return 0;
	}
	*val = get_int_from_str(flags, strings, 3);

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
void set(game *gptr,int *flags,char **strings, node **currentMove) {

	/* init ints to get, pass pointers to parsing and range check
	 * declare copy to be used */
	int col, row, val;
	int **old_board;

	/* if error in set values or range, return on error */
	if (!set_check(&col, &row, &val, gptr, flags, strings)) {
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

	/* copy values to old_board */
	old_board = init_2d_array(gptr->sideLength);
	copy_2d_array(old_board, gptr->user, gptr->sideLength);

	/* set value at X=col, Y=row, if not fixed or in edit mode*/
	gptr->user[row - 1][col - 1] = val;

	update_board_errors(gptr);

	/* if game over, free old_board and return */
	if (is_game_over(gptr, flags)) {
		free_2d_array(old_board, gptr->sideLength);
		return;
	}

	/* print board after operation */
	print_board(gptr, flags);

	/* commit move to list with old board */
	commit_move(currentMove, gptr, old_board, flags, 0);
	free_2d_array(old_board, gptr->sideLength);

}

/*------------Validate-----------------*/

/* Check if board is solvable */
void validate(game *gptr, GRBenv *env) {
	/* update board errors */
	update_board_errors(gptr);

	/* if board is erroneous, return error */
	if (board_has_errors(gptr)) {
		printf("Error, board is erroneous and thus not solvable\n");
		return;
	}

	/* check if board has solution using ILP from ILPsolver module */
	if (board_has_sol(gptr,env)) {
		printf("Board is solvable\n");
	} else {
		printf("Board is unsolvable\n");
	}
}

/*------------Guess-----------------*/

/* Parse argument to float */
double get_float_from_str(int *flags, char **strings, int arg) {
	double get_float;

	/* check if string is zero */
	if (load_is_str_zero(strings[arg])) {
		return 0;
	}

	/* try to convert to float with atof */
	get_float = atof(strings[arg]);

	/* if non zero, conversions successful, return float */
	if (!get_float) {
		return get_float;
	}

	/* else, return -1, error value*/
	printf("Error, invalid argument for command %s. Please enter float\n",
			strings[USER_COMMAND_NAME]);
	flags[INVALID_USER_COMMAND] = 1;
	return -1.0;

}

/*TODO - */
void guess(game *gptr, int *flags, char **strings, node **currentMove, GRBenv *env) {
	int **old_board;
	float thres = get_float_from_str(flags, strings, 1);

	if (flags[INVALID_USER_COMMAND]) {
		return;
	}

	old_board = init_2d_array(gptr->sideLength);
	copy_2d_array(old_board, gptr->user, gptr->sideLength);

	if (is_game_over(gptr, flags)) {
		free_2d_array(old_board, gptr->sideLength);
		return;
	}

	guess_aux(gptr, thres, env);

	commit_move(currentMove, gptr, old_board, flags, 0);
	free_2d_array(old_board, gptr->sideLength);
}

/*------------Generate-----------------*/
/* check validity of arguments passed to generate */
int generate_are_ints_valid(game *gptr, int x, int y) {
	int empty, sq;

	/* X is the number of empty cells to be randomly chosen
	 * and then attempted to be filled. If X is negative
	 * or X is more than the number of empty cells currently
	 * in the game, it's an error. X can be zero, i.e,
	 * the user can choose to not fill any cells */
	empty = count_empty(gptr->user, gptr->sideLength);
	if ((x < 0) || (x > empty)) {
		printf("Error, argument X for command generate should"
				" be in range 0<= X <= %d\n", empty);
		return 0;
	}
	/* TODO what happens if X==empty? all empty cells are filled
	 * probably will fail in 1000 iterations */

	sq = gptr->sideLength * gptr->sideLength;
	if ((y < 0) || (y > sq)) {
		printf("Error, argument Y for command generate should"
				" be in range 0<= Y <= %d\n", sq);
		return 0;
	}
	return 1;
}

void clear_all_but_y(game *gptr, int cells_to_leave) {
	int **cells;
	int i, j, index=0, num_cells;

	cells = calloc(gptr->sideLength * gptr->sideLength, sizeof(int*));
	memory_alloc_error();

	num_cells = gptr->sideLength * gptr->sideLength;
	for (i = 0; i < gptr->sideLength; i++) {
		for (j = 0; j < gptr->sideLength; j++) {
			cells[index] = calloc(2, sizeof(int));
			memory_alloc_error();

			cells[index][0] = i;
			cells[index][1] = j;
			index++;
		}
	}

	randomize_cell_array(cells, num_cells);
	for (index = 0; index < num_cells - cells_to_leave; index++) {
		gptr->user[cells[index][0]][cells[index][1]] = 0;
	}

	free_2d_array(cells,num_cells);
}

void generate(game *gptr, int *flags, char **strings, node **currentMove, GRBenv *env) {
	int cells_to_fill, cells_to_leave;
	int **local_board, **old_board;

	cells_to_fill = get_int_from_str(flags, strings, ARG1);
	cells_to_leave = get_int_from_str(flags, strings, ARG2);

	if (flags[INVALID_USER_COMMAND]) {
		return;
	}

	if (!generate_are_ints_valid(gptr, cells_to_fill, cells_to_leave)) {
		flags[INVALID_USER_COMMAND] = 1;
		return;
	}

	old_board = init_2d_array(gptr->sideLength);
	copy_2d_array(old_board, gptr->user, gptr->sideLength);
	local_board = gen_board(gptr,cells_to_fill,env);

	if (local_board == NULL) {
		flags[INVALID_USER_COMMAND] = 1;
		free_2d_array(old_board, gptr->sideLength);
		return;
	}

	copy_2d_array(gptr->user, local_board, gptr->sideLength);
	free_2d_array(local_board, gptr->sideLength);
	clear_all_but_y(gptr, cells_to_leave);

	commit_move(currentMove, gptr, old_board, flags, 0);
	print_board(gptr, flags);
}

/*------------Undo-----------------*/

/* Goes back one step in list, if exists */
void undo(game *gptr, node **currentMove, int *flags) {
	undo_aux(gptr, currentMove, flags, 1);
	print_board(gptr, flags);
}

/*------------Redo-----------------*/

/* Goes forward one step in list, if exists */
void redo(game *gptr, node **currentMove, int *flags) {
	/* call redo_aux */
	redo_aux(gptr, currentMove, flags);
	print_board(gptr, flags);
}

/*-------------Save---------------*/

/* Using fileIO, saves current board if possible */
void save(game *gptr, int *flags, char **strings, GRBenv *env) {

	/* if in edit mode */
	if (flags[MODE] == MODE_EDIT) {

		/* check if board is erroneous or unsolvable, return error if true*/
		if (board_has_errors(gptr)) {
			printf("Error, erroneous board cannot be saved in Edit mode\n");
			flags[INVALID_USER_COMMAND] = 1;
			return;
		}

		if (!board_has_sol(gptr,env)) {
			printf("Error, unsolvable board cannot be saved in Edit mode\n");
			flags[INVALID_USER_COMMAND] = 1;
			return;
		}

		/* if not erroneous or unsolvable, try to save */
		save_board(gptr,flags,strings);
		return;
	}

	/* if in solve mode, save board */
	save_board(gptr,flags,strings);

	printf("Board saved successfully\n");

}

/*------------Hint-----------------*/

/* Solve board, print value of cell  */
void hint(game *gptr, int *flags, char **strings, GRBenv *env) {
	int **local_board;
	int row, col, success;

	col = get_int_from_str(flags, strings, ARG1);
	row = get_int_from_str(flags, strings, ARG2);

	if ((row <= 0) || (row > gptr->sideLength)) {
		printf("Error, row range is 1<= row <= %d\n", gptr->sideLength);
		flags[INVALID_USER_COMMAND] = 1;
		return;
	}
	if ((col <= 0) || (col > gptr->sideLength)) {
		printf("Error, column range is 1<= row <= %d\n", gptr->sideLength);
		flags[INVALID_USER_COMMAND] = 1;
		return;
	}

	local_board = init_2d_array(gptr->sideLength);
	success = gurobi_ilp(local_board, gptr,env);

	if (!success) {
		printf("Board is unsolvable\n");
		return;
	}

	printf("Hint to row:%d, col:%d: %d\n", row, col, local_board[row][col]);

	free_2d_array(local_board, gptr->sideLength);
}

/*------------Guess hint-----------------*/

/*TODO -  */
void guess_hint(game *gptr, int *flags, char **strings, GRBenv *env) {
	int row, col;

	col = get_int_from_str(flags, strings, ARG1);
	row = get_int_from_str(flags, strings, ARG2);

	if ((row <= 0) || (row > gptr->sideLength)) {
		printf("Error, row range is 1<= row <= %d\n", gptr->sideLength);
		flags[INVALID_USER_COMMAND] = 1;
		return;
	}
	if ((col <= 0) || (col > gptr->sideLength)) {
		printf("Error, column range is 1<= row <= %d\n", gptr->sideLength);
		flags[INVALID_USER_COMMAND] = 1;
		return;
	}

	guess_hint_aux(gptr, row, col, env);
}

/*------------Num Solutions-----------------*/

/* Using stackTracking module, print number of
 * possible solutions */
void num_solutions(game *gptr) {
	printf("Number of possible solutions: %d\n", stack_tracking(gptr));
}

/*------------Autofill-----------------*/

/* Using autocomplete module, fill all obvious cells  */
void autofill(game *gptr, int *flags, node **currentMove) {
	int **old_board;

	/* update board errors*/
	update_board_errors(gptr);

	/* copy values to old_board */
	old_board = init_2d_array(gptr->sideLength);
	copy_2d_array(old_board, gptr->user, gptr->sideLength);

	/* check if board is erroneous */
	if (board_has_errors(gptr)) {
		printf("Error, cannot run autofill on an erroneous board\n");
		flags[INVALID_USER_COMMAND] = 1;
		return;
	}

	/* run autocomplete on board*/
	auto_complete(gptr);

	/* if game over, free old_board and return */
	if (is_game_over(gptr, flags)) {
		free_2d_array(old_board, gptr->sideLength);
		return;
	}

	/* commit move to list with old board */
	commit_move(currentMove, gptr, old_board, flags, 0);
	free_2d_array(old_board, gptr->sideLength);
}

/*------------Reset-----------------*/

/* While there are previous moves to be undone, call undoRedo
 * directly from undoRedo module, because undo from userOp
 * also prints the board after undo */
void reset(game *gptr, int *flags, node **currentMove) {
	while ((*currentMove)->prev != NULL) {
		undo_aux(gptr, currentMove, flags, 0);
	}

	/* print board after done*/
	print_board(gptr, flags);
}

void Exit(game *gptr, node **currentMove, char **strings, GRBenv *env) {
	int k;

	printf("Exiting game...\n");
	terminate_all(*currentMove);
	free_game_pointer(gptr);
	GRBfreeenv(env);
	for (k = STR_FREE_START; k <= STR_FREE_END; k++) {
		free(strings[k]);
	}
}

/*------------Function pointer array-----------------*/
/*initialize function pointer array with NULL values
f ops[NUM_OPS] = { &solve, &edit, &mark_errors, &print_board, &set, &validate,
		&guess, &generate, &undo, &redo, &save, &hint, &guess_hint,
		&num_solutions, &autofill, &reset, &Exit };

Old solution to userOps. instead of a switch, all operations were loaded in an array,
and an appropriate function pointer would be chosen in accordance to the parsed command.
Deprecated due to strict compiling, which demands all parameters sent to a function to be
used, making a generic function as an operation no better than a switch.
*/

/* public function using the function array  */
void user_op(game *gptr, int *flags, char **strings, node **currentMove, GRBenv *env) {
	if (flags[BLANK_ROW] || flags[INVALID_USER_COMMAND]) {
		return;
	}

	switch(flags[USER_COMMAND]) {
	case 1:
		solve(gptr, flags, strings, currentMove);
		break;
	case 2:
		edit(gptr, flags, strings, currentMove);
		break;
	case 3:
		mark_errors(gptr, flags, strings, currentMove);
		break;
	case 4:
		print_board(gptr, flags);
		break;
	case 5:
		set(gptr, flags, strings, currentMove);
		break;
	case 6:
		validate(gptr, env);
		break;
	case 7:
		guess(gptr, flags, strings, currentMove, env);
		break;
	case 8:
		generate(gptr, flags, strings, currentMove, env);
		break;
	case 9:
		undo(gptr, currentMove, flags);
		break;
	case 10:
		redo(gptr, currentMove, flags);
		break;
	case 11:
		save(gptr, flags, strings, env);
		break;
	case 12:
		hint(gptr, flags, strings, env);
		break;
	case 13:
		guess_hint(gptr, flags, strings, env);
		break;
	case 14:
		num_solutions(gptr);
		break;
	case 15:
		autofill(gptr, flags, currentMove);
		break;
	case 16:
		reset(gptr, flags, currentMove);
		break;
	case 17:
		Exit(gptr, currentMove, strings, env);
		break;
	}

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
	printf("	\\_______)(_______)(______/ (_______)|_/    \\/(_______)\n\n");
}
