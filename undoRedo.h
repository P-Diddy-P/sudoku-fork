#ifndef UNDOREDO_H_
#define UNDOREDO_H_

#include "gameStruct.h"


/* node struct changed, now has prev, next, flags and node (like game) */
typedef struct node {

	struct node *next;
	struct node *prev;
	int *flagArray;
	int **nodeBoard;
	int sideLength;
}node;

/* Moves in the game are stored in a doubly-linked list, where each node contains the flag values in the current move,
 * and the changes leading up to it in the board. If wholeBoard == 1, that means changes stores the entire board. This
 * is particularly useful for the first move, or for a lot of changes in the board. Otherwise, if wholeBoard == 0, changed
 * values are saved in an array of 4-tuples (row, col, old_value, new_value). The decision whether to store the whole board
 * or specific changes should be decided according to the number of changed cells.
 */

void commit_move(node **currentNode, game *gptr, int *flags,int isFirst);
/* finds differences between the current boards and oldBoard, storing said differences in a new linked list node.
 * In order to conserve space, differences will be stored in 4-tuples or the whole board according to space
 * considerations. The function returns the appended linked list node (already linked after the current node),
 * received from the append function. */

void undoRedo(game *gptr, node **current, int *flags, int undo);
/* Sets the board (TODO add flags setting (?)) to the data stored in the previous\next node in the linked list.
 * Also changes currentMove to point to the previous\next node in the linked list accordingly (so that it will point
 * to the actual current move).
 * TESTING NOTE: undoing then redoing a move should be equivalent to doing nothing. i.e. undoRedo should be completely
 * reversible.
 */

/* linked list API, remove terminate and free node */
void append(node **current, int **nodeBoard,int *flags,int isFirst,int sideLength);
void terminate(node *current);
void terminate_back(node *current);
void terminate_all(node *current);
void free_node(node *delNode);


/*--------------------TODO remove in the end-----------------------*/
void print_current_node(node *Node, node *currentMove,game *gptr);
void print_history(node *currentMove,game *gptr);

#endif /* UNDOREDO_H_ */
