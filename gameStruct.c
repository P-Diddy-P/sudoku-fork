#include "gameStruct.h"

/***********************************************************************/
/* TODO PRIVATE FUNCTIONS - SHOULD NOT BE INCLUDED IN HEADER FILE TODO */
/***********************************************************************/

/* Prints a dash character for n times then a newline character. Used to print according to format. */
void print_dashes(int n) {
	int i;

	for (i = 0; i < n; i++) {
		putchar('-');
	}
	putchar('\n');
}

/* Returns a control character to be printed after a number according to cell level flags (whether
 * a cell is fixed or contains an error), and game level flags (edit or solve mode, and mark errors
 * flag). */
char get_control_char(game *gptr, int row, int col, int *flags) {
	int print_error;

	print_error = ((flags[MODE] == MODE_EDIT)
			|| (flags[MODE] == MODE_SOLVE && flags[MARK_ERRORS_FLAG]));

	if (gptr->flag[row][col] == FIXED && flags[MODE] == MODE_SOLVE) {
		return '.';
	}

	if (print_error) {
		if (gptr->flag[row][col] == ERROR) {
			return '*';
		}

	}
	return ' ';
}

/* returns an ascii character of a given digit, or the space character if that digit is zero.
 * This function implicitly assumes that it will receive values lower than 100. This complies
 * to the board print format, as can be seen here: https://moodle.tau.ac.il/mod/forum/discuss.php?d=87711 */
char get_digit_char(int value, int second_digit) {
	int digit_value;

	digit_value = (second_digit) ? value / 10 : value % 10;

	/* if the printed digit is zero, but value is 10 or above, this
	 * must be the first digit of a multiple of 10, so it should be printed */
	return (digit_value || value >= 10) ? 48 + digit_value : 32;
}

/* Checks for every value in a row whether a subsequent cell (to the checked cell right)
 * contains the same value. In case a duplicate is found, both cell coordinates are marked in
 * validation board. */
void check_row_for_errors(game *gptr, int **errorBoard, int rowId) {
	int j; /* iteration variable for current checked cell. */
	int k; /* iteration variable for j subsequent comparisons */

	for (j = 0; j < gptr->sideLength; j++) {
		if (gptr->user[rowId][j] == 0) {
			continue; /* skip 0 values for performance, those don't cause errors */
		}

		for (k = j + 1; k < gptr->sideLength; k++) {
			if (gptr->user[rowId][j] == gptr->user[rowId][k]) {

				errorBoard[rowId][j] = ERROR;
				errorBoard[rowId][k] = ERROR;
			}
		}
	}
}

/* Checks for every value in a column whether a subsequent cell (below the checked cell)
 * contains the same value. In case a duplicate is found, both cell coordinates are marked in
 * validation board. */
void check_column_for_errors(game *gptr, int **errorBoard, int colId) {
	int i; /* iteration variable for current checked cell. */
	int k; /* iteration variable for i subsequent comparisons */

	for (i = 0; i < gptr->sideLength; i++) {
		if (gptr->user[i][colId] == 0) {
			continue; /* skip 0 values for performance, those don't cause errors */
		}

		for (k = i + 1; k < gptr->sideLength; k++) {
			if (gptr->user[i][colId] == gptr->user[k][colId]) {

				errorBoard[i][colId] = ERROR;
				errorBoard[k][colId] = ERROR;
			}
		}
	}
}
/* Indexing explained:
 * si and sj mark the first cell in the block (top left). Those remain static throughout the function.
 * ci and cj mark the current cell being compared to subsequent cells (marked with coords ri and rj),
 * and are incremented according to block rules in each iteration. That means cj increases until it passed
 * a whole block worth of columns from sj, then cj is reset to equal sj, and ci is incremented, etc.
 * In order to diminish running time, every cell is compared only to cells lower and to the right, so
 * for each cell [ci, cj], the coordinates [ri, rj] are set to those of the next cell, and are incremented
 * accordint to block rules.
 *
 * ri is set to be equal to ci, unless the column of the compared cell is the last column of the block.
 * in which case the subsequent cell will be in a lower row. i.e. if (cj - sj + 1) == gptr->cols, the
 * cell to the right of [ci, cj] will overflow the block borders, so go down a row.
 *
 * rj is set to be directly to the right of cj (cj + 1), unless cj is the last column in the block, in
 * which case (cj + 1) == (sj + gptr->cols), and rj is set to sj of a lower row. */
