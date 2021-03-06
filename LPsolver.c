/*
 * LPsolver.c
 *
 *  Created on: 14 Sep 2019
 *      Author: itait
 */

# include "LPsolver.h"

/* Frees a matrix of doubles. Used only to free prob_matrix in guess*/
void free_2d_double_array(double **map, int length) {
	int i;

	for (i = 0; i < length; i++) {
		free(map[i]);
	}
	free(map);
}

/* Expanding a 1d array to 2d array */
void reshape_solution(double *old_solution, double **new_solution, int num_rows,
		int row_length) {
	int i, j;

	for (i = 0; i < num_rows; i++) {
		for (j = 0; j < row_length; j++) {
			new_solution[i][j] = old_solution[i * row_length + j];
		}
	}
}

/* Remaps the valid values for a cell with probability over threshold
 * and normalizes the probabilities so that they sum to 1.
 * If all valid cells have Probability zero, alerts fill_cell */
int condanse_valids(game *gptr, int row, int col, double *original_prob,
		int **dest_index, double **dest_prob, double threshold,
		int *zero_sum_flag) {

	int i, vi = 0, num_valids = 0;
	double sum_prob = 0.0;

	for (i = 0; i < gptr->sideLength; i++) {
		if (original_prob[i] < threshold
				|| !check_valid_value(gptr, row, col, i + 1)) {
			original_prob[i] = -1.0;
		} else {
			num_valids++;
		}
	}

	*dest_index = calloc(num_valids, sizeof(int));
	*dest_prob = calloc(num_valids, sizeof(double));
	for (i = 0; i < gptr->sideLength; i++) {
		if (original_prob[i] >= 0.0) {
			sum_prob += original_prob[i];
			(*dest_index)[vi] = i;
			(*dest_prob)[vi] = original_prob[i];
			vi++;
		}
	}

	if (sum_prob == 0.0) {
		*zero_sum_flag = 1;
	} else {
		*zero_sum_flag = 0;
	}

	for (vi = 0; vi < num_valids; vi++) {
		/* normalize valid probs sh=o that they'll sum up to 1.0 approx. */
		(*dest_prob)[vi] = (*dest_prob)[vi] / sum_prob;
	}

	return num_valids;
}

/* Pick a random value from options, according to probability.
 * Builds an interval segmentation according to each value's
 * probability. */
int pick_random_value(int *value, double *prob, int length) {
	int i, store_index;
	double rand_num, bound = 0.0;
	double **intervals;

	intervals = malloc(length * sizeof(double *));
	for (i = 0; i < length; i++) {
		intervals[i] = calloc(2, sizeof(double));
		intervals[i][0] = bound;
		bound += prob[i];
		intervals[i][1] = bound;
	}
	intervals[i - 1][1] += 2.0;
	/* due to floating point error, it is possible that the normalized sum of probabilities for valid
	 * values will be greater/lower than 1.0 by some epsilon, to make sure rand_num will be below the
	 * upper bound of the last index, it is increased by 2.0. */

	rand_num = (double) rand() / (double) RAND_MAX;
	for (i = 0; i < length; i++) {
		if (rand_num >= intervals[i][0] && rand_num < intervals[i][1]) {
			store_index = value[i]; /* store index is used to return the correct index
			 * safely after deallocating valid_index */

			free_2d_double_array(intervals, length);
			return store_index;
		}
	}
	free_2d_double_array(intervals, length);
	return -1;
}

/* Fill a cell with a random value */
int fill_cell(game *gptr, double *value_prob, int row, int col,
		double threshold) {
	int *valid_value, zero_prob = 0;
	double *valid_prob;
	int num_valids = condanse_valids(gptr, row, col, value_prob, &valid_value,
			&valid_prob, threshold, &zero_prob);

	if (num_valids == 0) {
		free(valid_value);
		free(valid_prob);
		return 0;
	} else if (zero_prob == 1) {
		gptr->user[row][col] = valid_value[rand() % num_valids] + 1;
		free(valid_value);
		free(valid_prob);
		return 1;
	} else {
		gptr->user[row][col] = pick_random_value(valid_value, valid_prob,
				num_valids) + 1;
		free(valid_value);
		free(valid_prob);
		return 1;
	}
}

/******************************************************************************/
/******************************************************************************/

/* Implements guess_hint operation. */
int guess_hint_aux(game *gptr, int row, int col, GRBenv *env) {
	int i, empty_cells, objective_value, hint_cell;
	int **cell_map = NULL;
	double *solution_probs = NULL;

	empty_cells = enumerate_empty_cells(gptr, &cell_map);
	objective_value = gurobi_general(gptr, cell_map, &solution_probs,
			empty_cells, GRB_CONTINUOUS, env);

	if (objective_value < 0) {
		printf("Could not find solution.\n");

		free_2d_array(cell_map, empty_cells);
		free(solution_probs);
		return -1;
	}

	hint_cell = get_coords_index(cell_map, empty_cells, row, col);
	if (hint_cell == -1) {
		printf("Selected cell not found.\n");
		free_2d_array(cell_map, empty_cells);
		free(solution_probs);
		return -1;
	}

	printf("Possible values for row: %d, col: %d:  ", row + 1, col + 1); /* flip coordinates when printing due to X,Y scheme */
	for (i = 0; i < gptr->sideLength; i++) {
		if (solution_probs[hint_cell * gptr->sideLength + i] > 0.0) {
			printf("%d:%.2f  ", i + 1,
					solution_probs[hint_cell * gptr->sideLength + i]);
		}
	}
	printf("\n");

	free_2d_array(cell_map, empty_cells);
	free(solution_probs);
	return 0;
}

/* Implements guess operation. */
int guess_aux(game *gptr, double threshold, GRBenv *env) {
	int i, empty_cells, objective_value, filled_cells = 0;
	int **cell_map = NULL;
	double *solution_probs = NULL;
	double **prob_matrix = NULL; /* reshape solution_probs to a (sl x empty_cells) matrix */

	empty_cells = enumerate_empty_cells(gptr, &cell_map);
	objective_value = gurobi_general(gptr, cell_map, &solution_probs,
			empty_cells, GRB_CONTINUOUS, env);

	if (objective_value < 0) {
		printf("Could not find solution.\n");

		free_2d_array(cell_map, empty_cells);
		free(solution_probs);
		return -1;
	}

	prob_matrix = malloc(empty_cells * sizeof(double *));
	for (i = 0; i < empty_cells; i++) {
		prob_matrix[i] = calloc(gptr->sideLength, sizeof(double));
	}
	reshape_solution(solution_probs, prob_matrix, empty_cells,
			gptr->sideLength);
	free(solution_probs);

	/* PRINT CELL PROBABILITIES
	 for (i=0; i<empty_cells; i++) {
	 printf("[%d, %d]={ ", cell_map[i][0], cell_map[i][1]);
	 for (k=0; k<gptr->sideLength; k++) {
	 if (prob_matrix[i][k] > 0.0) {
	 printf("v%d=%.2f ", k+1, prob_matrix[i][k]);
	 }
	 }
	 printf("}\n");
	 } */

	for (i = 0; i < empty_cells; i++) {
		filled_cells += fill_cell(gptr, prob_matrix[i], cell_map[i][0],
				cell_map[i][1], threshold);
	}

	free_2d_array(cell_map, empty_cells);
	free_2d_double_array(prob_matrix, empty_cells);
	return filled_cells;
}

