#include "stackTracking.h"

void stackInit(stack *stkptr) {
	stkptr->totalSize = STACK_INIT_SIZE;
	stkptr->usedSize = -1;
	stkptr->current = NULL;
	stkptr->stackArray = malloc(stkptr->totalSize * sizeof(int *));
	if (errno || stkptr->stackArray == NULL) {
		printf("Error during memory allocation.%s\n", (stkptr->stackArray == NULL) ? "Received null pointer." : "");
		exit((errno) ? errno : 1);
	}
}


void stackResize(stack *stkptr, int enlarge) {
	stkptr->totalSize = stkptr->totalSize + (enlarge) ? STACK_ENLARGE_SIZE : -STACK_ENLARGE_SIZE;
	stkptr->stackArray = realloc(stkptr->stackArray, stkptr->totalSize);
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

	if (stkptr->usedSize >= stkptr->totalSize) {
		stackResize(stkptr, 1);
	}

	stkptr->usedSize++;
	(stkptr->stackArray)[stkptr->usedSize] = newMember;
	stkptr->current = newMember;
}


int *stackPop(stack *stkptr) { /* TODO currently stackPop doesn't free members popped from the stack.
 	 	 	 	 	 	 	 	  find a solution. Perhaps a generic return address which values are copied to
 	 	 	 	 	 	 	 	  then the member can be deleted. Otherwise defer responsibility of nulling data to
 	 	 	 	 	 	 	 	  stacktracking function itself. */
	if (stkptr->usedSize < 0) {
		return NULL;
	}
	stkptr->usedSize--;
	stkptr->current = (stkptr->stackArray)[stkptr->usedSize];
	return stkptr->stackArray[stkptr->usedSize + 1];
}


int *stackPeek(stack *stkptr) {
	if (stkptr->usedSize < 0) {
		return NULL;
	}
	return stkptr->current;
}
