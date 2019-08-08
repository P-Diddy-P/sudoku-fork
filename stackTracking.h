
#ifndef STACKTRACKING_H_
#define STACKTRACKING_H_

#include <stdio.h>
#include <stdlib.h>
#include "gameStruct.h"

#define				STACK_INIT_SIZE		40
#define				STACK_ENLARGE_RATE	2
#define				STACK_INC_THRESHOLD	0.75
#define				STACK_DEC_THRESHOLD	0.25

typedef struct stack {

	int totalSize;
	int usedSize;
	int **stackArray;
}stack;

/* Remove those after validation. This implementation should be hidden from the user */
/*
void stack_init(stack *stkptr);
void stack_push(stack *stkptr, int row, int col, int value);
void stack_diminish(stack *stkptr);
void stack_enlarge(stack *stkptr);
void stack_pop(stack *stkptr, int *dstptr);
void stack_peek(stack *stkptr, int *dstptr);
*/
int stack_tracking(game *gptr);

#endif /* STACKTRACKING_H_ */
