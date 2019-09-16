/*
 * ILPsolver.c
 *
 *  Created on: 3 Sep 2019
 *      Author: itait
 */

#include "ILPsolver.h"


/* Receives indices [i,j] and a board, and returns an array with all valid
 * values for [i,j] */

/* returns an array with all valid values for cell [i,j] */
void enumerate_valid_values_for_cell(int* valid_vals_array, int i, int j,
		game *gptr, int *num_vals) {
	int value;
	int num = 0;

	/* if value valid for cell, add to array */
	for (value = 1; value <= gptr->sideLength; value++) {
		if (check_valid_value(gptr, i, j, value)) {
			valid_vals_array[num] = value;
			num++;
		}
	}

	*num_vals = num;
}

/* swapping pointer values for int arrays, in an
 * array of pointers to int arrays */
void swap_cell_values(int *a, int *b) {
	int ti, tj;
	ti = b[0];
	tj = b[1];

	b[0] = a[0];
	b[1] = a[1];

	a[0] = ti;
	a[1] = tj;
}

/* swapping 2 ints in an int array */
void swap_numbers(int *a, int *b) {
	int temp;
	temp = *b;
	*b = *a;
	*a = temp;
}

/* Randomizing the array of empty cells for every iteration
 * Implemented using the Fisher-Yates algorithm (objects in hat),
 * which guarantees an uniform distribution of permutations in O(n) */
void randomize_cell_array(int** array, int length) {
	int i, j;

	/* pick an element from the end of elements not picked,
	 * and replace it with element from array */
	for (i = length - 1; i > 0; i--) {
		j = rand() % (i + 1);
		swap_cell_values(array[i], array[j]);
	}
}

/* Randomizing an array of of ints
 * Fisher-Yates */
void randomize_array(int* array, int length) {
	int i, j;

	/* for */
	for (i = length - 1; i > 0; i--) {
		j = rand() % (i + 1);
		swap_numbers(&(array[i]), &(array[j]));
	}
}

/* Solve a board and fill values, if solution exists
gets pointer to cell map (maybe generate cell map inside function?) and to board
this function is called from gen_board, which takes care of filling board with X random values
in gen_board, a local copy of gptr->user is created  */
int gurobi_ilp(int **board, game *gptr, GRBenv *env) {
	int **empty_cells = NULL;
	double *objective_solution = NULL;
	int empty_num, has_sol, empty_cell_index, value;

	empty_num = enumerate_empty_cells(gptr, &empty_cells);
	has_sol = gurobi_general(gptr, empty_cells, &objective_solution, empty_num,
								GRB_BINARY, env);

	if (has_sol < 0) {
		printf("No solution found\n");

		free(objective_solution);
		free_2d_array(empty_cells, gptr->sideLength);
		return 0;
	}

	if (has_sol != empty_num){
		printf("Objective value incorrect, ...\n");
		/* free allocated objective_solution and empty_cells*/
		free(objective_solution);
		free_2d_array(empty_cells,gptr->sideLength);
		return 0;
	}

	/* else, produce solution from objective_solution array. Explanation:
		The linear program variables X_ijk are IMPLICTLY encoded in
		the indices of the double array "objective_solution".
		The array conatains the value of X_ijk, where every index in objective_solution
		corresponds to the fourmula "the index of the cell in empty_cells, times sideLength,
		plus the value-1". The "origin" of this index system is in the objective function setting
		If it equals 1.0, it means that the the cell [i,j] encoded in empty_cells,
		should have the value+1 (index to value)
	*/
	for (empty_cell_index=0;empty_cell_index<empty_num;empty_cell_index++){
		for (value=0;value<gptr->sideLength;value++){
			if (objective_solution[empty_cell_index*gptr->sideLength+value]==1.0){
				board[empty_cells[empty_cell_index][0]][empty_cells[empty_cell_index][1]] = value+1;
			}
		}
	}

	free(objective_solution);
	free_2d_array(empty_cells,gptr->sideLength);

    return 1;
}


/* For Generate: returns solved board, NULL else
 * Logic of generate: assumes at least X cells are empty,
 * randomly chooses X from them, and tries to fill them with valid values.
 * If succeeds, proceed to get_sol(board), else try to pick (other) X empty cells
 * and fill them
 * TODO - review logic: maybe the meaning of "pick X cells and fill..."
 * was to first pick the cells, and then try to find values for them */
int** gen_board(game *gptr, int cells_to_fill,GRBenv *env) {
	int **local;
	int **empty_cells;
	int *valid_vals_cell;
	int cell_fill_success, sol_success = 0, empty_num, attempts_counter = 0,
			rand_ind, itr, valid_vals_cell_num, i, j;

	local = init_2d_array(gptr->sideLength);
	empty_num = enumerate_empty_cells(gptr, &empty_cells);
	valid_vals_cell = calloc(gptr->sideLength, sizeof(int));
	memory_alloc_error();

	while (attempts_counter++ < 1000) {
		randomize_cell_array(empty_cells, empty_num);
		cell_fill_success = 1;

		for (itr = 0; itr < cells_to_fill; itr++) {
			i = empty_cells[itr][0];
			j = empty_cells[itr][1];

			enumerate_valid_values_for_cell(valid_vals_cell, i, j, gptr,
					&valid_vals_cell_num);

			if (valid_vals_cell_num == 0) {
				cell_fill_success = 0;
				break;
			} else {
				rand_ind = rand() % valid_vals_cell_num;
				local[i][j] = valid_vals_cell[rand_ind];
			}
		}

		if (!cell_fill_success) {
			continue;
		}

		sol_success = gurobi_ilp(local, gptr, env);
		if (sol_success) {
			break;
		}
	}

	free_2d_array(empty_cells, empty_num);
	free(valid_vals_cell);
	if (!sol_success) {
		free_2d_array(local, gptr->sideLength);
		return NULL;
	} else {
		return local;
	}
}

/* For Validate: returns 1 if board is solvable, 0 else */
int board_has_sol(game *gptr, GRBenv *env) {
	int **local;
	int success;
	local = init_2d_array(gptr->sideLength);
	copy_2d_array(local, gptr->user, gptr->sideLength);

	success = gurobi_ilp(local, gptr,env);
	free_2d_array(local, gptr->sideLength);
	return success;
}

