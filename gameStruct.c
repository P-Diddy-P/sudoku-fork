#include "gameStruct.h"
#include "auxi.h"

/***********************************************************************/
/* TODO PRIVATE FUNCTIONS - SHOULD NOT BE INCLUDED IN HEADER FILE TODO */
/***********************************************************************/

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

void print_dashes(int n) {
	int i;

	for (i = 0; i < n; i++) {
		putchar('-');
	}
	putchar('\n');
}

/* added reference to game mode*/
char get_control_char(game *gptr, int row, int col, int *flags) {
	int print_error;

	print_error = ((flags[MODE] == MODE_EDIT)
			|| (flags[MODE] == MODE_SOLVE && flags[MARK_ERRORS_FLAG]));

	if (gptr->flag[row][col] == FIXED) {
		return '.';
	}

	if (print_error) {
		if (gptr->flag[row][col] == ERROR) {
			return '*';
		}

	}
	return ' ';
}

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

/* TODO - bug in update. example:
-------------------
|  3   0 |  4*  2*|
|  0   0 |  4*  2*|
-------------------
|  0   0 |  2   4 |
|  0   4*|  4*  1 |
-------------------
 *
 *
 *  */
/* TODO extremely convoluted indexing, consider revising */
void check_block_for_errors(game *gptr, int **errorBoard, int blockId) {
	int si = blockId - (blockId % gptr->rows); /* row of first cell in block */
	int sj = (blockId % gptr->rows) * gptr->cols; /* column of first cell in block */
	int ci, cj; /* iteration variables for the currently checked reference */
	int ri, rj; /* iteration variables for values compared to the current reference */

	for (ci = si; ci < si + gptr->rows; ci++) {
		for (cj = sj; cj < sj + gptr->cols; cj++) {
			/*printf("[[testing cell[%d][%d]=%d]]\n\n", ci+1, cj+1, gptr->user[ci][cj]);*/

			if (gptr->user[ci][cj] == 0) {
				/*printf("Value is 0, skipping comparisons.\n\n");*/
				continue; /* skip 0 values for performance, those don't cause errors */
			}

			for (ri = ci + (cj + 1) / gptr->cols; ri < si + gptr->rows; ri++) {
				rj = (ri == ci) ? (cj + 1) % gptr->cols : sj;

				for (/* defined above */; rj < sj + gptr->cols; rj++) {
					/*printf("    comparing [%d][%d]=(%d) ??? [%d][%d]=(%d)", ci+1, cj+1, gptr->user[ci][cj], ri+1, rj+1, gptr->user[ri][rj]);*/
					if (gptr->user[ci][cj] == gptr->user[ri][rj]) {
						/*printf(": numbers match - marking errors.");*/
						errorBoard[ci][cj] = ERROR;
						errorBoard[ri][rj] = ERROR;
					}
					/*printf("\n");*/
				}
			}
			/*printf("\n");*/
		}
	}
}

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

	/*TODO remove in the end*/
	if (gptr->sideLength > 2000) {
		printf(
				"WARNING - MATRIX DIMENSIONS ARE OVER %dX%d, TERMINATING WITH EXIT()...\n",
				gptr->sideLength, gptr->sideLength);
		exit(0);
	}
	/* changed to calloc from malloc */
	gptr->user = calloc(gptr->sideLength, sizeof(int *));
	gptr->flag = calloc(gptr->sideLength, sizeof(int *));

	for (i = 0; i < gptr->sideLength; i++) {
		gptr->user[i] = calloc(gptr->sideLength, sizeof(int));
		gptr->flag[i] = calloc(gptr->sideLength, sizeof(int));
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

void print_board_aux(game *gptr, int *flags) {
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
			printf(" %2d%c", gptr->user[i][j] ? gptr->user[i][j] : 0,
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

void update_random(game *gptr, int seed, int addFlags) {
	srand(seed);
	int i, j;

	for (i = 0; i < gptr->sideLength; i++) {
		for (j = 0; j < gptr->sideLength; j++) {
			gptr->user[i][j] = rand() % gptr->sideLength + 1;
			gptr->flag[i][j] = (rand() % 2) * addFlags;
		}
	}
}

int update_board_errors(game *gptr) {
	int i, j, errorsInBoard = 0;
	int **validationBoard = malloc(gptr->sideLength * sizeof(int *));
	for (i = 0; i < gptr->sideLength; i++) {
		validationBoard[i] = calloc(gptr->sideLength, sizeof(int *));
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
	int tmp = gptr->user[row][col];
	gptr->user[row][col] = 0;

	int rowValid = row_valid_value(gptr, row, value);
	int colValid = col_valid_value(gptr, col, value);
	int blockValid = block_valid_value(gptr, row, col, value);

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

	for (i = 0; i < sideLength; i++) {
		mat[i] = calloc(sideLength, sizeof(int));
	}

	if (errno) {
		printf("Error during game memory allocation.\n");
		exit(errno);
	}

	return mat;
}

/* free a 2D array of ints */
void free_2d_array(int **mat, int sideLength) {
	int i;

	if (mat == NULL) {
		return;
	}

	for (i = 0; i < sideLength; i++) {
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

