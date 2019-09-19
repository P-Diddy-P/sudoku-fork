/*
 * parser.c
 *
 *  Created on: 31 Jul 2019
 *      Author: itait
 *
 *
 *
 */

# include "parser.h"

/* reads line until first newline char encountered,
 * using fgets on the standard input. if line is not longer
 * than 257 chars including last newline char, returns an
 * array which contains the line.
 * if longer than 257 chars, updates.
 * if EOF, return singleton containing END_OF_FILE */
char* parse_get_line(int* flags, char* line) {
	int k;
	int valid_line_length = 0;
	char c;
	char *null_line_chk;


	if (feof(stdin)) {
		flags[EOF_EXIT] = 1;
		return 0;
	}

	/* get line using fgets - reads until first newline
	 * char encountered.
	 * for the specific case where the line is NULL, i.e,
	 * there are no whitespace, it must be the last line,
	 * because even a "blank" line has a newline char */
	null_line_chk = fgets(line, BUFF_SIZE, stdin);

	/* for the case that EOF reached after reading the line
	 * TODO need to check for EOF before calling parse_user or get_line
	 * in main flow
	 *
	 * if EOF reached straight after the line, need to do the op
	 * and then exit, withput reading another line */
	if (feof(stdin)) {
		flags[EOF_EXIT] = 1;

		/* last line contains EOF only - line is NULL, return NULL */
		if (null_line_chk == NULL) {
			return NULL;
		}

	}

	for (k = 0; k < BUFF_SIZE; k++) {
		if ((line[k] == 10) || (line[k] == 0)) {
			valid_line_length = 1;
			break;
		}
	}

	if (valid_line_length) {
		return line;
	} else {

		while (((c = fgetc(stdin)) != 10) || ((c = fgetc(stdin)) != EOF)) {
			/*advance stdin pointer until newline or EOF */
		}

		if (feof(stdin)) {
			flags[EOF_EXIT] = 1;
		}
		flags[INVALID_LINE_LENGTH] = 1;
		return 0;
	}
}

