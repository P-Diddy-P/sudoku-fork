#include "gameStruct.h"

/* TODO PRIVATE FUNCTIONS - SHOULD NOT BE INCLUDED IN HEADER FILE TODO */

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

/* TODO PUBLIC FUNCTIONS - INCLUDE IN HEADER FILE TODO */

void initBoard(game * gptr, int rows, int cols) {
	/* Construct a game board with given rows and cols per block */
	int i, sideLength = rows*cols;

	gptr->cols = cols;
	gptr->rows = rows;

	gptr->user = malloc(sideLength * sizeof(int *));
	gptr->flag = malloc(sideLength * sizeof(int *));

	for (i=0; i<sideLength; i++) {
		gptr->user = malloc(sideLength * sizeof(int));
		gptr->flag = malloc(sideLength * sizeof(int));
	} /* TODO currently calloc returns null pointer for columns. Check why */

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
		for (i=0; i<sideLength; i++) {
			if (gptr->user[i] == NULL || gptr->flag[i] == NULL) {
				printf("Column pointer memory allocation failed, got null pointer.\n");
				exit(1);
			}
		}
	}

}

void printBoard(game *gptr) {
	printf("Starting printboard\n");
	int sideLength = gptr->rows * gptr->cols;
	int dashLength = 4*sideLength + gptr->rows + 1;
	int i, j; /* iteration variables: i for current row, j for current column */

	for (i=0; i<sideLength; i++) {
		if (i % gptr->rows == 0) {
			printDashes(dashLength);
		}

		for (j=0; j<sideLength; j++) {
			if (j % gptr->cols == 0) {
				putchar('|');
			}
			printf(" %2d%c", gptr->user[i][j] ? gptr->user[i][j] : 0 , getControlChar(gptr, i, j));
		}
		printf("|\n");
	}
	printDashes(dashLength);
}
