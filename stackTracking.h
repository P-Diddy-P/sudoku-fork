#ifndef STACKTRACKING_H_
#define STACKTRACKING_H_

#include "gameStruct.h"

#define				STACK_INIT_SIZE		20
#define				STACK_ENLARGE_SIZE	10

typedef struct stack {

	int totalSize;
	int usedSize;
	int *current;
	int **stackArray;
}stack;

/* Remove those after validation. This implementation should be hidden from the user */
void stackInit(stack *stkptr);
void stackPush(stack *stkptr, int row, int col, int value);
int *stackPop(stack *stkptr);
int *stackPeek(stack *stkptr);

#endif /* STACKTRACKING_H_ */
