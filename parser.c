/*
 * parser.c
 *
 *  Created on: 31 Jul 2019
 *      Author: itait
 *
 *
 *
 * module purpose:
 * parsing user input, and decide if it's valid MOSTLY SYNTACTICALLY,
 * i.e, if the command name and number of arguments are valid.
 * THAT SAID, because game-state dependent errors need to be printed to
 * user independent of correcteness of arguments (for example, using
 * the command "generate" in "solve" mode should result in error).
 * the correcteness of the arguments in regard to board dimensions,
 * currnet cell value and so on, will be treated in game/operations module
 */

# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <errno.h>
#include <stdarg.h>

# include "auxi.h"

# define BUFF_SIZE 257
# define DELIM " \t\r\n"

/* reads line until first newline char encountered,
 * using fgets on the standard input. if line is not longer
 * than 257 chars including last newline char, returns an
 * array which contains the line.
 * if longer than 257 chars, returns a singleton with the constant INVALID_LENGTH.
 * if EOF, return singleton containing END_OF_FILE */
char* parse_get_line(int* flags, char* line) {
	int k;
	int val_len = 0;
	char c;

	if (feof(stdin)) {
		/* if line starts with EOF, return EOF*/
		flags[EOF_EXIT] = 1;
		return 0;
	}

	fgets(line, BUFF_SIZE, stdin);

	/* for the case that EOF reached after reading the line
	 * TODO need to check for EOF before calling parse_user or get_line
	 * in main flow*/
	if (feof(stdin)) {
		flags[EOF_EXIT] = 1;
	}

	for (k = 0; k < BUFF_SIZE; k++) {
		if (line[k] == 10) {/*if newline encountered before the 258th char (including 257)*/
			val_len = 1; /*valid line*/
			break;
		}

	}

	if (val_len) {/*return pointer to line if valid */

		return line;
	}

	else {/*invalid line length*/
		printf("INVALID LENGTH\n");
		while ((c = fgetc(stdin)) != 10) {
			/*advance stdin pointer until newline*/
		}

		if (feof(stdin)) {
			/*or  EOF encountered*/
			flags[EOF_EXIT] = 1;
		}
		/*if EOF wasn't encountered in end of line, return invalid length*/
		flags[INVALID_LINE_LENGTH] = 1;

		return 0;
	}

}

/* print the appropriate syntax*/
int parse_print_command_correct_args(int* flags) {
	printf("Correct syntax is: ");
	if (flags[USER_COMMAND] == SOLVE) {
		printf("%s\n", SOLVE_SYN);
	} else if (flags[USER_COMMAND] == EDIT) {
		printf("%s\n", EDIT_SYN);
	} else if (flags[USER_COMMAND] == MARK_ERRORS) {
		printf("%s\n", MARK_ERRORS_SYN);
	} else if (flags[USER_COMMAND] == SET) {
		printf("%s\n", SET_SYN);
	} else if (flags[USER_COMMAND] == GUESS) {
		printf("%s\n", GUESS_SYN);
	} else if (flags[USER_COMMAND] == GENERATE) {
		printf("%s\n", GENERATE_SYN);
	} else if (flags[USER_COMMAND] == SAVE) {
		printf("%s\n", SAVE_SYN);
	} else if (flags[USER_COMMAND] == HINT) {
		printf("%s\n", HINT_SYN);
	} else {
		printf("%s\n", GUESS_HINT_SYN);
	}

	return 0;
}

/* prints an error message to inform the user
 * that the command entered is not available
 * in current mode, and in which modes it is available.
 * implemented using variable length argument, as the function
 * can get 1 or 2 modes in which the commnad is available.*/
int parse_print_command_modes_error(int num_args, ...) {
	int k;
	char **to_print = calloc(num_args, sizeof(char*));

	va_list list;
	va_start(list, num_args);

	for (k = 0; k < num_args; k++) {
		to_print[k] = calloc(1, sizeof(char*));
		to_print[k] = va_arg(list, char*);
	}

	printf("Command %s not available in current mode"
			", is available in %s %s%s%s only", to_print[0], to_print[1],
			((num_args > 2) ? "and " : ""), ((num_args > 2) ? to_print[2] : ""),
			((num_args > 2) ? " modes" : "mode"));

	for (k = 0; k < num_args; k++) {
		free(to_print[k]);
	}
	free(to_print);

	return 0;
}

