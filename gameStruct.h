#ifndef GAMESTRUCT_H_
#define GAMESTRUCT_H_

# include "auxi.h"
# include "errors.h"
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
 *
 *
 * Changed: added currentMove pointer to game struct
 * */


typedef struct game {

	int rows;
	int cols;
	int sideLength;
	int **user;
	int **flag;
}game;

/* gameStruct.c functions */

void copy_board(game *gptr, int **dstBoard, int toGame,int isFixed);
/* Copies the current user board to\from a two dimensional array. NOTE: dstBoard must be initialized
 * to the correct size before being used in this function. */

void init_board(game * gptr, int rows, int cols);
/* Initialize an empty game board and returns a pointer to it
 * NOTE - pointer passed to init_board must be already initialized.
 * gptr pointing to null will result in undefined behavior */

void print_board(game *gptr, int *flags);
/* Prints the game board, including error marks and fixed values */

void update_board(game *gptr, int len, int *rowIds, int *colIds, int *values, int * flags);
/* Inserts values from array to the board for debug purposes. a flags array doesn't have to
 * be provided (can be replaced by null pointer) */

int update_board_errors(game *gptr);
/* Checks the entire board for errors in row, column and block and returns 0 for no errors, 1 otherwise.
 * Function also updates non-fixed cells to have an error flag if they have a duplicate value, and removes
 * error flags for cells with no value or cells with no duplicate value in their row.
 *
 * checkForErrors replaces the board entirely with valid/error values, regardless of old validity values. */

int board_has_errors(game *gptr);
/* Checks flag board for errors. Returning true if any cell in the board has ERROR flag, false otherwise.
 * Assumes the board is updated, i.e. every ERROR flagged cell corresponds to a cell with a problematic value.
 */

int check_valid_value(game *gptr, int row, int col, int value);
/*
 * Checks if setting value is [row][col] in the board keeps the board valid. Runs faster than checkForErrors
 * (linear instead of quadratic), but compares all row/column/block values to a single given value. In case
 * row/col/block already have errors, the value is invalid.
 *
 * NOTE: checkValidValue compares all values in a certain row/col/block to a POSSIBLE value, which is not necessarily
 * on the board. The value in gptr->user[row][col] is removed for the duration of the function, then returned.
 */

int find_next_empty_cell(game *gptr, int *rowAddress, int *colAddress);
/* Find the next cell with no number assigned to it (i.e. 0), starting from the
 * given [*rowAddress][*colAddress] coordinates. The coordinates given to the function
 * are set to the next empty cell, mimicking call-by-reference, and the return value indicates
 * whether the end of the board was reached (return true), or not (return false).
 *
 * Note that the search is inclusive, i.e. if
 * gptr->[*rowAddress][*colAddress] == 0, then the given addresses will not change value.
 */


/* ADDED FUNCTION - free_game_pointer, freeing the dynamically allocated space
 * for a game instance*/
/* Free game pointer
 * TODO maybe move to other module like gamestruct */
void free_game_pointer(game *gptr);


/* Iterates over board and returns true if a zero valued
 * cell found*/
int is_board_full(game *gptr);

/* Functions for allocation and freeing memory of a 2D array of ints*/
void free_2d_array(int **mat,int sideLength);

int** init_2d_array(int sideLength);

int count_empty(int **board, int sideLength);

/* TODO - for debugging purposes, delete in the end */
void print_matrix(int **mat, int sideLength);

void copy_1d_array(int *copy_to, int*copy_from, int num_elements);
void copy_2d_array(int **copy_to, int **copy_from, int side);
void assign_game_pointer(game *gptr,game *assign);

int row_valid_value(game *gptr, int row, int value);
int col_valid_value(game *gptr, int col, int value);
int block_valid_value(game *gptr, int valueRow, int valueCol, int value);



#endif /* GAMESTRUCT_H_ */
