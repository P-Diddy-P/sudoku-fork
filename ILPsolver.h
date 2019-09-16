/*
 * ILPsolver.h
 *
 *  Created on: 3 Sep 2019
 *      Author: itait
 */

#ifndef ILPSOLVER_H_
#define ILPSOLVER_H_
# include "gurobi_interface.h"



/* Generative function for generate*/
int** gen_board(game *gptr, int cells_to_fill,GRBenv *env);


/* Boolean function for validate*/
int board_has_sol(game *gptr,GRBenv *env);

int gurobi_ilp(int **local,game *gptr,GRBenv *env);

void randomize_cell_array(int** array, int length);


#endif /* ILPSOLVER_H_ */
