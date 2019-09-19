/*
 * fileIO.h
 *
 *  Created on: 6 Aug 2019
 *      Author: itait
 *
 *
 *Module purpose:
 *		Includes the functions for loading a new game and saving a current game,
 *		through it's two public functions "load_board" and "save_board".
 *		DOES NOT conduct "high order" validations of board, i.e,
 *		load_board loads a valid FORMAT board, without regard to it's
 *		validity as game. Similarly, save_board assumes that the board that he writes
 *		is a valid board in regard for board validity and game mode.
 */

#ifndef FILEIO_H_
#define FILEIO_H_

# include "gurobi_interface.h"


void load_board(game *local_gptr,int *flags,char **strings);

void save_board(game *gptr,int *flags,char **strings);

int load_is_str_zero(char *str);


#endif /* FILEIO_H_ */
