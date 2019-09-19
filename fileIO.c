/*
 * fileIO.c
 *
 *  Created on: 16 Aug 2019
 *      Author: itait
 *
 */

# include "fileIO.h"

/* --------- LOAD BOARD FUNCTIONS ---------*/

/* Check if next char in stream is whitespace */
int load_is_next_char_space(FILE *fp) {
	char c;

	/* get next char*/
	c = fgetc(fp);

	/* if it's a space, flag, write number if read and return true*/
	if (isspace(c)) {
		return 1;
	} else { /* else, push back char and return false*/
		ungetc(c, fp);
		return 0;
	}

}

/* Advance pointer over whitespace characters until EOF
 * or non-whitespace char reached*/
void load_advance_whitespace(FILE *fp) {

	/* as long as there's a whitespace, continue reading chars from stream*/
	while (load_is_next_char_space(fp)) {
		continue;
	}

	return;

}

/* Get next string token from file */
char* load_get_next_token(int *flags, FILE *fp) {
	char c;
	int cursize = 1;
	char *loc_str = 0;

	load_advance_whitespace(fp);

	if (feof(fp)) {
		flags[LOAD_IS_EOF] = 1;
		return 0;
	}

	loc_str = calloc(cursize, sizeof(char));

	memory_alloc_error();

	c = fgetc(fp);

	/* while not EOF or whitespace, keep reading chars and reallocating
	 * memory for next char in stream. */
	while (!feof(fp)) {
		loc_str[cursize - 1] = c;
		cursize++;
		loc_str = realloc(loc_str, cursize);
		c = fgetc(fp);
		if (isspace(c)) {
			ungetc(c, fp);
			loc_str[cursize - 1] = '\0';
			break;
		}
	}
	return loc_str;
}

/* Check if token read from file is a series of 48 (ascii for '0')*/
int load_is_str_zero(char *str) {
	int str_len, k;

	str_len = strlen(str);

	for (k = 0; k < str_len; k++) {

		if (str[k] != 48) {
			return 0;
		}
	}

	return 1;
}

int is_char_num(char c) {
	if ((c >= 48) && (c <= 57)) {
		return 1;
	}
	return 0;
}

/* Check validity of token read from file
 * i.e, it's a series of chars 0-9, that can end
 * in a single dot.*/
int load_string_validity(char *str, int str_len) {
	int itr;

	for (itr = 0; itr < str_len; itr++) {
		if ((itr == str_len - 1)
				&& ((str[itr] == 46) || is_char_num(str[itr]))) {
			return 1;
		}
		if (is_char_num(str[itr])) {
			continue;
		}
		return 0;
	}
	return 0;
}

/* Check if number read is valid for current board,
 * i.e, x<=N */
int load_is_number_valid_for_board(int side_length, int num) {
	if ((num < 0) || (num > side_length)) {
		return 0;
	}
	return 1;
}

/* Get next token from file, check it's validity.
 * used iteratively in load_entries*/
void load_next_number(FILE *fp, game *local_gptr, int *flags, int i, int j) {
	char *get_str = 0;
	int str_len;
	int get_int;
	int dot = 0;

	get_str = load_get_next_token(flags, fp);

	/* if EOF reached and not enough tokens read, return error*/
	if ((flags[LOAD_IS_EOF])
			& ((j != local_gptr->cols - 1) & (i != local_gptr->rows - 1))) {
		printf("Error, invalid file format, not enough tokens provided\n");
		flags[LOAD_IS_INVALID_FORMAT] = 1;
		free(get_str);
		return;
	}

	/* if EOF reached but board filled, return not on error*/
	if (flags[LOAD_IS_EOF]) {
		free(get_str);
		return;
	}

	str_len = strlen(get_str);

	if (!load_string_validity(get_str, str_len)) {
		printf("Error, invalid token in file\n");
		flags[LOAD_IS_INVALID_FORMAT] = 1;
		free(get_str);
		return;
	}

	/* if last char is dot, flag dot - 46 is ascii for '.' */
	if ((str_len > 1) && (get_str[str_len - 1] == 46)) {
		dot = 1;
		get_str[str_len - 1] = 0;/* append stop char in place of dot*/
	}

	/* if number is zero, free string and set get_int as 0
	 * (to make up for "atoi" function bad behavior with the char '0',
	 * where there's no difference between an error and the char '0') */
	if (load_is_str_zero(get_str)) {
		get_int = 0;
	} else {
		get_int = atoi(get_str);
	}
	free(get_str);

	if (!load_is_number_valid_for_board(local_gptr->sideLength, get_int)) {
		printf("Error, invalid number, must be in range 0 <= k <= %d ",
				local_gptr->sideLength);
		flags[LOAD_IS_INVALID_FORMAT] = 1;
		return;
	}

	/* if number is zero and has a dot, it's invalid format error
	 * (fixed cells cannot be empty). note that this a different
	 * error than a single dot without a number - that's just a
	 */
	if ((get_int == 0) && (dot)) {
		printf("Error, cannot load file with fixed empty cell\n");
		flags[LOAD_IS_INVALID_FORMAT] = 1;
		return;
	}

	/* number is valid for board, write it into the local game user and flags*/
	local_gptr->user[i][j] = get_int;
	local_gptr->flag[i][j] = dot;
}

