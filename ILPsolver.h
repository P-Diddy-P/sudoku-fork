/*
 * ILPsolver.h
 *
 *  Created on: 3 Sep 2019
 *      Author: itait
 *
 * Module purpose:
 * - functions using ILP for generating new random boards and validating
 * exitsting boards. Serves the operations: generate, hint and validate
 */

#ifndef ILPSOLVER_H_
#define ILPSOLVER_H_
# include "gurobi_interface.h"


int** gen_board(game *gptr, int cells_to_fill,GRBenv *env);

int board_has_sol(game *gptr,GRBenv *env);

int gurobi_ilp(int **local,game *gptr,GRBenv *env);

void randomize_cell_array(int** array, int length);


#endif /* ILPSOLVER_H_ */
