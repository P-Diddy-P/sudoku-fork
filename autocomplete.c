#include "autocomplete.h"

/* if there's more than one valid value per cell,
 * return false. Implemented as loop on all possible values
 * in range, where the first one sets the validValue flags to
 * a non-zero value. if there's another one, the function returns
 * false */
int find_single_valid_value(game *gptr, int row, int col) {
	int k, validValue = 0;

	for (k = 1; k <= gptr->sideLength; k++) {
		if (check_valid_value(gptr, row, col, k)) {
			if (validValue) {
				return 0;
			} else {
				validValue = k;
			}
		}
	}
	return validValue;
}

/* Goes over all empty cells in the board ONCE, filling each cell only if there is
 * a single possible value which can be put in the cell with the board staying valid.
 * returns true if any cell was filled during the run, false otherwise.
 */
int auto_complete(game *gptr) {
	int currentRow = 0, currentCol = 0;
	int svPlaceholder = 0, cellsFilled = 0;
	int *stkres = malloc(3 * sizeof(int));

	stack stk;
	stack *stkptr = &stk;

	stack_init(stkptr);

	while (!find_next_empty_cell(gptr, &currentRow, &currentCol)) {
		if ((svPlaceholder = find_single_valid_value(gptr, currentRow,
				currentCol))) {
			stack_push(stkptr, currentRow, currentCol, svPlaceholder);
			cellsFilled++;
		}

		currentRow += (currentCol + 1) / gptr->sideLength;
		currentCol = (currentCol + 1) % gptr->sideLength;
	}

	stack_pop(stkptr, stkres);
	while (stkres[2] > -1) {
		gptr->user[stkres[0]][stkres[1]] = stkres[2];
		stack_pop(stkptr, stkres);
	}

	free(stkres);
	stack_free(stkptr);
	return cellsFilled;
}

/* Goes over all empty cells in the board, filling each cell only if there is
 * a single possible value which can be put in the cell with the board staying valid.
 * This function uses auto_complete as a subroutine, and runs as many times as necessary
 * until it receives a 0 from auto_complete, i.e. no more cells could be filled in a single run.
 *
 * Returns the number of auto_complete rounds needed to fill the board with obvious values.
 */
int iterative_auto_complete(game *gptr) {
	int autocompleteRounds = 0;

	while (auto_complete(gptr)) {
		autocompleteRounds++;
	}

	return autocompleteRounds;
}
