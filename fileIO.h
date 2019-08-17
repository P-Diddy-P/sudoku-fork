/*
 * fileIO.h
 *
 *  Created on: 6 Aug 2019
 *      Author: itait
 */

#ifndef FILEIO_H_
#define FILEIO_H_

# include "auxi.h"
# include "gameStruct.h"


game* load_board(ARGS_DEF_FUNC);

int save_board(ARGS_DEF_FUNC);


#endif /* FILEIO_H_ */