/* print the appropriate syntax, if user input is wrong */
void parse_print_command_correct_args(int* flags) {
	printf("Correct syntax is: ");
	if (flags[USER_COMMAND] == SOLVE) {
		printf("%s\n", SOLVE_SYN);
	} else if ((flags[USER_COMMAND] == EDIT_NO_PATH)
			|| ((flags[USER_COMMAND] == EDIT_WITH_PATH))) {
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

}

/* prints an error message to inform the user
 * that the command entered is not available
 * in current mode, and in which modes it is available.
 * implemented using variable length argument, as the function
 * can get 1 or 2 modes in which the commnad is available.*/
void parse_print_command_modes_error(int num_args, ...) {
	int k;
	va_list list;
	char **to_print = to_print = calloc(num_args, sizeof(char*));
	memory_alloc_error();

	va_start(list, num_args);

	for (k = 0; k < num_args; k++) {
		to_print[k] = calloc(50, sizeof(char));
		memory_alloc_error();
		strcpy(to_print[k], va_arg(list, char*));
	}
	va_end(list);

	printf("Command %s not available in current mode"
			", is available in %s %s%s%s only\n", to_print[0], to_print[1],
			((num_args > 2) ? "and " : ""), ((num_args > 2) ? to_print[2] : ""),
			((num_args > 2) ? " modes" : "mode"));

	for (k = 0; k < num_args; k++) {
		free(to_print[k]);
	}

	free(to_print);
}

/* return true if token consists only of whitespace characters */
int is_token_ws(char *str) {
	int k, length;

	if (str == NULL) {
		return 1;
	}

	length = (int) strlen(str);
	for (k = 0; k < length; k++) {
		if (!isspace(str[k])) {
			return 0;
		}
	}

	return 1;
}

/*used to parse the path in commands SOLVE_STR, EDIT_STR & "save"
 * the optional argument is used to differentiate between
 * SOLVE_STR and "save" which have a mandatory path
 * and EDIT_STR where the path is optional*/
void parse_command_path(int* flags, char* token, char *strings[],
		int is_optional) {
	char *path;
	int k = 0;

	token = strtok(NULL, "\r\n");

	if ((token == NULL) || is_token_ws(token)) {
		if (is_optional) {
			strings[PATH] = NULL;
			return;
		} else {
			printf("Not enough arguments for command %s\n",
					strings[USER_COMMAND_NAME]);
			parse_print_command_correct_args(flags);
			flags[INVALID_USER_COMMAND] = 1;
			return;
		}
	}

	path = calloc(258, sizeof(char));
	memory_alloc_error();

	for (k = 0; k < 258; k++) {
		path[k] = token[k];
		if (token[k] == 0) {
			break;
		}
	}

	/* ignore whitespaces in the beginning*/
	while (path[0] == 32) {
		path += 1;
	}

	strings[PATH] = path;
}

/* parsing commands with arguments and storing the resulting tokens in the
 * common strings array*/
void parse_command_args(int* flags, char *strings[], char* token, int num_args) {
	int k,j;
	char *str;

	/*no need to check for EOF (get_line responsibility) */
	for (k = 1; k <= num_args; k++) {

		token = strtok(NULL, DELIM);

		if (token == NULL) {
			printf("Not enough arguments for command %s\n",
					strings[USER_COMMAND_NAME]);
			for(j=k-1; j>0; j--){
				free(strings[j]);
			}
			parse_print_command_correct_args(flags);
			flags[INVALID_USER_COMMAND] = 1;
			return;
		}


		str = calloc(strlen(token)+1, sizeof(char));
		memory_alloc_error();

		strcpy(str, token);

		strings[k] = str; /* store arguments in ARG1 (1), ARG2(2), ARG3(3)*/

	}

	token = strtok(NULL, DELIM);

	if (token != NULL) { /*too many arguments*/
		printf("Too many arguments for command %s \n",
				strings[USER_COMMAND_NAME]);

		for (j=1; j<=num_args; j++){
			free(strings[j]);
		}
		parse_print_command_correct_args(flags);
		flags[INVALID_USER_COMMAND] = 1;
	}
}

/*parsing commands with no arguments
 * checks if the first token is the only token in the line,
 * if not */
void parse_command_no_args(int* flags, char *strings[], char* token) {
	token = strtok(NULL, DELIM);

	if (token != NULL) {
		printf("Command %s receives no arguments\n",
				strings[USER_COMMAND_NAME]);

		flags[INVALID_USER_COMMAND] = 1;
	}
}

/*parsing lines according to command type - first token*/
void parse_line(char *line, int *flags, char* strings[]) {
	char* token;

	/* last line is EOF  */
	if (line == NULL) {
		flags[USER_COMMAND] = EXIT;
		return;
	}

	token = strtok(line, DELIM);

	if (token == NULL || is_token_ws(token)) { /* case blank row, only whitespace or tabs*/
		flags[BLANK_ROW] = 1;
		return;
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
		if (flags[MODE] != MODE_SOLVE) {
			parse_print_command_modes_error(2, MARK_ERRORS_STR, SOLVE_STR);
			flags[INVALID_USER_COMMAND] = 1;
		} else {
			flags[USER_COMMAND] = MARK_ERRORS;
			strings[USER_COMMAND_NAME] = MARK_ERRORS_STR;
			parse_command_args(flags, strings, token, 1);
		}
	}

	else if ((strcmp(token, PRINT_BOARD_STR)) == 0) {
		if (flags[MODE] == MODE_INIT) {
			parse_print_command_modes_error(3, PRINT_BOARD_STR, SOLVE_STR,
			EDIT_STR);
			flags[INVALID_USER_COMMAND] = 1;
		} else {
			flags[USER_COMMAND] = PRINT_BOARD;
			strings[USER_COMMAND_NAME] = PRINT_BOARD_STR;
			parse_command_no_args(flags, strings, token);
		}
	}

	else if ((strcmp(token, SET_STR)) == 0) {
		if (flags[MODE] == MODE_INIT) {
			parse_print_command_modes_error(3, SET_STR, SOLVE_STR, EDIT_STR);
			flags[INVALID_USER_COMMAND] = 1;
		} else {
			flags[USER_COMMAND] = SET;
			strings[USER_COMMAND_NAME] = SET_STR;
			parse_command_args(flags, strings, token, 3);
		}
	}

	else if ((strcmp(token, VALIDATE_STR)) == 0) {
		if (flags[MODE] == MODE_INIT) {
			parse_print_command_modes_error(3, VALIDATE_STR, SOLVE_STR,
			EDIT_STR);
			flags[INVALID_USER_COMMAND] = 1;
		} else {
			flags[USER_COMMAND] = VALIDATE;
			strings[USER_COMMAND_NAME] = VALIDATE_STR;
			parse_command_no_args(flags, strings, token);
		}
	}

	else if ((strcmp(token, GUESS_STR)) == 0) {
		if (flags[MODE] != MODE_SOLVE) {
			parse_print_command_modes_error(2, GUESS_STR, SOLVE_STR);
			flags[INVALID_USER_COMMAND] = 1;
		} else {
			flags[USER_COMMAND] = GUESS;
			strings[USER_COMMAND_NAME] = GUESS_STR;
			parse_command_args(flags, strings, token, 1);
		}
	}

	else if ((strcmp(token, GENERATE_STR)) == 0) {
		if (flags[MODE] != MODE_EDIT) {
			parse_print_command_modes_error(2, GENERATE_STR, EDIT_STR);
			flags[INVALID_USER_COMMAND] = 1;
		} else {
			flags[USER_COMMAND] = GENERATE;
			strings[USER_COMMAND_NAME] = GENERATE_STR;
			parse_command_args(flags, strings, token, 2);
		}
	}

	else if ((strcmp(token, UNDO_STR)) == 0) {
		if (flags[MODE] == MODE_INIT) {
			parse_print_command_modes_error(3, UNDO_STR, SOLVE_STR, EDIT_STR);
			flags[INVALID_USER_COMMAND] = 1;
		} else {
			flags[USER_COMMAND] = UNDO;
			strings[USER_COMMAND_NAME] = UNDO_STR;
			parse_command_no_args(flags, strings, token);
		}
	}

	else if ((strcmp(token, REDO_STR)) == 0) {
		if (flags[MODE] == MODE_INIT) {
			parse_print_command_modes_error(3, REDO_STR, SOLVE_STR, EDIT_STR);
			flags[INVALID_USER_COMMAND] = 1;
		} else {
			flags[USER_COMMAND] = REDO;
			strings[USER_COMMAND_NAME] = REDO_STR;
			parse_command_no_args(flags, strings, token);
		}
	}

	else if ((strcmp(token, SAVE_STR)) == 0) {
		if (flags[MODE] == MODE_INIT) {
			parse_print_command_modes_error(3, SAVE_STR, SOLVE_STR, EDIT_STR);
			flags[INVALID_USER_COMMAND] = 1;
		} else {
			flags[USER_COMMAND] = SAVE;
			strings[USER_COMMAND_NAME] = SAVE_STR;
			parse_command_path(flags, token, strings, 0);
		}
	}

	else if ((strcmp(token, HINT_STR)) == 0) {
		if (flags[MODE] != MODE_SOLVE) {
			parse_print_command_modes_error(2, HINT_STR, SOLVE_STR);
			flags[INVALID_USER_COMMAND] = 1;
		} else {
			flags[USER_COMMAND] = HINT;
			strings[USER_COMMAND_NAME] = HINT_STR;
			parse_command_args(flags, strings, token, 2);
		}
	}

	else if ((strcmp(token, GUESS_HINT_STR)) == 0) {
		if (flags[MODE] != MODE_SOLVE) {
			parse_print_command_modes_error(2, GUESS_HINT_STR, SOLVE_STR);
			flags[INVALID_USER_COMMAND] = 1;
		} else {
			flags[USER_COMMAND] = GUESS_HINT;
			strings[USER_COMMAND_NAME] = GUESS_HINT_STR;
			parse_command_args(flags, strings, token, 2);
		}
	}

	else if ((strcmp(token, NUM_SOLUTIONS_STR)) == 0) {
		if (flags[MODE] == MODE_INIT) {
			parse_print_command_modes_error(3, NUM_SOLUTIONS_STR, SOLVE_STR,
			EDIT_STR);
			flags[INVALID_USER_COMMAND] = 1;
		} else {
			flags[USER_COMMAND] = NUM_SOLUTIONS;
			strings[USER_COMMAND_NAME] = NUM_SOLUTIONS_STR;
			parse_command_no_args(flags, strings, token);
		}
	}

	else if ((strcmp(token, AUTOFILL_STR)) == 0) {
		if (flags[MODE] != MODE_SOLVE) {
			parse_print_command_modes_error(2, AUTOFILL_STR, SOLVE_STR);
			flags[INVALID_USER_COMMAND] = 1;
		} else {
			flags[USER_COMMAND] = AUTOFILL;
			strings[USER_COMMAND_NAME] = AUTOFILL_STR;
			parse_command_no_args(flags, strings, token);
		}
	}

	else if ((strcmp(token, RESET_STR)) == 0) {
		if (flags[MODE] == MODE_INIT) {
			parse_print_command_modes_error(3, RESET_STR, SOLVE_STR, EDIT_STR);
			flags[INVALID_USER_COMMAND] = 1;
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
		if (!flags[INVALID_USER_COMMAND]) {
			flags[EOF_EXIT] = 1;
		}
	}

	else { /* first token invalid - no such command exists*/
		printf("Invalid command\n");
		flags[INVALID_USER_COMMAND] = 1;

	}

}

/* input is flags array, reads input line and updates the flags array accordingly
 * */
void parse_user(int *flags, char *strings[]) {
	char input[BUFF_SIZE];
	char *line;
	int k = 0;

	if (flags[EOF_EXIT]) {
		flags[USER_COMMAND] = EXIT;
		return;
	}

	printf("Please enter a command:\n");

	for (k = NULLIFY_START; k <= NULLIFY_END; k++) {
		flags[k] = 0;
	}

	line = parse_get_line(flags, input);

	if (flags[INVALID_LINE_LENGTH]) {
		printf(
				"Command length invalid, at most 256 characters per line are allowed.\n");
		flags[INVALID_USER_COMMAND] = 1;
		return;
	}

	parse_line(line, flags, strings);

}

