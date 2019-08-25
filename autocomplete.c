#include "autocomplete.h"


int find_single_valid_value(game *gptr, int row, int col) {
	int k, validValue = 0;

	/* if there's more than one valid value per cell,
	 * return false. Implemented as loop on all possible values
	 * in range, where the first one sets the validValue flags to
	 * a non-zero value. if there's another one, the function returns
	 * false */
	for (k=1; k<=gptr->sideLength; k++) {
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


int auto_complete(game *gptr) {
	int currentRow = 0, currentCol = 0;
	int svPlaceholder = 0, cellsFilled = 0;

	while(!find_next_empty_cell(gptr, &currentRow, &currentCol)) {
		if ( (svPlaceholder = find_single_valid_value(gptr, currentRow, currentCol)) ) {
			gptr->user[currentRow][currentCol] = svPlaceholder;
			cellsFilled++;
		} else {
			currentRow += (currentCol + 1) / gptr->sideLength;
			currentCol = (currentCol + 1) % gptr->sideLength;
		}
	}
	return cellsFilled;
}


int iterative_auto_complete(game *gptr) {
	int autocompleteRounds = 0;

	while (auto_complete(gptr)) {
		autocompleteRounds++;
	}

	return autocompleteRounds;
}
