#ifndef GAMESTRUCT_H_
#define GAMESTRUCT_H_

# include "auxi.h"
# include "errors.h"

#define					VALID  0
#define 				FIXED  1
#define					ERROR  2

/* Module and Struct purpose:
	 * A game instance consists of 2 boards: one for actual game values,
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
	 * */


typedef struct game {

	int rows;
	int cols;
	int sideLength;
	int **user;
	int **flag;
}game;


void copy_board(game *gptr, int **dstBoard, int toGame,int isFixed);

void init_board(game * gptr, int rows, int cols);

void print_board(game *gptr, int *flags);

int update_board_errors(game *gptr);

int board_has_errors(game *gptr);

int check_valid_value(game *gptr, int row, int col, int value);

int find_next_empty_cell(game *gptr, int *rowAddress, int *colAddress);

void remove_fixed_flags(game *gptr);

void free_game_pointer(game *gptr);

int is_board_full(game *gptr);

void free_2d_array(int **mat,int sideLength);

int** init_2d_array(int sideLength);

int count_empty(int **board, int sideLength);


void copy_1d_array(int *copy_to, int*copy_from, int num_elements);
void copy_2d_array(int **copy_to, int **copy_from, int side);

void assign_game_pointer(game *gptr,game *assign);

int row_valid_value(game *gptr, int row, int value);
int col_valid_value(game *gptr, int col, int value);
int block_valid_value(game *gptr, int valueRow, int valueCol, int value);



#endif /* GAMESTRUCT_H_ */