/*used to parse the path in commands "solve", "edit" & "save"
 * the optional argument is used to differentiate between
 * "solve" and "save" which have a mandatory path
 * and "edit" where the path is optional*/
int parse_command_path(int* flags, char* token, char *strings[],
		int is_optional) {
	char *path;
	int k;
	int j = 0;
	;

	token = strtok(NULL, "\r\n");/*path could be token that ends in newline, no tabs or ws*/
	if (is_optional && (token == NULL)) {/*if edit an and no path given, return*/
		strings[PATH] = 0;
		return 0;
	}

	else if ((!is_optional) && (token == NULL)) {
		printf("Not enough arguments for command %s\n",
				strings[USER_COMMAND_NAME]);
		parse_print_command_correct_args(flags);
	}

	/*more than double the space allocated for windows based paths
	 * TODO - can remove when working on UNIX system only */
	path = calloc(600, sizeof(char));

	for (k = 0; k < 600; k++) {
		if (token[k] == 0) {
			break;
		}
		if (token[k] == 92) {
			path[j] = 92;
			path[j + 1] = 92;
			j += 2;
		} else {
			path[j] = token[k];
			j++;
		}
	}

	strings[PATH] = path;

	printf("PATH is:  %s\n", strings[PATH]);

	return 0;
}

/* parsing commands with arguments and storing the resulting tokens in the
 * common strings array*/
int parse_command_args(int* flags, char *strings[], char* token, int num_args) {
	int k;

	/*no need to check for EOF (get_line resp*/
	for (k = 1; k <= num_args; k++) {
		token = strtok(NULL, DELIM);

		if (token == NULL) {/*not enough arguments*/
			printf("Not enough arguments for command %s\n",
					strings[USER_COMMAND_NAME]);
			parse_print_command_correct_args(flags);
			flags[INVALID_USER_COMMAND] = 1;
			break;

		}

		strings[k] = token; /* strore arguments in ARG1 (1), ARG2(2), ARG3(3)*/
	}

	token = strtok(NULL, DELIM);

	if (token != NULL) { /*too many arguments*/
		printf("Too many arguments for command %s \n",
				strings[USER_COMMAND_NAME]);
		parse_print_command_correct_args(flags);
		flags[INVALID_USER_COMMAND] = 1;
	}

	return 0;
}

/*parsing commands with no arguments
 * checks if the first token is the only token in the line,
 * if not */
int parse_command_no_args(int* flags, char *strings[], char* token) {
	token = strtok(NULL, DELIM);

	if (token != NULL) {
		printf("Command %s receives no arguments\n",
				strings[USER_COMMAND_NAME]);
		flags[INVALID_USER_COMMAND] = 1;
	}

	return 0;
}

