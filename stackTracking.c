#include "stackTracking.h"

/********************************************************/
/*     TODO STACK ADT FUNCTIONS - PRIVATE USE TODO      */
/********************************************************/

/*TODO - why not use calloc function instead of malloc?*/

void stack_init(stack *stkptr) {
	stkptr->totalSize = STACK_INIT_SIZE;
	stkptr->usedSize = 0;
	stkptr->stackArray = malloc(stkptr->totalSize * sizeof(int *));
	if (errno || stkptr->stackArray == NULL) {
		printf("Error during memory allocation.%s\n",
				(stkptr->stackArray == NULL) ? "Received null pointer." : "");
		exit((errno) ? errno : 1);
	}
}

void stack_diminish(stack *stkptr) {
	/* printf("    reducing stack size by %d\n", STACK_ENLARGE_RATE); */

	stkptr->totalSize = stkptr->totalSize / STACK_ENLARGE_RATE;
	stkptr->stackArray = realloc(stkptr->stackArray,
			stkptr->totalSize * sizeof(int *));
	if (errno || stkptr->stackArray == NULL) {
		printf("Error during memory allocation.%s\n",
				(stkptr->stackArray == NULL) ? "Received null pointer." : "");
		exit((errno) ? errno : 1);
	}
}

void stack_enlarge(stack *stkptr) {
	/* printf("    increasing stack size by %d\n", STACK_ENLARGE_RATE); */

	stkptr->totalSize = stkptr->totalSize * STACK_ENLARGE_RATE;
	stkptr->stackArray = realloc(stkptr->stackArray,
			stkptr->totalSize * sizeof(int *));
	if (errno || stkptr->stackArray == NULL) {
		printf("Error during memory allocation.%s\n",
				(stkptr->stackArray == NULL) ? "Received null pointer." : "");
		exit((errno) ? errno : 1);
	}
}

void stack_push(stack *stkptr, int row, int col, int value) {
	int *newMember = malloc(3 * sizeof(int));
	*newMember = row;
	*(newMember + 1) = col;
	*(newMember + 2) = value;

	if (stkptr->usedSize >= stkptr->totalSize * STACK_INC_THRESHOLD) {
		/* printf("    requiring stack resize. using %d out of %d cells", stkptr->usedSize, stkptr->totalSize); */
		stack_enlarge(stkptr);
	}
	stkptr->usedSize++;
	stkptr->stackArray[stkptr->usedSize - 1] = newMember;
}

void stack_peek(stack *stkptr, int *dstptr) {
	int i;

	if (stkptr->usedSize <= 0) {
		for (i = 0; i < 3; i++) {
			*(dstptr + i) = -1;
		}
	} else {
		for (i = 0; i < 3; i++) {
			*(dstptr + i) = stkptr->stackArray[stkptr->usedSize - 1][i];
		}
	}
}

void stack_pop(stack *stkptr, int *dstptr) {
	stack_peek(stkptr, dstptr);
	if ((stkptr->usedSize <= stkptr->totalSize * STACK_DEC_THRESHOLD)
			&& (stkptr->totalSize > STACK_INIT_SIZE)) {
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

void stack_free(stack *stkptr) {
	int i;

	for (i = 0; i < stkptr->usedSize; i++) {
		free(stkptr->stackArray[i]);
	}
	free(stkptr->stackArray);
}

/********************************************************/
/*  TODO STACKTRACKING FUNCTIONS - CAN BE IMPORTED TODO */
/********************************************************/

void print_top(stack *stkptr) {
	int *top = malloc(3 * sizeof(int));
	stack_peek(stkptr, top);
	printf("[%d, %d, %d] at %d / %d", top[0], top[1], top[2], stkptr->usedSize,
			stkptr->totalSize);
	free(top);
}

int stack_tracking(game *gptr) { /* TODO - build function logic with pen and paper instead of typing stupid shit */
	int possibleSolutions = 0;
	int currentRow = 0, currentCol = 0;
	int *stkres;
	stack stk;
	stack *stkptr = &stk;

	/* printf("||starting stack tracking||\n"); */
	if (board_has_errors(gptr)
			|| find_next_empty_cell(gptr, &currentRow, &currentCol)) {
		/* printf("  board %s. Exiting with 0 solutions.\n", (board_has_errors(gptr) ? "has errors" : "is full"));
		 printf("||stack tracking finished\n"); */
		return 0;
		/* if the board contains errors, or no cell is empty, then there are no possible solutions (we will
		 * assume the board is not solved correctly when counting solutions. if both conditions are false, we still
		 * get the coordinates of the first empty cell in board from find_next_empty_cell */
	}

	stack_init(stkptr);
	stkres = malloc(3 * sizeof(int));
	stack_push(stkptr, currentRow, currentCol, 1);

	while (!stack_empty(stkptr)) {
		stack_peek(stkptr, stkres);
		/*printf("  setting value [[%d][%d]=%d]\n", stkres[0], stkres[1], stkres[2]);*/
		currentRow = stkres[0];
		currentCol = stkres[1];
		gptr->user[currentRow][currentCol] = stkres[2];

		/* If top value (i,j,val) in stack is valid, i.e, does not collide with other cells,
		 * check if it's the last one - if true, it's a solution. else, push and continue.
		 * Note that find_next_empty_cell receives as arguments the current row and col, and updates them
		 * in the process, so the element being pushed to stack, is the next empty cell after the last
		 * valid one, with the value 1*/
		if (check_valid_value(gptr, currentRow, currentCol, stkres[2])) {
			if (find_next_empty_cell(gptr, &currentRow, &currentCol)) {
				possibleSolutions++;
				printf("||%d||\n", possibleSolutions);
				/*printf("  found valid board. Current solutions are %d.\n", possibleSolutions);*/
				/* if we reached the end of the board, the number of solutions increments (as we found a solution),
				 * and we proceed to increment the value of the topmost member of the stack (same as with an invalid
				 * value). */
			} else {
				stack_push(stkptr, currentRow, currentCol, 1);
				/*printf("  value is valid, pushing to stack.\n");*/
				continue;
				/* if we find another empty cell, we will attempt to fill it with a valid value next, based on the
				 * current top of the stack */
			}
		}

		/* if no value was pushed to the stack, the current value must be popped. this value will be replaced by it's
		 * subsequent value, unless it already equals gptr->sideLength, in which case we will iteratively go back to the
		 * earliest value which can be incremented, emptying each cell as we go along. */
		do {
			stack_pop(stkptr, stkres);
			/*printf("    removed [[%d][%d]=%d] from the stack.\n", stkres[0], stkres[1], stkres[2]);*/
			if (stkres[0] >= 0 && stkres[1] >= 0) {
				gptr->user[stkres[0]][stkres[1]] = 0;
			}
			/* while the values in the stack are at maximal range (for example, 9 in regular game),
			 * pop the values from the stack and discard thems*/
		} while (stkres[2] >= gptr->sideLength);
		/*printf("    exited do while due to value stkres[2]=%d, relative to %d", stkres[2], gptr->sideLength);*/

		if (stkres[2] > 0 && stkres[2] < gptr->sideLength) {
			stack_push(stkptr, stkres[0], stkres[1], stkres[2] + 1);
		}

	}

	free(stkres);
	stack_free(stkptr);
	return possibleSolutions;
}
