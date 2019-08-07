#ifndef STACKTRACKING_H_
#define STACKTRACKING_H_

#include <stdio.h>
#include <stdlib.h>

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
void stackInit(stack *stkptr);
void stackPush(stack *stkptr, int row, int col, int value);
void stackDiminish(stack *stkptr);
void stackEnlarge(stack *stkptr);
void stackPop(stack *stkptr, int *dstptr);
void stackPeek(stack *stkptr, int *dstptr);
int stackTracking(game *gptr);

#endif /* STACKTRACKING_H_ */
