#include "undoRedo.h"
#include "auxi.h"

/* frees the pointer allocated for the struct - flag and nodeBoard,
 * and the node itself. assumes node allocated,
 * TODO - what error should be returned? */
void free_node(node *delNode) {

	if (!delNode) {
		printf("ERROR, DELNODE IS NULL! EXITING...\n");
		exit(0);
	}
	/* free flag array and nodeBoard */
	free(delNode->flagArray);
	free_2d_array(delNode->nodeBoard, delNode->sideLength);
	/* free node */
	free(delNode);
}

/* terminating nodes from current node forward, until the
 * last node. while loop breaks when current is NULL*/
void terminate(node *current) {
	node *nextNode;

	/* while current is not NULL - if last node
	 * it's next will be NULL, terminating */
	while (current) {
		nextNode = current->next;
		free_node(current);
		current = nextNode;
	}
}

/* termination of the entire list
 * first current is reverted to first node in list,
 * and then terminating until the last node */
void terminate_all(node *current) {
	/*if current not initialized - first move like edit or solve -
	 * then return without doing nothing */
	if (!current) {
		return;
	}

	/* revert back to first node, and then terminate forward*/
	while (current->prev != NULL) {
		current = current->prev;
	}

	/* after reverting to first node in list, terminate all nodes with forward */
	terminate(current);
}

/* allocate new node with required data
 * isFirst is used to determine if forward termination is required
 * lastly, current is derefed and assigned the address
 * of the newly created node */
void append(node **current, int **nodeBoard, int *flags, int isFirst,
		int sideLength) {
	/* allocate space for node */
	node *newNode = calloc(1, sizeof(node));

	/* allocate space for flag array ad copy flags array */
	newNode->flagArray = calloc(NUM_FLAGS, sizeof(int));
	copy_1d_array(newNode->flagArray, flags, NUM_FLAGS);

	/* set nodeBoard*/
	newNode->nodeBoard = nodeBoard;
	/* set previous of new node as current, and next as NULL.
	 * this way, the previous pointer of the first node in an
	 * undoRedo list points to itself, and the next pointer of
	 * the last node points to NULL */

	if (isFirst) {
		newNode->prev = NULL;
	} else {
		newNode->prev = (*current);
	}

	newNode->next = NULL;

	/* terminate nodes from current forward (exclusive),
	 * set current next as newNode*/
	if (!isFirst) {
		terminate((*current)->next);
		(*current)->next = newNode;
	}

	*current = newNode;
}

/* Copies a game state (flags and board) from a node adjacent to current (node->next or
 * node->prev), and set current to the node of the new state. */
void undoRedo(game *gptr, node **current, int *flags, int undo) {

	/* local node that will hold current. if redo
	 * xor current.prev if undo */
	node *local_node;

	/* if undo and no previous move or redo and no next, return error*/
	if ((!undo && (*current)->next == NULL)
			|| (undo && (*current)->prev == NULL)) {
		printf("Error, no move available to %s\n",
				(undo ? "undo" : "redo"));
		flags[INVALID_USER_COMMAND] = 1;
		return;
	}

	if (undo) {
		local_node = (*current)->prev;
	} else {
		local_node = (*current)->next;
	}

	/* copy board - TODO what about flag board?
	 * maybe update errors after allocation */
	copy_board(gptr, local_node->nodeBoard, 1, 0);

	/* copy flags */
	copy_1d_array(flags, local_node->flagArray, NUM_FLAGS);

	/* current pointer now points to previous move */
	*current = local_node;
}

/* allocate new nodeBoard from recently changed board
 * append new node with recent data */
void commit_move(node **current, game *gptr, int *flags, int isFirst) {
	int **nodeBoard = NULL;

	nodeBoard = init_2d_array(gptr->sideLength);
	copy_board(gptr, nodeBoard, 0, 0);
	append(current, nodeBoard, flags, isFirst, gptr->sideLength);
}

/* Prints the difference in nodeBoards between source node and destination node.
 * If no changes were detected between the nodes, indicates that by a printed message.
 * TODO discuss message format. */
void print_diff_nodes(node *source,node *dest){
	int i, j, changeExist = 0;
	int **sourceBoard = source->nodeBoard;
	int **destBoard = dest->nodeBoard;

	for (i=0; i<source->sideLength; i++) {
		for (j=0; j<source->sideLength; j++) {
			printf("	Cell [%d, %d] changed from %d to %d.\n", j, i, sourceBoard[i][j], destBoard[i][j]);
			changeExist = 1;
		}
	}

	if (!changeExist) {
		printf("	No changes to the board.\n");
	}
}


/*---------debugging-----------------------------------------------------*/
/*TODO REMOVE- for debugging purposes only*/
void print_current_node(node *Node, node *current, game *gptr) {
	int j;

	if (current == NULL) {
		printf("ERROR - current IS NULL! \nEXITING...\n");
		exit(0);
	}

	printf("%s", (Node->prev == NULL ? "First\n" : ""));
	printf("%s", (Node->next == NULL ? "Last\n" : ""));
	printf("%s", (Node == current ? "Current\n" : ""));

	print_matrix(Node->nodeBoard, gptr->sideLength);
	if (!(Node->next == NULL)) {
		for (j = 0; j < 4; j++) {
			printf("\t||\n");
		}
		printf("\t\\/");
	}

}

/*TODO REMOVE- for debugging purposes only*/
void print_history(node *currentNode, game *gptr) {
	int k;
	node *current;

	printf("--------MOVES HISTORY-------\n");
	if (currentNode->next == NULL && currentNode->prev == NULL) {
		printf("Single node in list, printing..\n");
		print_current_node(currentNode, currentNode, gptr);
		return;
	}

	current = currentNode;
	while (current->prev != NULL) {

		if (k > 50) {
			printf("More than 50 iterations, probably error, exiting...\n");
			exit(0);
		}

		current = current->prev;
		k++;
	}
	k = 1;
	while (current != NULL) {
		if (k > 50) {
			printf("More than 50 iterations, probably ERROR...\n");
			exit(0);
		}
		printf("\n---------Move %d----------\n", k);
		print_current_node(current, currentNode, gptr);
		current = current->next;
		k++;
	}

	printf("--------END OF MOVES HISTORY-------\n");

}
