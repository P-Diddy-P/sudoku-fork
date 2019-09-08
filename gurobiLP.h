#ifndef GUROBILP_H_
#define GUROBILP_H_

#include "gameStruct.h"
#include "gurobi_c.h"

#define					SAMPLE_SIZE		10000

/* Given [row, column] coordinates, solves the game with gurobi linear programming and prints the possible
 * values of the coordinates with assumed probability. */
int guess_hint(game *gptr, int row, int col);

/* Given some threshold, solves the game with gurobi linear programming and fills cells with values which
 * 1. do not cause an error in the current game state, 2. have a higher assumed probability than threshold */
int guess(game *gptr, float threshold);

#endif /* GUROBILP_H_ */
