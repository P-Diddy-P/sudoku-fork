/*
 * userOp.h
 *
 *  Created on: 4 Aug 2019
 *      Author: itait
 *
 *Module purpose:
 *	 Includes all operations defined in the game.
 *	 The public function user_op receives the game pointer, flags and strings
 *	 and operates upon the current board or prints to user the appropriate error message.
 *	 The module is implemented by implementing the various operations as functions and
 *	 helper functions, using the file i/o module and solver module.
 *	 The general functions of operations are then stored in array of
 *	 function pointers, and the public function user_op is using that
 *	 array to call the appropriate operation from the array on the board.
 *
 *	 No need to check if mode is valid, being checked in parse module
 */

#ifndef USEROP_H_
#define USEROP_H_

#include "fileIO.h"
#include "autocomplete.h"
#include "stackTracking.h"
#include "undoRedo.h"
#include "ILPsolver.h"
#include "LPsolver.h"

/* every operation-function receives as arguments the game pointer, flags int array,
 * strings array and currentMove pointer */
# define NUM_OPS 17

void user_op(game *gptr, int *flags, char **strings, node **currentMove,GRBenv *env);

void print_art();

#endif /* USEROP_H_ */
