/*
 * userOp.h
 *
 *  Created on: 4 Aug 2019
 *      Author: itait
 */

#ifndef USEROP_H_
#define USEROP_H_

# include "auxi.h"
# include "gameStruct.h"

/*one public function user_op, recieves the game and user command
 * and operates upon the game*/

int user_op(game * gptr,int *flags, char *strings[]);


#endif /* USEROP_H_ */
