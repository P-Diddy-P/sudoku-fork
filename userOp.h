/*
 * userOp.h
 *
 *  Created on: 4 Aug 2019
 *      Author: itait
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

/* Deprecaded definitions for userOp array implementation

# define ARGS_PASS_FUNC gptr, flags, strings, currentMove, env
# define ARGS_DEF_FUNC game *gptr, int *flags, char **strings, node **currentMove, GRBenv *env
typedef void (*f)(game*, int*, char**, node**,GRBenv*);
*/


/*one public function user_op, receives the game and user command
 * and operates upon the game*/

void user_op(game *gptr, int *flags, char **strings, node **currentMove,GRBenv *env);

void print_art();

#endif /* USEROP_H_ */