void check_block_for_errors(game *gptr, int **errorBoard, int blockId) {
	int si = blockId - (blockId % gptr->rows);
	int sj = (blockId % gptr->rows) * gptr->cols;
	int ci, cj;
	int ri, rj;

	for (ci = si; ci < si + gptr->rows; ci++) {
		for (cj = sj; cj < sj + gptr->cols; cj++) {
			/* printf("[[testing cell[%d][%d]=%d]]\n\n", ci+1, cj+1, gptr->user[ci][cj]); */

			if (gptr->user[ci][cj] == 0) {
				/*printf("Value is 0, skipping comparisons.\n\n");*/
				continue; /* skip 0 values for performance, those don't cause errors */
			}

			for (ri = ci + (cj - sj + 1)/gptr->cols; ri < si + gptr->rows; ri++) {
				rj = (ri == ci) ? (cj + 1) % (sj + gptr->cols) : sj;

				for (/* defined above */; rj < sj + gptr->cols; rj++) {
					/* printf("    comparing [%d][%d]=(%d) ??? [%d][%d]=(%d)", ci+1, cj+1, gptr->user[ci][cj],
							ri+1, rj+1, gptr->user[ri][rj]); */
					if (gptr->user[ci][cj] == gptr->user[ri][rj]) {
						/* printf(": numbers match - marking errors."); */
						errorBoard[ci][cj] = ERROR;
						errorBoard[ri][rj] = ERROR;
					}
					/* printf("\n"); */
				}
			}
			/* printf("\n"); */
		}
	}
}

/* Next 3 functions make sure a given row/column/block does not already contain a certain value.
 * These functions should be called before entering that value to the board. And will return true(1)
 * if the value is not already in said row/column/block. */
int row_valid_value(game *gptr, int row, int value) {
	int j;
	for (j = 0; j < gptr->sideLength; j++) {
		if (gptr->user[row][j] == value) {
			return 0;
		}
	}
	return 1;
}

int col_valid_value(game *gptr, int col, int value) {
	int i;
	for (i = 0; i < gptr->sideLength; i++) {
		if (gptr->user[i][col] == value) {
			return 0;
		}
	}
	return 1;
}

int block_valid_value(game *gptr, int valueRow, int valueCol, int value) {
	int i, j;
	int startRow = valueRow - valueRow % gptr->rows, startCol = valueCol
			- valueCol % gptr->cols;

	for (i = startRow; i < startRow + gptr->rows; i++) {
		for (j = startCol; j < startCol + gptr->cols; j++) {
			if (gptr->user[i][j] == value) {
				return 0;
			}
		}
	}
	return 1;
}

/*******************************************************/
/* TODO PUBLIC FUNCTIONS - INCLUDE IN HEADER FILE TODO */
/*******************************************************/

/* 2 functions for copying 1 and 2 dimensional arrays, value by value
 * assumes memory for both pointers is already allocated
 * TODO - maybe add a null pointer mechanism */
void copy_1d_array(int *copy_to, int *copy_from, int num_elements) {
	int i;

	for (i = 0; i < num_elements; i++) {
		copy_to[i] = copy_from[i];
	}
}

void copy_2d_array(int **copy_to, int **copy_from, int side) {
	int i, j;

	for (i = 0; i < side; i++) {
		for (j = 0; j < side; j++) {
			copy_to[i][j] = copy_from[i][j];
		}
	}
}

/* Changed function arguments of copy_board to add
 * functionality. The int isFixed controls if only fixed values
 * are copied to the destination board */
void copy_board(game *gptr, int **dstBoard, int toGame, int isFixed) {
	int i, j;

	for (i = 0; i < gptr->sideLength; i++) {
		for (j = 0; j < gptr->sideLength; j++) {
			if (toGame) {
				gptr->user[i][j] = dstBoard[i][j];
			} else {
				dstBoard[i][j] = (
						isFixed ?
								(gptr->flag[i][j] == 1 ? gptr->user[i][j] : 0) :
								gptr->user[i][j]);
			}
		}
	}
}