/* Checks if the two strings that were read are valid*/
int load_dimensions_are_valid(char *rows_str, char *cols_str) {
	int rows, cols;

	if (load_is_str_zero(rows_str) || load_is_str_zero(cols_str)) {
		printf("Error, dimensions must be positive integers\n");
		return 0;
	}

	rows = atoi(rows_str);
	cols = atoi(cols_str);

	if (!rows || !cols) {
		printf("Error, dimensions invalid format\n");
		return 0;
	}
	return 1;
}

/* Loads first 2 tokens from file, and checks their validity in
 * regard for valid chars and range */
int load_get_dimensions(FILE *fp, game *local_gptr, int *flags) {
	char *rows_str = 0, *cols_str = 0;
	int rows, cols;

	load_advance_whitespace(fp);

	if (feof(fp)) {
		printf("Error, not enough dimensions parameters\n");
		return 0;
	}

	rows_str = load_get_next_token(flags, fp);

	if (feof(fp)) {
		printf("Error, not enough dimensions parameters\n");
		return 0;
	}

	cols_str = load_get_next_token(flags, fp);

	if (feof(fp)) {
		printf("Error, not enough dimensions parameters\n");
		return 0;
	}

	/* if one or more tokens failed to load, or EOF reached, return error*/
	if ((rows_str == NULL) || (cols_str == NULL) || feof(fp)) {
		flags[LOAD_IS_INVALID_FORMAT] = 1;
		printf("Error, not enough dimensions parameters\n");
		return 0;
	}

	/* if 2 tokens were successfully read, check their validity*/
	if (!load_dimensions_are_valid(rows_str, cols_str)) {
		flags[LOAD_IS_INVALID_FORMAT] = 1;
		return 0;
	}

	rows = atoi(rows_str);
	cols = atoi(cols_str);

	free(rows_str);
	free(cols_str);

	if ((rows < 0) || (cols < 0)) {
		printf("Error, dimensions must be positive integers\n");
		flags[LOAD_IS_INVALID_FORMAT] = 1;

		return 0;
	}


	local_gptr->rows = rows;
	local_gptr->cols = cols;
	local_gptr->sideLength = rows * cols;


	return 1;

}

/* After loading N ints into file, check if file over, i.e,
 * if there are no more non-whitespace characters left in the
 * file after the lest int was read*/
int load_is_file_over(FILE *fp, int *flags) {

	load_advance_whitespace(fp);

	/* if there isn't an EOF after the whitespaces,
	 * return error*/
	if (!feof(fp)) {
		flags[LOAD_IS_INVALID_FORMAT] = 1;
		printf("Error, too much tokens provided in file\n");
		return 0;
	}

	return 1;

}

/* Iterate sideLength^2 to load numbers into board*/
int load_entries(game *local_gptr, FILE *fp, int *flags) {
	int i = 0, j = 0;

	for (i = 0; i < local_gptr->sideLength; i++) {
		for (j = 0; j < local_gptr->sideLength; j++) {

			/* get and write next number to board */
			load_next_number(fp, local_gptr, flags, i, j);

			if (flags[LOAD_IS_INVALID_FORMAT]) {
				return 0;
			}

		}
	}

	if (!load_is_file_over(fp, flags)) {
		return 0;
	}
	return 1;
}

/* After tring to open a file, return error if no file found
 * and reset the errno flag to 0, so it will not cause anymore
 * problems */
