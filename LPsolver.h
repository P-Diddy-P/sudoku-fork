/*
 * LPsolver.h
 *
 *  Created on: 14 Sep 2019
 *      Author: itait
 *
 *
 * Module purpose:
 * - functions using LP for speculative solutions of exisiting boards
 * serves the operations guess and guess_hint
 */


# include "gurobi_interface.h"

int guess_aux(game *gptr, double threshold,GRBenv *env);

int guess_hint_aux(game *gptr, int row, int col,GRBenv *env);