/* remove all fixed flags of a game */
void remove_fixed_flags(game *gptr) {
	int i, j;

	for (i=0; i<gptr->sideLength; i++) {
		for (j=0; j<gptr->sideLength; j++) {
			if (gptr->flag[i][j] == FIXED) {
				gptr->flag[i][j] = VALID;
			}
		}
	}
}

int find_next_empty_cell(game *gptr, int *rowAddress, int *colAddress) {
	/* make sure the initial coordinates are not overflowing */
	if (*rowAddress > gptr->sideLength - 1
			|| *colAddress > gptr->sideLength - 1) {
		return 1;
	}

	while (gptr->user[*rowAddress][*colAddress]) {
		*rowAddress += (*colAddress + 1) / gptr->sideLength;
		*colAddress = (*colAddress + 1) % gptr->sideLength;

		if (*rowAddress > gptr->sideLength - 1
				|| *colAddress > gptr->sideLength - 1) {
			return 1; /* indicating overflow - reached the end of the board */
		}
	}
	return 0;
}

void init_board(game * gptr, int rows, int cols) {
	/* Construct a game board with given rows and cols per block */
	int i;

	gptr->cols = cols;
	gptr->rows = rows;
	gptr->sideLength = rows * cols;


	/* changed to calloc from malloc */
	gptr->user = calloc(gptr->sideLength, sizeof(int *));
	memory_alloc_error();
	gptr->flag = calloc(gptr->sideLength, sizeof(int *));
	memory_alloc_error();

	for (i = 0; i < gptr->sideLength; i++) {
		gptr->user[i] = calloc(gptr->sideLength, sizeof(int));
		memory_alloc_error();
		gptr->flag[i] = calloc(gptr->sideLength, sizeof(int));
		memory_alloc_error();
	}

	/* compound statement check for memory allocation errors */
	/* TODO: refactor to error functions in the compound statement, reduce clutter */
	{

		if (errno) {/* TODO - errno could be set to non-zero value by a wide variety of errors, consider changing logic*/
			printf("Error during game memory allocation.\n");
			exit(errno);
		}
		if (gptr->user == NULL || gptr->flag == NULL) {
			printf("Row pointer memory allocation failed, got null pointer.\n");
			exit(1);
		}
		for (i = 0; i < gptr->sideLength; i++) {
			if (gptr->user[i] == NULL || gptr->flag[i] == NULL) {
				printf(
						"Column pointer memory allocation failed, got null pointer.\n");
				exit(1);
			}
		}
	}
}

void print_board(game *gptr, int *flags) {
	int dashLength = 4 * gptr->sideLength + gptr->rows + 1;
	int i, j; /* iteration variables: i for current row, j for current column */

	for (i = 0; i < gptr->sideLength; i++) {
		if (i % gptr->rows == 0) {
			print_dashes(dashLength);
		}

		for (j = 0; j < gptr->sideLength; j++) {
			if (j % gptr->cols == 0) {
				putchar('|');
			}
			printf(" %c%c%c", get_digit_char(gptr->user[i][j], 1), get_digit_char(gptr->user[i][j], 0),
					get_control_char(gptr, i, j, flags));
		}
		printf("|\n");
	}
	print_dashes(dashLength);
}

void print_matrix(int **mat, int sideLength) {
	int i, j; /* iteration variables: i for current row, j for current column */

	for (i = 0; i < sideLength; i++) {
		for (j = 0; j < sideLength; j++) {
			printf(" %d", (mat[i][j] ? mat[i][j] : 0));
		}
		printf("\n");
	}
}

/* TODO debug function. remove from final product */
void update_board(game *gptr, int len, int *rowIds, int *colIds, int *values,
		int * flags) {
	int addFlags = !(flags == NULL);
	int r, c, i;

	for (i = 0; i < len; i++) {
		r = rowIds[i];
		c = colIds[i];
		gptr->user[r][c] = values[i];
		if (addFlags) {
			gptr->flag[r][c] = flags[i];
		}
	}
}

