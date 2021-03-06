#include "undoRedo.h"
#include "auxi.h"

/* copy changes to gptr->user, according to undo or redo
 * in undo, copy values from current old values
 * in redo, copy values from new */
int copy_changes(game *gptr, node *changeNode, int undo) {
	int ind, change_i, change_j;

	if (changeNode->changesLen == 0) {
		return 0;
	}

	for (ind = 0; ind < changeNode->changesLen; ind++) {
		change_i = changeNode->changes[ind][0];
		change_j = changeNode->changes[ind][1];
		if (undo) {
			gptr->user[change_i][change_j] = changeNode->changes[ind][2];
		} else {
			gptr->user[change_i][change_j] = changeNode->changes[ind][3];
		}
	}

	return 1;
}

/* return number of changes made from previous move*/
int count_diffs(int **old_board, int **new_board, int sideLength) {
	int i, j, counter = 0;

	for (i = 0; i < sideLength; i++) {
		for (j = 0; j < sideLength; j++) {
			if (old_board[i][j] != new_board[i][j]) {
				counter++;
			}
		}
	}

	return counter;
}

/* return a list of quad-arrays [[i_1,j_1,old_1,new_1],[i_2,j_2,old_2,new_2],...]
 * if there are changes, NULL if there are no changes */
int** get_changes_list(int **old_board, int **new_board, int sideLength,
		int *changes_num) {
	int i, j, changes_counter, change_index = 0;
	int **changes;

	changes_counter = count_diffs(old_board, new_board, sideLength);
	*changes_num = changes_counter;

	if (changes_num == 0) {
		return NULL;
	}

	changes = calloc(changes_counter, sizeof(int *));
	memory_alloc_error();

	for (i = 0; i < sideLength; i++) {
		for (j = 0; j < sideLength; j++) {
			if (old_board[i][j] != new_board[i][j]) {
				changes[change_index] = calloc(4, sizeof(int));
				memory_alloc_error();

				changes[change_index][0] = i;
				changes[change_index][1] = j;
				changes[change_index][2] = old_board[i][j];
				changes[change_index][3] = new_board[i][j];

				change_index++;
			}
		}
	}

	return changes;
}

/* print changes from currentMove node */
void print_changes(int **changes, int changes_num, int undo) {
	int itr;

	if (changes_num == 0) {
		printf("No cells were changed\n");
		return;
	}

	for (itr = 0; itr < changes_num; itr++) {
		printf("Row: %d, Col: %d changed from %d to %d\n", changes[itr][0] + 1,
				changes[itr][1] + 1, (undo ? changes[itr][3] : changes[itr][2]),
				(undo ? changes[itr][2] : changes[itr][3]));
	}
}

/* frees the pointer allocated for the struct - flag and nodeBoard,
 * and the node itself. assumes node allocated,
 * TODO - what error should be returned? */
void free_node(node *delNode) {
	free(delNode->flagArray);
	free_2d_array(delNode->changes, delNode->changesLen);
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
void append(node **current, int **changes, int *flags, int isFirst,
		int changesLen) {
	node *newNode = calloc(1, sizeof(node));
	memory_alloc_error();
	newNode->flagArray = calloc(NUM_FLAGS, sizeof(int));
	memory_alloc_error();

	copy_1d_array(newNode->flagArray, flags, NUM_FLAGS);

	if (isFirst) {
		newNode->changesLen = 0;
		newNode->noChanges = 1;
		newNode->changes = NULL;
		newNode->prev = NULL;
		newNode->next = NULL;
		*current = newNode;
		return;
	}

	if (changesLen == 0) {
		newNode->noChanges = 1;
	} else {
		newNode->noChanges = 0;
		newNode->changes = changes;
		newNode->changesLen = changesLen;
	}

	/* set previous of new node as current, and next as NULL.
	 * this way, the previous pointer of the first node in an
	 * undoRedo list points to itself, and the next pointer of
	 * the last node points to NULL */
	newNode->prev = (*current);
	newNode->next = NULL;

	if (!isFirst) {
		terminate((*current)->next);
		(*current)->next = newNode;
	}
	*current = newNode;
}

/* undo and redo functions. logic differs between them:
 * - undo: first copy old values, print to user the changes (if functions
 * was not called from reset), and move back pointer
 * - redo: first move current pointer forward, then copy values from new. */
int undo_aux(game *gptr, node **current, int *flags, int print_bool) {
	int changes_exist;

	if ((*current)->prev == NULL) {
		printf("Error, original board, no move to undo\n");
		return 0;
	}

	copy_1d_array(flags,((*current)->prev)->flagArray, NUM_FLAGS);
	changes_exist = copy_changes(gptr, *current, 1);

	if (print_bool) {
		print_changes((*current)->changes, (*current)->changesLen, 1);
	}

	*current = (*current)->prev;
	return changes_exist;
}

int redo_aux(game *gptr, node **current, int *flags) {
	int changes_exist;

	if ((*current)->next == NULL) {
		printf("Error, no move to redo\n");
		return 0;
	}

	*current = (*current)->next;
	copy_1d_array(flags,(*current)->flagArray, NUM_FLAGS);
	changes_exist = copy_changes(gptr, *current, 0);
	print_changes((*current)->changes, (*current)->changesLen, 0);

	return changes_exist;
}

/* finds differences between the current board and oldBoard, storing
 * said differences in a new linked list node.
 * In order to conserve space, differences will be stored in 4-tuples */
void commit_move(node **currentNode, game *gptr, int **old_board, int *flags,
		int isFirst) {
	int changes_num;
	int **changes = NULL;

	if (old_board == NULL) {
		append(currentNode, NULL, flags, isFirst, 0);
		return;
	}

	changes = get_changes_list(old_board, gptr->user, gptr->sideLength,
			&changes_num);
	append(currentNode, changes, flags, isFirst, changes_num);
}
