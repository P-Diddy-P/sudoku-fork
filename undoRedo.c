#include "undoRedo.h"


void free_node(node *delNode) {
	int i;

	free(delNode->flagArray);
	for (i=0; i<delNode->changeLength; i++) {
		free(delNode->changes[i]);
	}
	free(delNode->changes);
	free(delNode);
}


void terminate(node *currentNode) {
	node *nextNode;

	while (currentNode != NULL) {
		nextNode = currentNode->next;
		printf("freeing node: %p => %d\n", currentNode, currentNode->wholeBoard);
		free_node(currentNode);
		currentNode = nextNode;
	}
}


node *append(node *current, int *flags, int wbFlag, int **data, int cl) {
	node *newNode = malloc(sizeof(node));
	newNode->flagArray = flags;
	newNode->wholeBoard = wbFlag;
	newNode->changes = data;
	newNode->changeLength = cl;

	newNode->prev = current;
	newNode->next = NULL;
	if (current != NULL) {
		if (current->next != NULL) {
			terminate(current->next);
		}
		current->next = newNode;
	}
	return newNode;
}


void undoRedo(game *gptr, node *currentMove, int undo) { /* TODO add consideration for flags */
	/* TODO
	 * TODO
	 * TODO
	 * implement
	 * TODO
	 * TODO
	 * TODO
	 */
}


node *commit_move(node *currentNode, game *gptr, int **oldBoard, int *flags) {
	int i, j, k, countDiff = 0;
	int **nodeBoard;

	if (oldBoard == NULL) {
		/* Special case: if this is the first move and there is no old board,
		 * always copy the whole board */
		copy_board(gptr, nodeBoard, 0);
		return append(currentNode, flags, 1, nodeBoard, gptr->sideLength);
	}

	for (i=0; i<gptr->sideLength; i++) {
		for (j=0; j<gptr->sideLength; j++) {
			if (gptr->user[i][j] != oldBoard[i][j]) {
				countDiff++;
			}
		}
	}

	if (countDiff*4 > gptr->sideLength*gptr->sideLength) {
		/* if storing individual differences costs more space than storing the whole board
		 * simply store the board */
		copy_board(gptr, nodeBoard, 0);
		return append(currentNode, flags, 1, nodeBoard, gptr->sideLength);

	} else {
		/* storing individual differences costs less. Get individual differences and store them */
		nodeBoard = malloc(countDiff * sizeof(int *));
		k = 0; /*  */
		for (i=0; i<gptr->sideLength; i++) {
				for (j=0; j<gptr->sideLength; j++) {
					if (gptr->user[i][j] != oldBoard[i][j]) {
						nodeBoard[k] = malloc(4 *sizeof(int));
						nodeBoard[k][0] = i;
						nodeBoard[k][1] = j;
						nodeBoard[k][2] = oldBoard[i][j];
						nodeBoard[k][3] = gptr->user[i][j];

						k++;
					}
				}
			}

		return append(currentNode, flags, 0, nodeBoard, countDiff);
	}
}
