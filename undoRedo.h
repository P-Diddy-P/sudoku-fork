#ifndef UNDOREDO_H_
#define UNDOREDO_H_

#include "gameStruct.h"


/* node struct changed, now has prev, next, flags and node (like game) */
typedef struct node {

	struct node *next;
	struct node *prev;
	int *flagArray;
	int **changes;
	int changesLen;
	int sideLength;
	int noChanges; /* Boolean variable indicating whether the node has no */
}node;

/* Moves in the game are stored in a doubly-linked list, where each node contains the changes conducted
 * to get the current board.
 */

/* finds differences between the current board and oldBoard, storing said differences in a new linked list node.
 * In order to conserve space, differences will be stored in 4-tuples */
void commit_move(node **currentNode, game *gptr,int **old, int *flags, int isFirst);


/* undo and redo functions. logic differs between them:
 * - undo: first copy old values, print to user the changes (if functions
 * was not called from reset), and move back pointer
 * - redo: first move current pointer forward, then copy values from new. */
void undo_aux(game *gptr, node **current, int *flags, int print_bool);
void redo_aux(game *gptr, node **current, int *flags);

/* print changes from currentMove node */
void print_changes(int **changes, int changes_num, int undo);

/* linked list API, remove terminate and free node */
void append(node **current, int **changes,int *flags,int isFirst,int changesLen);
void terminate(node *current);
void terminate_back(node *current);
void terminate_all(node *current);
void free_node(node *delNode);


/*--------------------TODO remove in the end-----------------------*/
void print_current_node(node *Node, node *currentMove);
void print_history(node *currentMove);

#endif /* UNDOREDO_H_ */
