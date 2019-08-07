#include "stackTracking.h"
#include "gameStruct.h"

void stackInit(stack *stkptr) {
	stkptr->totalSize = STACK_INIT_SIZE;
	stkptr->usedSize = 0;
	stkptr->stackArray = malloc(stkptr->totalSize * sizeof(int *));
	if (errno || stkptr->stackArray == NULL) {
		printf("Error during memory allocation.%s\n", (stkptr->stackArray == NULL) ? "Received null pointer." : "");
		exit((errno) ? errno : 1);
	}
}


void stackDiminish(stack *stkptr) {
	/* printf("    reducing stack size by %d\n", STACK_ENLARGE_RATE); */

	stkptr->totalSize = stkptr->totalSize/STACK_ENLARGE_RATE;
	stkptr->stackArray = realloc(stkptr->stackArray, stkptr->totalSize * sizeof(int *));
	if (errno || stkptr->stackArray == NULL) {
		printf("Error during memory allocation.%s\n", (stkptr->stackArray == NULL) ? "Received null pointer." : "");
		exit((errno) ? errno : 1);
	}
}


void stackEnlarge(stack *stkptr) {
	/* printf("    increasing stack size by %d\n", STACK_ENLARGE_RATE); */

	stkptr->totalSize = stkptr->totalSize*STACK_ENLARGE_RATE;
	stkptr->stackArray = realloc(stkptr->stackArray, stkptr->totalSize * sizeof(int *));
	if (errno || stkptr->stackArray == NULL) {
		printf("Error during memory allocation.%s\n", (stkptr->stackArray == NULL) ? "Received null pointer." : "");
		exit((errno) ? errno : 1);
	}
}


void stackPush(stack *stkptr, int row, int col, int value) {
	int *newMember = malloc(3 * sizeof(int));
	*newMember = row;
	*(newMember + 1) = col;
	*(newMember + 2) = value;

	if (stkptr->usedSize >= stkptr->totalSize*STACK_INC_THRESHOLD) {
		/* printf("    requiring stack resize. using %d out of %d cells", stkptr->usedSize, stkptr->totalSize); */
		stackEnlarge(stkptr);
	}
	stkptr->usedSize++;
	stkptr->stackArray[stkptr->usedSize - 1] = newMember;
}


void stackPop(stack *stkptr, int *dstptr) {
	stackPeek(stkptr, dstptr);
	if ((stkptr->usedSize <= stkptr->totalSize*STACK_DEC_THRESHOLD) && (stkptr->totalSize > STACK_INIT_SIZE)) {
		/* printf("    requiring stack resize. using %d out of %d cells", stkptr->usedSize, stkptr->totalSize); */
		stackDiminish(stkptr);
	}
	if (stkptr->usedSize > 0) {
		free(stkptr->stackArray[stkptr->usedSize - 1]);
		stkptr->usedSize--;
	}
}


void stackPeek(stack *stkptr, int *dstptr) {
	int i;

	if (stkptr->usedSize <= 0) {
		for (i=0; i<3; i++) {
			*(dstptr + i) = -1;
		}
	} else {
		for (i=0; i<3; i++) {
			*(dstptr + i) = stkptr->stackArray[stkptr->usedSize - 1][i];
		}
	}
}


int stackTracking(game *gptr) {
	int possibleSolutions = 0;

	if (boardHasErrors(gptr)) { /* If there are errors in board, there are no solutions */
		return 0;
	}

	/* fill in stack logic */

	return possibleSolutions;
}