int load_open_file(int *flags) {
	if (errno == NO_FILE) {
		errno = 0;
		printf("Error, file not found\n");
		flags[INVALID_USER_COMMAND] = 1;
		return 0;
	}
	return 1;
}

/* Try to access file with path, read dimensions, initialize board */
void load_board(game *local_gptr, int *flags, char **strings) {
	FILE *fp;

	fp = fopen(strings[PATH], "r");

	free(strings[PATH]);
	strings[PATH] = NULL;

	if (!load_open_file(flags)) {
		return;
	}

	if (!load_get_dimensions(fp, local_gptr, flags)) {
		if (fclose(fp) == EOF) {
			printf("Error on closing file\n");
			return;
		}
		return;
	}

	init_board(local_gptr, local_gptr->rows, local_gptr->cols);

	if (load_entries(local_gptr, fp, flags)) {
		if (fclose(fp) == EOF) {
			printf("Error on closing file\n");
			return;
		}
		return;
	}
	if (fclose(fp) == EOF) {
		printf("Error on closing file\n");
		return;
	}
	flags[INVALID_USER_COMMAND] = 1;
	return;

}

/* ----------------------------------------
 * ----------------------------------------
 * ----------------------------------------
 * --------- SAVE BOARD FUNCTIONS ---------
 * ----------------------------------------
 * ----------------------------------------*/

/* get str to write from int */
char* save_get_str(int num) {
	int num_dig;
	double log_res;
	char *num_str;

	/* calculate number of chars to allocate in string,
	 * using the int provided, ceiling it's log in base 10,
	 * add 1 for terminator and convert to int*/
	log_res = (num ? log10(num) : 1);
	log_res = ceil(log_res);
	num_dig = log_res + 1;

	/* allocate chars to string (+1 null terminator!!) */
	num_str = calloc(num_dig + 1, sizeof(char));

	memory_alloc_error();

	sprintf(num_str, "%d", num);

	return num_str;

}

/* Writes first two tokens to first line of file */
void save_write_dims(FILE *fp, game *gptr) {
	char *m_str, *n_str;

	m_str = save_get_str(gptr->rows);
	n_str = save_get_str(gptr->cols);

	fprintf(fp, "%s", m_str);
	fputc(32, fp);
	fprintf(fp, "%s", n_str);
	fputc(32, fp);
	fputc(10, fp);

	free(m_str);
	free(n_str);

}

/* Write a single number to file, writing every value!=0 with a dot in edit mode
 * and every fixed number with a dot in solve mode */
void save_write_num(FILE *fp, game *gptr, int *flags, int i, int j) {
	char* to_write;
	int is_dot;
	int is_not_zero;

	is_not_zero = gptr->user[i][j];

	to_write = save_get_str(gptr->user[i][j]);

	if (flags[MODE] == MODE_EDIT) {
		/* if in EDIT mode, save every non-zero value with dot */
		is_dot = (gptr->user[i][j] != 0 ? 1 : 0);
	} else {
		/* if in solve mode, save every non-zero value with fixed flag*/
		is_dot = (gptr->flag[i][j] == 1 ? 1 : 0);
	}

	fprintf(fp, "%s", to_write);

	if (is_not_zero && is_dot) {
		fputc(46, fp);
	}

	/*  write a whitespace, even after last number in row
	 * (that's the behavior of the provided executable) */
	fputc(32, fp);

	free(to_write);

}

/* Iteratively writes lines to file */
void save_write_lines(FILE *fp, game *gptr, int *flags) {
	int i, j;

	for (i = 0; i < gptr->sideLength; i++) {
		for (j = 0; j < gptr->sideLength; j++) {
			save_write_num(fp, gptr, flags, i, j);
		}

		putc(10, fp);
	}

}

/* Receives a VALID board to write, in regard to current mode,
 * and writes to path, if exists*/
void save_board(game *gptr, int *flags, char **strings) {
	FILE *fp;

	fp = fopen(strings[PATH], "w");

	free(strings[PATH]);
	strings[PATH] = NULL;

	if (fp == NULL) {
		printf("Error, could not find path\n");
		flags[INVALID_USER_COMMAND] = 1;
		return;
	}


	save_write_dims(fp, gptr);

	save_write_lines(fp, gptr, flags);

	if (fclose(fp) == EOF) {
		printf("Error on closing file\n");
		return;
	}

}
