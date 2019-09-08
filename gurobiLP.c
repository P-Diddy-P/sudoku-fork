#include "gurobiLP.h"

/********************************************************/
/* TODO UTILITY FUNCTIONS - NOT RELATED TO GUROBI  TODO */
/********************************************************/

/* returns the index of a cell with given coordinates [row, col] in map argument.
 * If the cell is not in map returns -1. */
int get_coords_index(int **map, int mapLength, int row, int col) {
    int i;

    for (i=0; i<mapLength; i++) {
        if (map[i][0] == row && map[i][1] == col) {
            return i;
        }
    }

    return -1;
}

/* Creates an indexing of empty cells in a game. Each member of the mapping
 * is a two-array containing row and column */
int create_map(game *gptr, int ***dest) {
    int i, j, k, empty_cells = 0;

    for (i=0; i<gptr->sideLength; i++) {
        for (j=0; j<gptr->sideLength; j++) {
            if (gptr->user[i][j] == 0) {
                empty_cells++;
            }
        }
    }

    (*dest) = malloc(empty_cells * sizeof(int *));
    k = 0;
    for (i=0; i<gptr->sideLength; i++) {
        for (j=0; j<gptr->sideLength; j++) {
            if (gptr->user[i][j] == 0) {
                (*dest)[k] = malloc(2 * sizeof(int));
                (*dest)[k][0] = i;
                (*dest)[k][1] = j;
                k++;
            }
        }
    }

    if (k == empty_cells) {
        return empty_cells;
    } else {
        printf("error, could not fill entire map.\n");
        return -1;
    }
}

/* Free a 2D array. According to: https://stackoverflow.com/questions/39878298/ it is
 * not possible to free 2D arrays of a generic type safely without some macro hacks, which
 * I do not care to try. */
void free_2d_double_array(double **array, int length) {
    int i;

    for (i=0; i<length; i++) {
        free(array[i]);
    }
    free(array);
}

/********************************************************/
/* TODO             GUROBI   FUNCTIONS             TODO */
/********************************************************/
/* NOTE: gurobi functions will cause compilation errors in eclipse.
 * TODO consider adding empty declarations in header to subvert the problem. */

