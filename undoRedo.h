#ifndef UNDOREDO_H_
#define UNDOREDO_H_

#include "gameStruct.h"

typedef struct node {

	struct node *next;
	struct node *prev;
	int *flagArray;
	int wholeBoard;
	int changeLength;
	int **changes;
}node;

/* Moves in the game are stored in a doubly-linked list, where each node contains the flag values in the current move,
 * and the changes leading up to it in the board. If wholeBoard == 1, that means changes stores the entire board. This
 * is particularly useful for the first move, or for a lot of changes in the board. Otherwise, if wholeBoard == 0, changed
 * values are saved in an array of 4-tuples (row, col, old_value, new_value). The decision whether to store the whole board
 * or specific changes should be decided according to the number of changed cells.
 */

node *commit_move(node *currentNode, game *gptr, int **oldBoard, int *flags);
/* finds differences between the current boards and oldBoard, storing said differences in a new linked list node.
 * In order to conserve space, differences will be stored in 4-tuples or the whole board according to space
 * considerations. The function returns the appended linked list node (already linked after the current node),
 * received from the append function. */

void undoRedo(game *gptr, node *currentMove, int undo);
/* Sets the board (TODO add flags setting) to the data stored in the previous\next node in the linked list.
 * Also changes currentMove to point to the previous\next node in the linked list accordingly (so that it will point
 * to the actual current move).
 * TESTING NOTE: undoing then redoing a move should be equivalent to doing nothing. i.e. undoRedo should be completely
 * reversible.
 */

/* linked list API, remove terminate and free node */
node *append(node *current, int *flags, int wbFlag, int **data, int cl);
void terminate(node *currentNode);
void free_node(node *delNode);

#endif /* UNDOREDO_H_ */
