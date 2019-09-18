/*
 * gurobi_interface.c
 *
 *  Created on: 14 Sep 2019
 *      Author: itait
 */

# include "gurobi_interface.h"
# include "gameStruct.h"

int get_coords_index(int **map, int mapLength, int row, int col) {
	int i;

	for (i = 0; i < mapLength; i++) {
		if (map[i][0] == row && map[i][1] == col) {
			return i;
		}
	}
	return -1;
}

int enumerate_empty_cells(game *gptr, int ***empty_cells) {
	int i, j, k, empty_num = 0;

	for (i = 0; i < gptr->sideLength; i++) {
		for (j = 0; j < gptr->sideLength; j++) {
			if (gptr->user[i][j] == 0) {
				empty_num++;
			}
		}
	}

	(*empty_cells) = malloc(empty_num * sizeof(int *));
	memory_alloc_error();

	k = 0;
	for (i = 0; i < gptr->sideLength; i++) {
		for (j = 0; j < gptr->sideLength; j++) {
			if (gptr->user[i][j] == 0) {
				(*empty_cells)[k] = malloc(2 * sizeof(int));
				memory_alloc_error();

				(*empty_cells)[k][0] = i;
				(*empty_cells)[k][1] = j;
				k++;
			}
		}
	}

	if (k == empty_num) {
		return empty_num;
	} else {
		printf("error, could not fill entire map.\n");
		return -1;
	}
}