/*parsing lines according to command type - first token*/
int parse_line(char *line, int *flags, char* strings[]) {
	char* token;

	token = strtok(line, DELIM);

	if (token == NULL) { /* case blank row, only whitespace or tabs*/
		flags[BLANK_ROW] = 1;
	}

	else if ((strcmp(token, SOLVE_STR)) == 0) {
		flags[USER_COMMAND] = SOLVE;
		strings[USER_COMMAND_NAME] = SOLVE_STR;
		parse_command_path(flags, token, strings, 0);
	}

	else if ((strcmp(token, EDIT_STR)) == 0) {
		flags[USER_COMMAND] = EDIT;
		strings[USER_COMMAND_NAME] = EDIT_STR;
		parse_command_path(flags, token, strings, 1);
	}

	else if ((strcmp(token, MARK_ERRORS_STR)) == 0) {
		if (flags[MODE] != MODE_SOLVE) { /*if not in correct mode, print error*/
			parse_print_command_modes_error(2, MARK_ERRORS_STR, "solve");
			flags[INVALID_USER_COMMAND] = 1;
		} else {
			flags[USER_COMMAND] = MARK_ERRORS;
			strings[USER_COMMAND_NAME] = MARK_ERRORS_STR;
			parse_command_args(flags, strings, token, 1);
		}
	}

	else if ((strcmp(token, PRINT_BOARD_STR)) == 0) {
		if ((flags[MODE] != MODE_SOLVE) && (flags[MODE] != MODE_EDIT)) {
			parse_print_command_modes_error(3, PRINT_BOARD_STR, "solve",
					"edit");
			flags[INVALID_USER_COMMAND] = 1;
			return 0;
		} else {
			flags[USER_COMMAND] = PRINT_BOARD;
			strings[USER_COMMAND_NAME] = PRINT_BOARD_STR;
			parse_command_no_args(flags, strings, token);
		}
	}

	else if ((strcmp(token, SET_STR)) == 0) {
		if ((flags[MODE] != MODE_SOLVE) && (flags[MODE] != MODE_EDIT)) {
			parse_print_command_modes_error(3, SET_STR, "solve", "edit");
			flags[INVALID_USER_COMMAND] = 1;
			return 0;
		} else {
			flags[USER_COMMAND] = SET;
			strings[USER_COMMAND_NAME] = SET_STR;
			parse_command_args(flags, strings, token, 3);
		}

	}

	else if ((strcmp(token, VALIDATE_STR)) == 0) {
		if ((flags[MODE] != MODE_SOLVE) && (flags[MODE] != MODE_EDIT)) {
			parse_print_command_modes_error(3, VALIDATE_STR, "solve", "edit");
			flags[INVALID_USER_COMMAND] = 1;
			return 0;
		} else {
			flags[USER_COMMAND] = VALIDATE;
			strings[USER_COMMAND_NAME] = VALIDATE_STR;
			parse_command_no_args(flags, strings, token);
		}
	}

	else if ((strcmp(token, GUESS_STR)) == 0) {
		if (flags[MODE] != MODE_SOLVE) {
			parse_print_command_modes_error(2, GUESS_STR, "solve");
			flags[INVALID_USER_COMMAND] = 1;
			return 0;
		}
		{
			flags[USER_COMMAND] = GUESS;
			strings[USER_COMMAND_NAME] = GUESS_STR;
			parse_command_args(flags, strings, token, 1);
		}
	}

	else if ((strcmp(token, GENERATE_STR)) == 0) {
		if (flags[MODE] != MODE_SOLVE) {
			parse_print_command_modes_error(2, GENERATE_STR, "edit");
			flags[INVALID_USER_COMMAND] = 1;
			return 0;
		}
		{
			flags[USER_COMMAND] = GENERATE;
			strings[USER_COMMAND_NAME] = GENERATE_STR;
			parse_command_args(flags, strings, token, 2);
		}
	}

	else if ((strcmp(token, UNDO_STR)) == 0) {
		if ((flags[MODE] != MODE_SOLVE) && (flags[MODE] != MODE_EDIT)) {
			parse_print_command_modes_error(3, UNDO_STR, "solve", "edit");
			flags[INVALID_USER_COMMAND] = 1;
			return 0;
		}
		{
			flags[USER_COMMAND] = UNDO;
			strings[USER_COMMAND_NAME] = UNDO_STR;
			parse_command_no_args(flags, strings, token);
		}
	}

	else if ((strcmp(token, REDO_STR)) == 0) {
		if ((flags[MODE] != MODE_SOLVE) && (flags[MODE] != MODE_EDIT)) {
			parse_print_command_modes_error(3, REDO_STR, "solve", "edit");
			flags[INVALID_USER_COMMAND] = 1;
			return 0;
		} else {
			flags[USER_COMMAND] = REDO;
			strings[USER_COMMAND_NAME] = REDO_STR;
			parse_command_no_args(flags, strings, token);
		}
	}

	else if ((strcmp(token, SAVE_STR)) == 0) {
		if ((flags[MODE] != MODE_SOLVE) && (flags[MODE] != MODE_EDIT)) {
			parse_print_command_modes_error(3, SAVE_STR, "solve", "edit");
			flags[INVALID_USER_COMMAND] = 1;
			return 0;
		} else {
			flags[USER_COMMAND] = SAVE;
			strings[USER_COMMAND_NAME] = SAVE_STR;
			parse_command_path(flags, token, strings, 0);
		}
	}

	else if ((strcmp(token, HINT_STR)) == 0) {
		if (flags[MODE] != MODE_SOLVE) {
			parse_print_command_modes_error(2, HINT_STR, "solve");
			flags[INVALID_USER_COMMAND] = 1;
			return 0;
		} else {
			flags[USER_COMMAND] = HINT;
			strings[USER_COMMAND_NAME] = HINT_STR;
			parse_command_args(flags, strings, token, 2);
		}
	}

	else if ((strcmp(token, GUESS_HINT_STR)) == 0) {
		if (flags[MODE] != MODE_SOLVE) {
			parse_print_command_modes_error(2, GUESS_HINT_STR, "solve");
			flags[INVALID_USER_COMMAND] = 1;
			return 0;
		} else {
			flags[USER_COMMAND] = GUESS_HINT;
			strings[USER_COMMAND_NAME] = GUESS_HINT_STR;
			parse_command_args(flags, strings, token, 2);
		}
	}

	else if ((strcmp(token, NUM_SOLUTIONS_STR)) == 0) {
		if ((flags[MODE] != MODE_SOLVE) && (flags[MODE] != MODE_EDIT)) {
			parse_print_command_modes_error(3, NUM_SOLUTIONS_STR, "solve",
					"edit");
			flags[INVALID_USER_COMMAND] = 1;
			return 0;
		} else {
			flags[USER_COMMAND] = NUM_SOLUTIONS;
			strings[USER_COMMAND_NAME] = NUM_SOLUTIONS_STR;
			parse_command_no_args(flags, strings, token);
		}
	}

	else if ((strcmp(token, AUTOFILL_STR)) == 0) {
		if (flags[MODE] != MODE_SOLVE) {
			parse_print_command_modes_error(2, AUTOFILL_STR, "solve");
			flags[INVALID_USER_COMMAND] = 1;
			return 0;
		} else {
			flags[USER_COMMAND] = AUTOFILL;
			strings[USER_COMMAND_NAME] = AUTOFILL_STR;
			parse_command_no_args(flags, strings, token);
		}
	}

	else if ((strcmp(token, RESET_STR)) == 0) {
		if ((flags[MODE] != MODE_SOLVE) && (flags[MODE] != MODE_EDIT)) {
			parse_print_command_modes_error(3, RESET_STR, "solve", "edit");
			flags[INVALID_USER_COMMAND] = 1;
			return 0;
		} else {
			flags[USER_COMMAND] = RESET;
			strings[USER_COMMAND_NAME] = RESET_STR;
			parse_command_no_args(flags, strings, token);
		}
	}

	else if ((strcmp(token, EXIT_STR)) == 0) {
		flags[USER_COMMAND] = EXIT;
		strings[USER_COMMAND_NAME] = EXIT_STR;
		parse_command_no_args(flags, strings, token);
	}

	else { /* first token invalid - no such command exists*/
		printf("Invalid command\n");
		flags[INVALID_USER_COMMAND] = 1;
		/*TODO - invalid user command, try again
		 * general for blank row and all syntactically invalid commands?*/
	}

	return 0;
}

/* input is flags array, reads input line and updates the flags array accordingly
 * */
int parse_user(int *flags, char *strings[]) {
	char input[BUFF_SIZE];
	char *line;
	int k=0;

	if (flags[EOF_EXIT]) {/*if previous line end in EOF*/
		return 0;
	}

	printf("Please enter a command\n");


	/*nullify user command range before iteration*/
	for (k=NULLIFY_START;k<=NULLIFY_END;k++){
		flags[k]=0;
	}


	line = parse_get_line(flags, input);/*read line chars from stdin*/

	if (flags[INVALID_LINE_LENGTH]) {
		printf(
				"Command length invalid, at most 256 characters per line are allowed\n");

		/* return and prompt user for another line (part of game flow)*/
		return 0;
	}



	/*if valid line, parse line and fill flags*/
	parse_line(line, flags, strings);

	return 0;
}