int update_board_errors(game *gptr) {
	int i, j, errorsInBoard = 0;
	int **validationBoard = malloc(gptr->sideLength * sizeof(int *));
	memory_alloc_error();
	for (i = 0; i < gptr->sideLength; i++) {
		validationBoard[i] = calloc(gptr->sideLength, sizeof(int *));
		memory_alloc_error();
	}

	for (i = 0; i < gptr->sideLength; i++) {
		check_row_for_errors(gptr, validationBoard, i);
		check_column_for_errors(gptr, validationBoard, i);
		check_block_for_errors(gptr, validationBoard, i);
	}
	for (i = 0; i < gptr->sideLength; i++) {
		for (j = 0; j < gptr->sideLength; j++) {
			gptr->flag[i][j] =
					(gptr->flag[i][j] == FIXED) ? FIXED : validationBoard[i][j];
			errorsInBoard =	(errorsInBoard) ? errorsInBoard : validationBoard[i][j];
		}
	}

	free_2d_array(validationBoard, gptr->sideLength);
	return errorsInBoard;
}

int board_has_errors(game *gptr) {
	int i, j;

	for (i = 0; i < gptr->sideLength; i++) {
		for (j = 0; j < gptr->sideLength; j++) {
			if (gptr->flag[i][j] == ERROR) {
				return 1;
			}
		}
	}
	return 0;
}

int check_valid_value(game *gptr, int row, int col, int value) {
	int rowValid, colValid, blockValid, tmp = gptr->user[row][col];
	gptr->user[row][col] = 0;

	rowValid = row_valid_value(gptr, row, value);
	colValid = col_valid_value(gptr, col, value);
	blockValid = block_valid_value(gptr, row, col, value);

	gptr->user[row][col] = tmp;
	return (rowValid && colValid && blockValid);
}

/* allocate memory for a 2D array of ints
 * TODO check if problematic returning pointer */
int** init_2d_array(int sideLength) {
	int i;
	int **mat;

	/*TODO remove in the end*/
	if (sideLength > 2000) {
		printf(
				"WARNING - MATRIX DIMENSIONS ARE: %d X %d, TERMINATING WITH EXIT()...\n",
				sideLength, sideLength);
		exit(0);
	}

	mat = calloc(sideLength, sizeof(int *));

	memory_alloc_error();

	for (i = 0; i < sideLength; i++) {
		mat[i] = calloc(sideLength, sizeof(int));
		memory_alloc_error();
	}


	return mat;
}

/* free a 2D array of ints */
void free_2d_array(int **mat, int length) {
	int i;

	if (mat == NULL) {
		return;
	}

	for (i = 0; i < length; i++) {
		free(mat[i]);
	}
	free(mat);
}

void free_game_pointer(game *gptr) {

	/* if gptr is NULL, return without freeing */
	if (!gptr) {
		return;
	}

	free_2d_array(gptr->user, gptr->sideLength);
	free_2d_array(gptr->flag, gptr->sideLength);

}

/* copies values of loaded game to game pointer.
 * assumes gptr user and flags were freed before call
 *
 * TODO - if we want to "plug" the pointer of the loaded board
 * to gptr, every refrence to gptr need to be changed to
 * **gptr, so the value itself of *gptr will change.
 * seems like too much work at the moment and potential
 * for many bugs.*/
void assign_game_pointer(game *gptr, game *assign) {
	gptr->user = init_2d_array(assign->sideLength);
	gptr->flag = init_2d_array(assign->sideLength);
	gptr->rows = assign->rows;
	gptr->cols = assign->cols;
	gptr->sideLength = assign->sideLength;
	copy_2d_array(gptr->flag, assign->flag, assign->sideLength);
	copy_2d_array(gptr->user, assign->user, assign->sideLength);
}

int is_board_full(game *gptr) {
	int i, j;

	/* if board contains a 0 value, return false*/
	for (i = 0; i < gptr->sideLength; i++) {
		for (j = 0; j < gptr->sideLength; j++) {
			if (!gptr->user[i][j]) {
				return 0;
			}
		}
	}

	/* else return true*/
	return 1;

}

int count_empty(int **board, int sideLength){
	int i,j, counter=0;

	for (i=0;i<sideLength;i++){
		for (j=0;j<sideLength;j++){
			if (!board[i][j]){
				counter+=1;
			}
		}
	}


	return counter;
}


