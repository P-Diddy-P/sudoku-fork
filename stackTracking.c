#include "stackTracking.h"

/********************************************************/
/*     TODO STACK ADT FUNCTIONS - PRIVATE USE TODO      */
/********************************************************/

void stack_init(stack *stkptr) {
	stkptr->totalSize = STACK_INIT_SIZE;
	stkptr->usedSize = 0;
	stkptr->stackArray = malloc(stkptr->totalSize * sizeof(int *));
	if (errno || stkptr->stackArray == NULL) {
		printf("Error during memory allocation.%s\n", (stkptr->stackArray == NULL) ? "Received null pointer." : "");
		exit((errno) ? errno : 1);
	}
}


void stack_diminish(stack *stkptr) {
	/* printf("    reducing stack size by %d\n", STACK_ENLARGE_RATE); */

	stkptr->totalSize = stkptr->totalSize/STACK_ENLARGE_RATE;
	stkptr->stackArray = realloc(stkptr->stackArray, stkptr->totalSize * sizeof(int *));
	if (errno || stkptr->stackArray == NULL) {
		printf("Error during memory allocation.%s\n", (stkptr->stackArray == NULL) ? "Received null pointer." : "");
		exit((errno) ? errno : 1);
	}
}


void stack_enlarge(stack *stkptr) {
	/* printf("    increasing stack size by %d\n", STACK_ENLARGE_RATE); */

	stkptr->totalSize = stkptr->totalSize*STACK_ENLARGE_RATE;
	stkptr->stackArray = realloc(stkptr->stackArray, stkptr->totalSize * sizeof(int *));
	if (errno || stkptr->stackArray == NULL) {
		printf("Error during memory allocation.%s\n", (stkptr->stackArray == NULL) ? "Received null pointer." : "");
		exit((errno) ? errno : 1);
	}
}


void stack_push(stack *stkptr, int row, int col, int value) {
	int *newMember = malloc(3 * sizeof(int));
	*newMember = row;
	*(newMember + 1) = col;
	*(newMember + 2) = value;

	if (stkptr->usedSize >= stkptr->totalSize*STACK_INC_THRESHOLD) {
		/* printf("    requiring stack resize. using %d out of %d cells", stkptr->usedSize, stkptr->totalSize); */
		stack_enlarge(stkptr);
	}
	stkptr->usedSize++;
	stkptr->stackArray[stkptr->usedSize - 1] = newMember;
}


void stack_peek(stack *stkptr, int *dstptr) {
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


void stack_pop(stack *stkptr, int *dstptr) {
	stack_peek(stkptr, dstptr);
	if ((stkptr->usedSize <= stkptr->totalSize*STACK_DEC_THRESHOLD) && (stkptr->totalSize > STACK_INIT_SIZE)) {
		/* printf("    requiring stack resize. using %d out of %d cells", stkptr->usedSize, stkptr->totalSize); */
		stack_diminish(stkptr);
	}
	if (stkptr->usedSize > 0) {
		free(stkptr->stackArray[stkptr->usedSize - 1]);
		stkptr->usedSize--;
	}
}


int stack_empty(stack *stkptr) {
	return (stkptr->usedSize < 1);
}

/********************************************************/
/*  TODO STACKTRACKING FUNCTIONS - CAN BE IMPORTED TODO */
/********************************************************/


void print_top(stack *stkptr) {
	int *top = malloc(3 * sizeof(int));
	stack_peek(stkptr, top);
	printf("[%d, %d, %d] at %d / %d", top[0], top[1], top[2], stkptr->usedSize, stkptr->totalSize);
	free(top);
}


int find_next_empty_cell(game *gptr, int *rowAddress, int *colAddress) {
	while (gptr->user[*rowAddress][*colAddress]) {
		*rowAddress += (*colAddress + 1) / gptr->sideLength;
		*colAddress = (*colAddress + 1) % gptr->sideLength;
		if (*rowAddress > gptr->sideLength || *colAddress > gptr->sideLength) {
			return 1; /* indicating overflow - reached the end of the board */
		}
	}
	return 0;
}


int stack_tracking(game *gptr) { /* TODO - build function logic with pen and paper instead of typing stupid shit */
	int possibleSolutions = 0, currentRow = 0, currentCol = 0, boardFull;
	int *stkres;
	stack stk;
	stack *stkptr = &stk;

	do {
		boardFull = find_next_empty_cell(gptr, &currentRow, &currentCol);

	} while(!stack_empty(stkptr));

	return possibleSolutions;
}
