#include "undoRedo.h"
#include "auxi.h"

void copy_1d_array(int *copy_to, int*copy_from, int num_elements) {
	int i;

	for (i = 0; i < num_elements; i++) {
		copy_to[i] = copy_from[i];
	}

}

void free_node(node *delNode) {
	int i;

	printf("In free_node\n");


	if (delNode==NULL){
		printf("ERROR, DELNODE IS NULL! EXITING...\n");
		exit(0);
	}

	free(delNode->flagArray);
	printf("flagArray freed\n");
	for (i = 0; i < delNode->changeLength; i++) {
		free(delNode->changes[i]);
		printf("change %d freed\n",i);
	}
	free(delNode->changes);
	printf("changes freed\n");
	free(delNode);
}

void terminate(node *currentNode) {
	printf("Terminating forward...\n");

	node *nextNode;

	while (currentNode != NULL) {

		nextNode = currentNode->next;
		printf("freeing node: %p => %d\n", currentNode,
				currentNode->wholeBoard);

		printf("Before free node\n");

		free_node(currentNode);
		currentNode = nextNode;
	}
}

/* Free all nodes from current node and backwards */
void terminate_back(node *currentNode) {
	printf("Terminating forward...\n");

	node *prevNode;

	while (currentNode != NULL) {

		prevNode = currentNode->prev;
		printf("freeing node: %p => %d\n", currentNode,
				currentNode->wholeBoard);
		free_node(currentNode);
		currentNode = prevNode;
	}
}

void terminate_all(node *currentNode) {
	printf("Terminating all\n");

	terminate(currentNode);
	terminate_back(currentNode);
	free_node(currentNode);
}

void append(node **current, int *flags, int wbFlag, int **data, int cl) {
	printf("In append\n");

	node *newNode = malloc(sizeof(node));
	newNode->flagArray = calloc(NUM_FLAGS,sizeof(int));
	copy_1d_array(newNode->flagArray,flags,NUM_FLAGS);
	newNode->wholeBoard = wbFlag;
	newNode->changes = data;
	newNode->changeLength = cl;

	newNode->prev = (*current);
	newNode->next = NULL;

	printf("New node allocated\n");

	if ((*current) != NULL) {
		if ((*current)->next != NULL) {
			printf("Terminating next nodes...\n");
			terminate((*current)->next);
		}
		printf("Setting next node\n");
		(*current)->next = newNode;
		printf("Next node set\n");
	}

	printf("First node in list\n");

	*current = newNode;

}

void apply_changes(game *gptr, int **changes, int changesLength, int undo) {
	int i;

	/* for every change in the list, copy to current board the old value if it's a undo
	 * or the "new" value, if it's a redo */
	for (i = 0; i < changesLength; i++) {
		gptr->user[changes[i][0]][changes[i][1]] = (
				undo ? changes[i][2] : changes[i][3]);
	}
}

/* Reverts game pointer and flags to previous state in case of*/
void undoRedo(game *gptr, node *currentMove, int *flags, int undo) {

	printf("In undoRedo, op is %s\n", (undo ? "undo" : "redo"));

	/* local node that will hold currentMove.next if redo
	 * xor currentMove.prev if undo */
	node *local_node;

	/* if undo and no previous move or redo and no next, return error*/
	if ((!undo && currentMove->next == NULL)
			|| (undo && currentMove->prev == NULL)) {
		printf("Error, cannot %s move, no %s move available\n",
				(undo ? "undo" : "redo"), (undo ? "previous" : "next"));
		flags[INVALID_USER_COMMAND] = 1;
		return;
	}

	if (undo) {
		local_node = currentMove->prev;
	} else {
		local_node = currentMove->next;
	}

	/* if next/prev node is a whole board node, copy values from board saved in board.
	 * why not freeing current user board: suppose there's a undo with whole board, and then
	 * another with changes, and then a move is done, deleting the redo moves.
	 * then the pointer which now serves as gptr->user will be freed, causing an error */
	if (local_node->wholeBoard) {
		printf("Copying board...\n");
		copy_board(gptr, local_node->changes, 1, 0);
	}
	/* if not whole board, call apply_changes function*/
	else {
		printf("Applying changes...\n");
		apply_changes(gptr, local_node->changes, local_node->changeLength, 1);
	}
	/* copy flags*/
	copy_1d_array(flags, local_node->flagArray, NUM_FLAGS);

	/*TODO what about flag board in gptr? do the flags need to be copied as well?*/

	/* currentMove pointer now points to previous move*/
	currentMove = local_node;

}

