/*
 * auxi.h
 *
 *  Created on: 1 Aug 2019
 *      Author: itait
 */

#ifndef AUXI_H_
#define AUXI_H_

# include "errors.h"


/*constants definitions*/

# define NUM_FLAGS 50
# define NUM_STRINGS 50

# define INVALID_LENGTH 0
# define END_OF_FILE 1
# define FALSE 0
# define TRUE 1



/*								FLAGS ARRAY						*/


/*
 	 	 	 	 	 ++ALL VALUES MUST BE DISTINCT++
 */
# define MODE 0

	# define MODE_INIT 0
	# define MODE_EDIT 1
	# define MODE_SOLVE 2

# define IS_SOLVED 1 /*bin*/

# define EOF_EXIT 2 /*bin*/

# define INVALID_LINE_LENGTH 3 /*bin*/

# define BLANK_ROW 4 /*bin*/

# define USER_COMMAND 5 /*name of command*/

	# define SOLVE 1
	# define EDIT 2
	# define MARK_ERRORS 3
	# define PRINT_BOARD 4
	# define SET 5
	# define VALIDATE 6
	# define GUESS 7
	# define GENERATE 8
	# define UNDO 9
	# define REDO 10
	# define SAVE 11
	# define HINT 12
	# define GUESS_HINT 13
	# define NUM_SOLUTIONS 14
	# define AUTOFILL 15
	# define RESET 16
	# define EXIT 17
	# define EDIT_NO_PATH 18
	# define EDIT_WITH_PATH 19

# define INVALID_USER_COMMAND 6

/* load assisting cells*/

# define LOAD_IS_INVALID_FORMAT 7

# define LOAD_IS_EOF 8

# define MARK_ERRORS_FLAG 9


/* in each iteration of parsing user input,
 * values in flags array need to be nullified
 * preparing for the next command from the user.
 * thus, a continuous range in the
 * flags array is used for representing the
 * user commands and if their valid */

# define NULLIFY_START 3
# define NULLIFY_END 8





/*								STRINGS ARRAY						*/

# define USER_COMMAND_NAME 0 /*name of command - in strings array*/

	# define SOLVE_STR "solve"
	# define EDIT_STR "edit"
	# define MARK_ERRORS_STR "mark_errors"
	# define PRINT_BOARD_STR "print_board"
	# define SET_STR "set"
	# define VALIDATE_STR "validate"
	# define GUESS_STR "guess"
 	# define GENERATE_STR "generate"
	# define UNDO_STR "undo"
	# define REDO_STR "redo"
	# define SAVE_STR "save"
	# define HINT_STR "hint"
	# define GUESS_HINT_STR "guess_hint"
	# define NUM_SOLUTIONS_STR "num_solutions"
	# define AUTOFILL_STR "autofill"
	# define RESET_STR "reset"
	# define EXIT_STR "exit"

	/*commands syntax (commands with arguments only)*/
	# define SOLVE_SYN "solve X"
	# define EDIT_SYN "edit [X]"
	# define MARK_ERRORS_SYN "mark_errors X"
	# define SET_SYN "set X Y Z"
	# define GUESS_SYN "guess X"
 	# define GENERATE_SYN "generate X Y"
	# define SAVE_SYN "save X"
	# define HINT_SYN "hint X Y"
	# define GUESS_HINT_SYN "guess_hint X Y"

# define ARG1 1 /* arguments for command if exist*/

# define ARG2 2

# define ARG3 3

# define PATH 4 /* cell for storing path string TODO - need to be freed upon exit*/




/*								OTHER						*/
# define NO_FILE 2


#endif /* AUXI_H_ */
