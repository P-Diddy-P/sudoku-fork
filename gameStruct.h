#include <stdio.h>
#include <stdlib.h>

/* Constant identifiers for fixed values and error values in the board */
#define					VALID  0
#define 				FIXED  1
#define					ERROR  2

/* A game instance consists of 2 boards: one for actual game values,
 * and another board to flag values as fixed/errors (a board value can
 * be either fixed or an error, not both).
 *
 * rows and cols members indicate the number of rows and columns PER BLOCK^.
 * So if a game has rows=3 & cols=2, that means that each block has 3 rows and
 * 2 columns. in order to keep a sudoku like structure, the blocks are ordered
 * in 3 columns and 2 rows, leading to a 6-by-6 game board.
 *
 * sideLength indicates the number of cells in each row, column and block, and is
 * mostly added for convenience purposes.
 *
 * ^(TODO consider changing member name to reflect on it's block nature)
 * */
typedef struct game {

	int rows;
	int cols;
	int sideLength;
	int **user;
	int **flag;
}game;

/* gameStruct.c functions */

void initBoard(game * gptr, int rows, int cols);
/* Initialize an empty game board and returns a pointer to it */

void printBoard(game *gptr);
/* Prints the game board, including error marks and fixed values */

void updateBoard(game *gptr, int len, int *rowIds, int *colIds, int *values, int * flags);
/* Inserts values from array to the board for debug purposes. a flags array doesn't have to
 * be provided (can be replaced by null pointer) */

void updateRandom(game *gptr, int seed, int addFlags);
/* Fills the board entirely with random values according to a predefined seed. Can also fill
 * flags randomly. NOTE: addFlags must be binary (0 or 1)! */

int checkForErrors(game *gptr);
/* Checks the entire board for errors in row, column and block and returns 0 for no errors, 1 otherwise.
 * Function also updates non-fixed cells to have an error flag if they have a duplicate value, and removes
 * error flags for cells with no value or cells with no duplicate value in their row.
 *
 * checkForErrors replaces the board entirely with valid/error values, regardless of old validity values. */
