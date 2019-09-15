/*
 * LPsolver.h
 *
 *  Created on: 14 Sep 2019
 *      Author: itait
 */


# include "gurobi_interface.h"

int guess_aux(game *gptr, float threshold,GRBenv *env);

int guess_hint_aux(game *gptr, int row, int col,GRBenv *env);
