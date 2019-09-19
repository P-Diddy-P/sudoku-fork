/*
 * gurobi_interface.h
 *
 *  Created on: 14 Sep 2019
 *      Author: itait
 *
 *
 *
 * Module Purpose:
 * - Contains all functions interacting with Gurobi in any way,
 * except for setting up the environment in main.c
 */

#ifndef GUROBI_INTERFACE_H_
#define GUROBI_INTERFACE_H_
# include "gurobi_c.h"
# include "gameStruct.h"

int gurobi_general(game *gptr, int **cell_map, double **objective_solution,
		int empty_cells, char GRB_type, GRBenv *env);

int get_coords_index(int **map, int mapLength, int row, int col);


int enumerate_empty_cells(game *gptr, int ***empty_cells);

#endif /* GUROBI_INTERFACE_H_ */
