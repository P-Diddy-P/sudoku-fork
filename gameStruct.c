#include "gameStruct.h"


/***********************************************************************/
/* TODO PRIVATE FUNCTIONS - SHOULD NOT BE INCLUDED IN HEADER FILE TODO */
/***********************************************************************/


void printDashes(int n) {
	int i;

	for (i=0; i<n; i++) {
		putchar('-');
	}
	putchar('\n');
}


char getControlChar(game *gptr, int row, int col) {
	if (gptr->flag[row][col] == FIXED) { /* TODO add game mode to condition? */
		return '.';
	}
	if (gptr->flag[row][col] == ERROR) {
		return '*';
	}
	return ' ';
}


void checkRowForErrors(game *gptr, int **errorBoard, int rowId) {
	int j; /* iteration variable for current checked cell. */
	int k; /* iteration variable for j subsequent comparisons */

	for (j=0; j<gptr->sideLength; j++) {
		if (gptr->user[rowId][j] == 0) {
			continue; /* skip 0 values for performance, those don't cause errors */
		}

		for (k=j+1; k<gptr->sideLength; k++) {
			if (gptr->user[rowId][j] == gptr->user[rowId][k]) {

				errorBoard[rowId][j] = ERROR;
				errorBoard[rowId][k] = ERROR;
			}
		}
	}
}


void checkColumnForErrors(game *gptr, int **errorBoard, int colId) {
	int i; /* iteration variable for current checked cell. */
	int k; /* iteration variable for i subsequent comparisons */

	for (i=0; i<gptr->sideLength; i++) {
		if (gptr->user[i][colId] == 0) {
			continue; /* skip 0 values for performance, those don't cause errors */
		}

		for (k=i+1; k<gptr->sideLength; k++) {
			if (gptr->user[i][colId] == gptr->user[k][colId]) {

				errorBoard[i][colId] = ERROR;
				errorBoard[k][colId] = ERROR;
			}
		}
	}
}


void checkBlockForErrors(game *gptr, int **errorBoard, int blockId) {
	int si = blockId - (blockId % gptr->rows); /* row of first cell in block */
	int sj = (blockId % gptr->rows) * gptr->cols; /* column of first cell in block */
	int ci, cj;
	int ri, rj;

	for (ci=si; ci<si+gptr->rows; ci++) { /* TODO extremely convoluted iteration, consider revising */
		for (cj=sj; cj<sj+gptr->cols; cj++) {
			printf("[[testing cell[%d][%d]=%d]]\n\n", ci, cj, gptr->user[ci][cj]);

			if (gptr->user[ci][cj] == 0) {
				printf("Value is 0, skipping comparisons.\n\n");
				continue; /* skip 0 values for performance, those don't cause errors */
			}

			for (ri = ci + (cj + 1)/gptr->cols; ri<si+gptr->rows; ri++) {
				rj = (ri == ci) ? (cj + 1) % gptr->cols : sj;

				for (/* defined above */; rj<sj+gptr->cols; rj++) {
					printf("    comparing [%d][%d]=(%d) ??? [%d][%d]=(%d)", ci, cj, gptr->user[ci][cj], ri, rj, gptr->user[ri][rj]);
					if (gptr->user[ci][cj] == gptr->user[ri][rj]) {
						printf(": numbers match - marking errors.");
						errorBoard[ci][cj] = ERROR;
						errorBoard[ri][rj] = ERROR;
					}
					printf("\n");
				}
			}
			printf("\n");
		}
	}
}


/*******************************************************/
/* TODO PUBLIC FUNCTIONS - INCLUDE IN HEADER FILE TODO */
/*******************************************************/


void initBoard(game * gptr, int rows, int cols) {
	/* Construct a game board with given rows and cols per block */
	int i;

	gptr->cols = cols;
	gptr->rows = rows;
	gptr->sideLength = rows * cols;

	gptr->user = malloc(gptr->sideLength * sizeof(int *));
	gptr->flag = malloc(gptr->sideLength * sizeof(int *));

	for (i=0; i<gptr->sideLength; i++) {
		gptr->user[i] = calloc(gptr->sideLength, sizeof(int));
		gptr->flag[i] = calloc(gptr->sideLength, sizeof(int));
	}

	/* compound statement check for memory allocation errors */
	/* TODO: refactor to error functions in the compound statement, reduce clutter */
	{
		if (errno) {
			printf("Error during game memory allocation.\n");
			exit(errno);
		}
		if (gptr->user == NULL || gptr->flag == NULL) {
			printf("Row pointer memory allocation failed, got null pointer.\n");
			exit(1);
		}
		for (i=0; i<gptr->sideLength; i++) {
			if (gptr->user[i] == NULL || gptr->flag[i] == NULL) {
				printf("Column pointer memory allocation failed, got null pointer.\n");
				exit(1);
			}
		}
	}

}

void printBoard(game *gptr) {
	int dashLength = 4*gptr->sideLength + gptr->rows + 1;
	int i, j; /* iteration variables: i for current row, j for current column */

	for (i=0; i<gptr->sideLength; i++) {
		if (i % gptr->rows == 0) {
			printDashes(dashLength);
		}

		for (j=0; j<gptr->sideLength; j++) {
			if (j % gptr->cols == 0) {
				putchar('|');
			}
			printf(" %2d%c", gptr->user[i][j] ? gptr->user[i][j] : 0 , getControlChar(gptr, i, j));
		}
		printf("|\n");
	}
	printDashes(dashLength);
}

void updateBoard(game *gptr, int len, int *rowIds, int *colIds, int *values, int * flags) {
	int addFlags = !(flags == NULL);
	int r, c, i;

	for (i=0; i<len; i++) {
		r = rowIds[i]; c = colIds[i];
		gptr->user[r][c] = values[i];
		if (addFlags) {
			gptr->flag[r][c] = flags[i];
		}
	}
}

void updateRandom(game *gptr, int seed, int addFlags) {
	srand(seed);
	int i, j;

	for (i=0; i<gptr->sideLength; i++) {
		for (j=0; j<gptr->sideLength; j++) {
			gptr->user[i][j] = rand() % gptr->sideLength + 1;
			gptr->flag[i][j] = (rand() % 2)*addFlags;
		}
	}
}

int checkForErrors(game *gptr) {
	int i, j, errorsInBoard = 0;
	int **validationBoard = malloc(gptr->sideLength * sizeof(int *));
	for (i=0; i<gptr->sideLength; i++) {
		validationBoard[i] = calloc(gptr->sideLength, sizeof(int *));
	}

	for (i=0; i<gptr->sideLength; i++) {
		checkRowForErrors(gptr, validationBoard, i);
		checkColumnForErrors(gptr, validationBoard, i);
		checkBlockForErrors(gptr, validationBoard, i);
	}
	for (i=0; i<gptr->sideLength; i++) {
		for (j=0; j<gptr->sideLength; j++) {
			gptr->flag[i][j] = (gptr->flag[i][j] == FIXED) ? FIXED : validationBoard[i][j];
			errorsInBoard = (errorsInBoard) ? errorsInBoard : validationBoard[i][j];
		}
	}
	return errorsInBoard;
}