int add_single_value_constraints(GRBenv *env, GRBmodel *model, int **cell_map, game *gptr, double *constraint_coefficient, int empty_cells) {
    int k, i, error, constraint_vars;
    int *constraint_index = calloc(gptr->sideLength, sizeof(int));

    for (k = 0; k < empty_cells; k++) { /* empty cell index */
        constraint_vars = 0;

        for (i = 0; i < gptr->sideLength; i++) { /* cell value */
            if (check_valid_value(gptr, cell_map[k][0], cell_map[k][1], i + 1)) {
                constraint_index[constraint_vars++] = k * gptr->sideLength + i;
            }
        }

        error = GRBaddconstr(model, constraint_vars, constraint_index, constraint_coefficient, GRB_EQUAL, 1.0, NULL);
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

int add_row_constraints(GRBenv *env, GRBmodel *model, int **cell_map, game *gptr, double *constraint_coefficient, int empty_cells) {
    int i, j, k, constraint_vars, error, coords_index;
    int *constraint_index = calloc(gptr->sideLength, sizeof(int));

    for (i=0; i<gptr->sideLength; i++) {
        for (k=0; k<gptr->sideLength; k++) {
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
            for (j=0; j<gptr->sideLength; j++) {
                if (gptr->user[i][j] || !check_valid_value(gptr, i, j, k + 1)) {
                    continue;
                }

                coords_index = get_coords_index(cell_map, empty_cells, i, j);
                if (coords_index == -1) {
                    free(constraint_index);
                    return -2;
                }
                constraint_index[constraint_vars++] = coords_index * gptr->sideLength + k;
            }

            error = GRBaddconstr(model, constraint_vars, constraint_index, constraint_coefficient, GRB_EQUAL, 1.0, NULL);
            if (error) {
                printf("ERROR %d GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
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

int add_column_constraints(GRBenv *env, GRBmodel *model, int **cell_map, game *gptr, double *constraint_coefficient, int empty_cells) {
    int i, j, k, constraint_vars, error, coords_index;
    int *constraint_index = calloc(gptr->sideLength, sizeof(int));

    for (j=0; j<gptr->sideLength; j++) {
        for (k=0; k<gptr->sideLength; k++) {
            constraint_vars = 0;

            if (!col_valid_value(gptr, j, k + 1)) {
                continue;
            }

            for (i=0; i<gptr->sideLength; i++) {
                if (gptr->user[i][j] || !check_valid_value(gptr, i, j, k + 1)) {
                    continue;
                }

                coords_index = get_coords_index(cell_map, empty_cells, i, j);
                if (coords_index == -1) {
                    free(constraint_index);
                    return -2;
                }
                constraint_index[constraint_vars++] = coords_index * gptr->sideLength + k;
            }

            error = GRBaddconstr(model, constraint_vars, constraint_index, constraint_coefficient, GRB_EQUAL, 1.0, NULL);
            if (error) {
                printf("ERROR %d GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
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

int add_block_constraints(GRBenv *env, GRBmodel *model, int **cell_map, game *gptr, double *constraint_coefficient, int empty_cells) {
    int i, j, k, b, constraint_vars, error, coords_index;
    int *constraint_index = calloc(gptr->sideLength, sizeof(int));

    for (b=0; b<gptr->sideLength; b++) {
        for (k=0; k<gptr->sideLength; k++) {
            constraint_vars = 0;
            i = gptr->rows * (b / gptr->rows); /* i and j are iteration variables, they are defined here */
            j = (b % gptr->rows) * gptr->cols;  /* to indicate the block starting index for block_valid_value */

            if (!block_valid_value(gptr, i, j, k + 1)) {
                continue;
            }

            for (/* defined above, see comment */; i<gptr->rows * (b / gptr->rows + 1); i++) {
                for (j = (b % gptr->rows) * gptr->cols; j < (b % gptr->rows + 1) * gptr->cols; j++) {
                    if (gptr->user[i][j] || !check_valid_value(gptr, i, j, k + 1)) {
                        continue;
                    }

                    coords_index = get_coords_index(cell_map, empty_cells, i, j);
                    if (coords_index == -1) {
                        free(constraint_index);
                        return -2;
                    }
                    constraint_index[constraint_vars++] = coords_index * gptr->sideLength + k;
                }
            }

            error = GRBaddconstr(model, constraint_vars, constraint_index, constraint_coefficient, GRB_EQUAL, 1.0, NULL);
            if (error) {
                printf("ERROR %d GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
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

int add_constraints(GRBenv *env, GRBmodel *model, int **cell_map, game *gptr, double *constraint_coefficient, int empty_cells) {
    int error;

    error = add_single_value_constraints(env, model, cell_map, gptr, constraint_coefficient, empty_cells);
    if (error) {
        printf("Error while adding single cell value constraints.\n");
        return -1;
    }

    error = add_row_constraints(env, model, cell_map, gptr, constraint_coefficient, empty_cells);
    if (error) {
        printf("Error while adding single cell value constraints%s.\n", (error == -2) ? ". Found unmapped empty cell" : "");
        return -1;
    }

    error = add_column_constraints(env, model, cell_map, gptr, constraint_coefficient, empty_cells);
    if (error) {
        printf("Error while adding single cell value constraints%s.\n", (error == -2) ? ". Found unmapped empty cell" : "");
        return -1;
    }

    error = add_block_constraints(env, model, cell_map, gptr, constraint_coefficient, empty_cells);
    if (error) {
        printf("Error while adding single cell value constraints%s.\n", (error == -2) ? ". Found unmapped empty cell" : "");
        return -1;
    }

    return 0;
}

/* Solves a sudoku board using gurobi linear programming and returns the objective value of a simple sum
 * of all variables (which are all possible values of all empty cells). */
int gurobi_lp(game *gptr, int **cell_map, double ***solution_address, int empty_cells) {
    GRBenv *env = NULL;
    GRBmodel *model = NULL;
    int optimstatus, error, i, k;

    double *objective_coefficient;
    double objective_value;
    double *objective_solution;
    double *variable_upper_bound;
    double *constraint_coefficient = calloc(gptr->sideLength, sizeof(double));

    error = GRBloadenv(&env, "LPguess.log");
    if (error) {
        printf("ERROR %d GRBloadenv: %s\n", error, GRBgeterrormsg(env));
        return -1;
    }

    error = GRBsetintparam(env, GRB_INT_PAR_LOGTOCONSOLE, 0);
    if (error) {
        printf("ERROR %d GRBsetintattr(): %s\n", error, GRBgeterrormsg(env));
        return -1;
    }

    objective_coefficient = calloc(empty_cells * gptr->sideLength, sizeof(double));
    variable_upper_bound = calloc(empty_cells * gptr->sideLength, sizeof(double));

    for (k = 0; k < empty_cells; k++) {
        for (i = 0; i < gptr->sideLength; i++) {
            if (check_valid_value(gptr, cell_map[k][0], cell_map[k][1], i + 1)) {
                objective_coefficient[k * gptr->sideLength + i] = 1.0;
            }
            variable_upper_bound[k * gptr->sideLength + i] = 1.0;
        }
    }

    error = GRBnewmodel(env, &model, "sudoku guess", empty_cells * gptr->sideLength, objective_coefficient,
                            NULL, variable_upper_bound, NULL, NULL);
    if (error) {
        printf("ERROR %d GRBnewmodel: %s\n", error, GRBgeterrormsg(env));
        return -1;
    }

    error = GRBupdatemodel(model);
    if (error) {
        printf("ERROR %d GRBupdatemodel(): %s\n", error, GRBgeterrormsg(env));
        return -1;
    }

    for (i=0; i<gptr->sideLength; i++) {
        constraint_coefficient[i] = 1.0;
        /* all constraint coefficients will be either 1.0 or 0.0,
        and if set to 0.0 will not be in the constraint at all */
    }

    error = add_constraints(env, model, cell_map, gptr, constraint_coefficient, empty_cells);
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
        printf("THIS ERROR %d GRBgetdblattr(): %s\n", error, GRBgeterrormsg(env));
        return -1;
    }

    objective_solution = calloc(empty_cells * gptr->sideLength, sizeof(double));
    error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, empty_cells*gptr->sideLength, objective_solution);
    if (error) {
        printf("ERROR %d GRBgetdblattrarray(): %s\n", error, GRBgeterrormsg(env));
        return -1;
    }

    (*solution_address) = malloc(empty_cells * sizeof(double *));
    for (i=0; i<empty_cells; i++) {
        (*solution_address)[i] = calloc(gptr->sideLength, sizeof(double));
    }

    for (i=0; i<empty_cells; i++) {
        for (k=0; k<gptr->sideLength; k++) {
            (*solution_address)[i][k] = objective_solution[i*gptr->sideLength + k];
        }
    }

    free(objective_coefficient);
    free(objective_solution);
    free(variable_upper_bound);
    GRBfreemodel(model);
    GRBfreeenv(env);

    return (int) objective_value;
}

/* Fills a cell with a random value out of a pool of values with correctness probability of over threshold.
 * If more than one value is possible, start a random process to choose a fill value. The random process chooses
 * an index SAMPLE_SIZE times, adding it's occurrence probability to a score in each iteration, thus making sure higher
 * probability values will have their score increase more quickly, and chosen more often. The value with the
 * highest score is chosen to fill the cell. Invalid values or values with probability below threshold have their
 * probability reduced to 0, effectively making sure they are never chosen.
 *
 * TODO a note about randomness: it is technically possible that only random values with probability 0 will be randomly
 * chosen, as they are not removed from the array. HOWEVER, with a large enough SAMPLE_SIZE this is virtually impossible.
 * Say in a 5x5 board with 2 possible values to fill a cell out of 25, the probability to land on an invalid value once is 0.92,
 * and to land on an invalid value SAMPLE_SIZE times is 0.92^SAMPLE_SIZE. with the current SAMPLE_SIZE of 10000, this probability
 * is 0.92^10000 < 10^-325 */
int fill_cell(game *gptr, double *value_probability, int row, int col, float threshold) {
    int i, index, valid_values = 0, max_value = -1;
    double *value_score = calloc(gptr->sideLength, sizeof(double)); /* should implicitly initialize to zero */
    double max_score = 0;

    for (i=0; i<gptr->sideLength; i++) {
        /* if a value has probability lower than the threshold, or is invalid for the board,
        it's probability of being chosen drops to 0 */
        if (value_probability[i] < threshold || !check_valid_value(gptr, row, col, i+1)) {
            value_probability[i] = 0.0;
        } else {
            valid_values++;
        }
    }

    if (!valid_values) {
        free(value_score);
        return 0;

    } else if (valid_values == 1) {
        for (i=0; i<gptr->sideLength; i++) {
            if (value_probability[i] > 0.0) {
                gptr->user[row][col] = i+1;

                free(value_score);
                return 1;
            }
        }
    }

    for (i=0; i<SAMPLE_SIZE; i++) {
        index = rand() % gptr->sideLength;
        value_score[index] += value_probability[index];
    }

    for (i=0; i<gptr->sideLength; i++) {
        if (value_score[i] > max_score) {
            max_score = value_score[i];
            max_value = i;
        }
    }

    if (max_value >= 0) {
        gptr->user[row][col] = max_value + 1;
        free(value_score);
        return 1;
    }

    free(value_score);
    return 0;
}

/********************************************************/
/* TODO             GUROBI  OPERATIONS             TODO */
/********************************************************/

int guess_hint(game *gptr, int row, int col) {
    int i, empty_cells, objective_value, hint_cell = -1;
    int **cell_map = NULL;
    double **solution_probabilities = NULL;

    empty_cells = create_map(gptr, &cell_map);
    objective_value = gurobi_lp(gptr, cell_map, &solution_probabilities, empty_cells);

    if (objective_value < 0) {
        if (objective_value == -1) {
            printf("There was an error in one of gurobi's functions.\n");
        } else if (objective_value == -2) {
            printf("Linear model of board is infeasible or unbounded.\n");
        } else {
            printf("Unknown error occured, invalid objective value.\n");
        }

        free_2d_array(cell_map, empty_cells);
        free_2d_double_array(solution_probabilities, empty_cells);
        return -1;
        /* replace with some error value. Consider different return values for for different error causes */
    }

    for (i=0; i<empty_cells; i++) {
        if (cell_map[i][0] == row && cell_map[i][1] == col) {
            hint_cell = i;
            break;
        }
    }

    if (hint_cell == -1) {
        printf("Selected cell not found.\n");
        return -1;
    }

    printf("Possible values for [%d, %d]:  ", col ,row); /* flip coordinates when printing due to X,Y scheme */
    for (i=0; i<gptr->sideLength; i++) {
        if (solution_probabilities[hint_cell][i] > 0.0) {
            printf("%d:%.2f  ", i+1, solution_probabilities[hint_cell][i]);
        }
    }
    printf("\n");

    free_2d_array(cell_map, empty_cells);
    free_2d_double_array(solution_probabilities, empty_cells);
    return 0;
}

int guess(game *gptr, float threshold) {
    int i, empty_cells, objective_value, filled_cells = 0;
    int **cell_map = NULL;
    double **solution_probabilities = NULL;

    empty_cells = create_map(gptr, &cell_map);
    objective_value = gurobi_lp(gptr, cell_map, &solution_probabilities, empty_cells);

    if (objective_value < 0) {
        if (objective_value == -1) {
            printf("There was an error in one of gurobi's functions.\n");
        } else if (objective_value == -2) {
            printf("Linear model of board is infeasible or unbounded.\n");
        } else {
            printf("Unknown error occured, invalid objective value.\n");
        }

        free_2d_array(cell_map, empty_cells);
        free_2d_double_array(solution_probabilities, empty_cells);
        return -1;
        /* replace with some error value. Consider different return values for for different error causes */
    }

    for (i=0; i<empty_cells; i++) {
        filled_cells += fill_cell(gptr, solution_probabilities[i], cell_map[i][0], cell_map[i][1], threshold);
    }

    free_2d_array(cell_map, empty_cells);
    free_2d_double_array(solution_probabilities, empty_cells);
    return filled_cells;
}
