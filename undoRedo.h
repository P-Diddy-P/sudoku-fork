#ifndef UNDOREDO_H_
#define UNDOREDO_H_

/* Module purpose:
 * implements the undo/redo functionality using a double linked list.
 * */

#include "gameStruct.h"

typedef struct node {

	struct node *next;
	struct node *prev;
	int *flagArray;
	int **changes;
	int changesLen;
	int sideLength;
	int noChanges;
}node;


void commit_move(node **currentNode, game *gptr,int **old, int *flags, int isFirst);

int undo_aux(game *gptr, node **current, int *flags, int print_bool);
int redo_aux(game *gptr, node **current, int *flags);

void print_changes(int **changes, int changes_num, int undo);

void append(node **current, int **changes,int *flags,int isFirst,int changesLen);

void terminate_all(node *current);

#endif /* UNDOREDO_H_ */
