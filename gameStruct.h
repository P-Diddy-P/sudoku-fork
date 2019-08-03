#include <stdio.h>
#include <stdlib.h>

/* Constant identifiers for fixed values and error values in the board */
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
 * ^(TODO consider changing member name to reflect on it's block nature)
 * */
typedef struct game {

	int rows;
	int cols;
	int **user;
	int **flag;
}game;

/* gameStruct.c functions */

void initBoard(game * gptr, int rows, int cols);
/* Initialize an empty game board and returns a pointer to it */

void printBoard(game *gptr);
/* Prints the game board, including error marks and fixed values */