void commit_move(node *currentNode, game *gptr, int **oldBoard, int *flags) {
	int i, j, k, countDiff = 0;
	int **nodeBoard = NULL;

	printf("In commit move\n");

	printf("dimensions: %d\n",gptr->sideLength);

	if (oldBoard == NULL) {
		printf("Old board is NULL\n");
		/* Special case: if this is the first move and there is no old board,
		 * always copy the whole board */
		printf("Allocating space for nodeBoard\n");
		nodeBoard = init_2d_array(gptr->sideLength);
		printf("Space allocated.\nCopying board...\n");
		copy_board(gptr, nodeBoard, 0, 0);
		printf("Board copied\nAppending to list...\n");
		append(&currentNode, flags, 1, nodeBoard, gptr->sideLength);
		printf("Appended\n");
		/* TODO possible bug with changeLength argument, shoudln't be sideLength^2?*/
		return;
	}



	for (i = 0; i < gptr->sideLength; i++) {
		for (j = 0; j < gptr->sideLength; j++) {
			if (gptr->user[i][j] != oldBoard[i][j]) {
				countDiff++;
			}
		}
	}

	if (countDiff * 4 > gptr->sideLength * gptr->sideLength) {
		/* if storing individual differences costs more space than storing the whole board
		 * simply store the board */
		copy_board(gptr, nodeBoard, 0, 0);
		append(&currentNode, flags, 1, nodeBoard, gptr->sideLength);
		return;
		/* TODO possible bug with changeLength argument, shoudln't be sideLength^2?*/

	} else {
		/* storing individual differences costs less. Get individual differences and store them */
		nodeBoard = malloc(countDiff * sizeof(int *));
		k = 0; /*  */
		for (i = 0; i < gptr->sideLength; i++) {
			for (j = 0; j < gptr->sideLength; j++) {
				if (gptr->user[i][j] != oldBoard[i][j]) {
					nodeBoard[k] = malloc(4 * sizeof(int));
					nodeBoard[k][0] = i;
					nodeBoard[k][1] = j;
					nodeBoard[k][2] = oldBoard[i][j];
					nodeBoard[k][3] = gptr->user[i][j];

					k++;
				}
			}
		}
		append(&currentNode, flags, 0, nodeBoard, countDiff);
	}
}

/*TODO REMOVE- for debugging purposes only*/
void print_current_node(node *Node, node *currentMove, game *gptr) {
	int i;

	printf("-------------\nPRINITING NODE\n-------------");

	if (currentMove == NULL) {
		printf("ERROR - CURRENTMOVE IS NULL! \nEXITING...\n");
		exit(0);
	}

	printf("%s", (Node->prev == NULL ? "First\n" : ""));
	printf("%s", (Node->next == NULL ? "Last\n" : ""));
	printf("%s", (Node == currentMove ? "Current\n" : ""));

	if (Node->wholeBoard) {
		print_board_matrix(Node->changes, gptr->cols, gptr->rows,
				gptr->sideLength);
	} else {
		for (i = 0; i < Node->changeLength; i++) {
			printf("Change %d: [i=%d,j=%d,old=%d,new=%d]\n", i + 1,
					Node->changes[i][0], Node->changes[i][1],
					Node->changes[i][2], Node->changes[i][3]);
		}
	}

}

/*TODO REMOVE- for debugging purposes only*/

void print_history(node *currentMove, game *gptr) {

	node *current;

	printf("Printing moves history...\n");

	if (currentMove==NULL || currentMove->changes==NULL){
		printf("ERROR, CURRENTMOVE NOT INITIALIZED PROPERLY, TERMINATING ON EXIT...\n");
		exit(0);
	}

	current = currentMove;

	while (current->prev != current && current->prev != NULL) {
		printf("reverting to first node\n");
		current = current->prev;
	}
	printf("after reverting\n");
	while (current->next != NULL) {
		printf("printing nodes...\n");
		print_current_node(current, currentMove, gptr);
	}
	printf("reached end of print history\n");

}