int add_single_value_constraints(GRBenv *env, GRBmodel *model, /*int **cell_map, */
		game *gptr, double *constraint_coefficient, int empty_cells) {
	int k, i, error, constraint_vars;
	int *constraint_index = calloc(gptr->sideLength, sizeof(int));

	for (k = 0; k < empty_cells; k++) { /* empty cell index */
		constraint_vars = 0;

		for (i = 0; i < gptr->sideLength; i++) { /* cell value */
			constraint_index[constraint_vars++] = k * gptr->sideLength + i;
		}

		error = GRBaddconstr(model, constraint_vars, constraint_index,
				constraint_coefficient, GRB_EQUAL, 1.0, NULL);
		if (error) {
			printf("ERROR %d GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
			free(constraint_index);
			return -1;
		}
	}

	error = GRBupdatemodel(model);
	if (error) {
		printf("ERROR %d GRBupdatemodel(): %s\n", error, GRBgeterrormsg(env));
		free(constraint_index);
		return -1;
	}

	free(constraint_index);
	return 0;
}

int add_row_constraints(GRBenv *env, GRBmodel *model, int **cell_map,
		game *gptr, double *constraint_coefficient, int empty_cells) {
	int i, j, k, constraint_vars, error, coords_index;
	int *constraint_index = calloc(gptr->sideLength, sizeof(int));

	for (i = 0; i < gptr->sideLength; i++) {
		for (k = 0; k < gptr->sideLength; k++) {
			constraint_vars = 0;

			/* if the value is not valid for the column, that means
			 there's already a cell with that value in the same column,
			 making this function a good replacement for vlaue_in_row.
			 Thanks Itai */
			if (!row_valid_value(gptr, i, k + 1)) {
				continue;
			}

			/* check all empty cells in row which are able to contain value k (doesn't clash with
			 column/block constraints), and add their index to the constraint_index array */
			for (j = 0; j < gptr->sideLength; j++) {
				if (gptr->user[i][j] || !check_valid_value(gptr, i, j, k + 1)) {
					continue;
				}

				coords_index = get_coords_index(cell_map, empty_cells, i, j);
				if (coords_index == -1) {
					free(constraint_index);
					return -2;
				}
				constraint_index[constraint_vars++] = coords_index
						* gptr->sideLength + k;
			}

			error = GRBaddconstr(model, constraint_vars, constraint_index,
					constraint_coefficient, GRB_EQUAL, 1.0, NULL);
			if (error) {
				printf("ERROR %d GRBaddconstr(): %s\n", error,
						GRBgeterrormsg(env));
				free(constraint_index);
				return -1;
			}
		}
	}

	error = GRBupdatemodel(model);
	if (error) {
		printf("ERROR %d GRBupdatemodel(): %s\n", error, GRBgeterrormsg(env));
		free(constraint_index);
		return -1;
	}

	free(constraint_index);
	return 0;
}

int add_column_constraints(GRBenv *env, GRBmodel *model, int **cell_map,
		game *gptr, double *constraint_coefficient, int empty_cells) {
	int i, j, k, constraint_vars, error, coords_index;
	int *constraint_index = calloc(gptr->sideLength, sizeof(int));

	for (j = 0; j < gptr->sideLength; j++) {
		for (k = 0; k < gptr->sideLength; k++) {
			constraint_vars = 0;

			if (!col_valid_value(gptr, j, k + 1)) {
				continue;
			}

			for (i = 0; i < gptr->sideLength; i++) {
				if (gptr->user[i][j] || !check_valid_value(gptr, i, j, k + 1)) {
					continue;
				}

				coords_index = get_coords_index(cell_map, empty_cells, i, j);
				if (coords_index == -1) {
					free(constraint_index);
					return -2;
				}
				constraint_index[constraint_vars++] = coords_index
						* gptr->sideLength + k;
			}

			error = GRBaddconstr(model, constraint_vars, constraint_index,
					constraint_coefficient, GRB_EQUAL, 1.0, NULL);
			if (error) {
				printf("ERROR %d GRBaddconstr(): %s\n", error,
						GRBgeterrormsg(env));
				free(constraint_index);
				return -1;
			}
		}
	}

	error = GRBupdatemodel(model);
	if (error) {
		printf("ERROR %d GRBupdatemodel(): %s\n", error, GRBgeterrormsg(env));
		free(constraint_index);
		return -1;
	}

	free(constraint_index);
	return 0;
}

int add_block_constraints(GRBenv *env, GRBmodel *model, int **cell_map,
		game *gptr, double *constraint_coefficient, int empty_cells) {
	int i, j, k, b, constraint_vars, error, coords_index;
	int *constraint_index = calloc(gptr->sideLength, sizeof(int));

	for (b = 0; b < gptr->sideLength; b++) {
		for (k = 0; k < gptr->sideLength; k++) {
			constraint_vars = 0;
			i = gptr->rows * (b / gptr->rows); /* i and j are iteration variables, they are defined here */
			j = (b % gptr->rows) * gptr->cols; /* to indicate the block starting index for block_valid_value */

			if (!block_valid_value(gptr, i, j, k + 1)) {
				continue;
			}

			for (/* defined above, see comment */;
					i < gptr->rows * (b / gptr->rows + 1); i++) {
				for (j = (b % gptr->rows) * gptr->cols;
						j < (b % gptr->rows + 1) * gptr->cols; j++) {
					if (gptr->user[i][j]
							|| !check_valid_value(gptr, i, j, k + 1)) {
						continue;
					}

					coords_index = get_coords_index(cell_map, empty_cells, i,
							j);
					if (coords_index == -1) {
						free(constraint_index);
						return -2;
					}
					constraint_index[constraint_vars++] = coords_index
							* gptr->sideLength + k;
				}
			}

			error = GRBaddconstr(model, constraint_vars, constraint_index,
					constraint_coefficient, GRB_EQUAL, 1.0, NULL);
			if (error) {
				printf("ERROR %d GRBaddconstr(): %s\n", error,
						GRBgeterrormsg(env));
				free(constraint_index);
				return -1;
			}
		}
	}

	error = GRBupdatemodel(model);
	if (error) {
		printf("ERROR %d GRBupdatemodel(): %s\n", error, GRBgeterrormsg(env));
		free(constraint_index);
		return -1;
	}

	free(constraint_index);
	return 0;
}

int add_constraints(GRBenv *env, GRBmodel *model, int **cell_map, game *gptr,
		double *constraint_coefficient, int empty_cells) {
	int error;

	error = add_single_value_constraints(env, model, /* cell_map, */ gptr,
			constraint_coefficient, empty_cells);
	if (error) {
		printf("Error while adding single cell value constraints.\n");
		return -1;
	}

	error = add_row_constraints(env, model, cell_map, gptr,
			constraint_coefficient, empty_cells);
	if (error) {
		printf("Error while adding single cell value constraints%s.\n",
				(error == -2) ? ". Found unmapped empty cell" : "");
		return -1;
	}

	error = add_column_constraints(env, model, cell_map, gptr,
			constraint_coefficient, empty_cells);
	if (error) {
		printf("Error while adding single cell value constraints%s.\n",
				(error == -2) ? ". Found unmapped empty cell" : "");
		return -1;
	}

	error = add_block_constraints(env, model, cell_map, gptr,
			constraint_coefficient, empty_cells);
	if (error) {
		printf("Error while adding single cell value constraints%s.\n",
				(error == -2) ? ". Found unmapped empty cell" : "");
		return -1;
	}

	return 0;
}

double get_objective_coefficient(game *gptr, int row, int col, int value) {
	int i, j, contender_cells = 0;
	int start_row = row - row % gptr->rows, start_col = col - col % gptr->cols;

	for (i = 0; i < gptr->sideLength; i++) {
		if (i != row && !gptr->user[i][col]
				&& check_valid_value(gptr, i, col, value)) {
			contender_cells++;
		}
	}

	for (j = 0; j < gptr->sideLength; j++) {
		if (j != col && !gptr->user[row][j]
				&& check_valid_value(gptr, row, j, value)) {
			contender_cells++;
		}
	}

	for (i = start_row; i < start_row + gptr->rows; i++) {
		if (i == row) {
			continue;
			/* skip over cells in the same row and column, as they were already tested */
		}

		for (j = start_col; j < start_col + gptr->cols; j++) {
			if (j != col && !gptr->user[row][j]
					&& check_valid_value(gptr, row, j, value)) {
				contender_cells++;
			}
		}
	}

	if (!contender_cells) {
		return (double) 1.0;
	} else {
		/* return 1.0; objective function v0.0 (naive) */
		/* return (double) (1.0 / contender_cells); objective function v1.0 */
		return (double) (1.0 / pow(2.0, contender_cells)); /* objective function v2.0 */
	}
}

/* Currently there are 3 options to setting objective coefficients:
 1. For each value in each empty cell count the number of other incident cells which can receive the same value, divide it by the
 number of such cells (or 2 to the power of the number of such cells).
 2. For each value in each empty cell order this cekk and all other incident cells according to their prob to have the said
 value. */
void set_objective_function(game *gptr, int **cell_map,
		double *objective_coefficient, int empty_cells) {
	int i, k;

	for (i = 0; i < empty_cells; i++) {
		for (k = 0; k < gptr->sideLength; k++) {
			objective_coefficient[i * gptr->sideLength + k] =
					get_objective_coefficient(gptr, cell_map[i][0],
							cell_map[i][1], k);
		}
	}
}

int gurobi_general(game *gptr, int **cell_map, double **objective_solution,
		int empty_cells, char GRB_type, GRBenv *env) {
	GRBmodel *model = NULL;
	int optimstatus, error, i, k;

	double *objective_coefficient = calloc(empty_cells * gptr->sideLength,
			sizeof(double));
	double objective_value;
	double *variable_upper_bound = calloc(empty_cells * gptr->sideLength,
			sizeof(double));
	double *constraint_coefficient = calloc(gptr->sideLength, sizeof(double));
	char *variable_type = calloc(empty_cells * gptr->sideLength, sizeof(char));

	for (k = 0; k < empty_cells; k++) {
		for (i = 0; i < gptr->sideLength; i++) {
			if (check_valid_value(gptr, cell_map[k][0], cell_map[k][1],
					i + 1)) {
				objective_coefficient[k * gptr->sideLength + i] = 1.0;
			}
			variable_upper_bound[k * gptr->sideLength + i] = 1.0;
			variable_type[k * gptr->sideLength + i] = GRB_type;
		}
	}

	if (GRB_type == GRB_CONTINUOUS) {
		set_objective_function(gptr, cell_map, objective_coefficient,
				empty_cells);
	}

	error = GRBnewmodel(env, &model, "sudoku guess",
					empty_cells * gptr->sideLength, objective_coefficient, NULL,
					variable_upper_bound, variable_type, NULL);
	if (error) {
		printf("ERROR %d GRBnewmodel: %s\n", error, GRBgeterrormsg(env));
		return -1;
	}

	error = GRBsetintattr(model, GRB_INT_ATTR_MODELSENSE, GRB_MAXIMIZE);
	if (error) {
		printf("ERROR %d GRBsetintattr(): %s\n", error, GRBgeterrormsg(env));
		return -1;
	}

	error = GRBupdatemodel(model);
	if (error) {
		printf("ERROR %d GRBupdatemodel(): %s\n", error, GRBgeterrormsg(env));
		return -1;
	}

	for (i = 0; i < gptr->sideLength; i++) {
		constraint_coefficient[i] = 1.0;
		/* all constraint coefficients will be either 1.0 or 0.0,
		 * and if set to 0.0 will not be in the constraint index array
		 * at all. */
	}

	error = add_constraints(env, model, cell_map, gptr, constraint_coefficient,
			empty_cells);
	if (error) {
		printf("Error while adding single cell value constraints.\n");
		return -1;
	}

	error = GRBoptimize(model);
	if (error) {
		printf("ERROR %d GRBoptimize(): %s\n", error, GRBgeterrormsg(env));
		return -1;
	}

	error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
	if (error) {
		printf("ERROR %d GRBgetintattr(): %s\n", error, GRBgeterrormsg(env));
		return -1;
	} else if (optimstatus == GRB_INFEASIBLE) {
		printf("No feasible solution found.\n");
		return -2;
	} else if (optimstatus == GRB_UNBOUNDED) {
		printf("Linear model received is unbounded.\n");
		return -2;
	}

	error = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &objective_value);
	if (error) {
		printf("THIS ERROR %d GRBgetdblattr(): %s\n", error,
				GRBgeterrormsg(env));
		return -1;
	}

	*objective_solution = calloc(empty_cells * gptr->sideLength,
									sizeof(double));

	error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0,
					empty_cells * gptr->sideLength, *objective_solution);
	if (error) {
		printf("ERROR %d GRBgetdblattrarray(): %s\n", error,
				GRBgeterrormsg(env));
		return -1;
	}

	free(objective_coefficient);
	free(constraint_coefficient);
	free(variable_type);
	free(variable_upper_bound);
	GRBfreemodel(model);

	return (int) objective_value;
}
