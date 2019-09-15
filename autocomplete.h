#ifndef AUTOCOMPLETE_H_
#define AUTOCOMPLETE_H_

#include "stackTracking.h"


int auto_complete(game *gptr);
/* Goes over all empty cells in the board ONCE, filling each cell only if there is
 * a single possible value which can be put in the cell with the board staying valid.
 * returns true if any cell was filled during the run, false otherwise.
 */

int iterative_auto_complete(game *gptr);
/* Goes over all empty cells in the board, filling each cell only if there is
 * a single possible value which can be put in the cell with the board staying valid.
 * This function uses auto_complete as a subroutine, and runs as many times as necessary
 * until it receives a 0 from auto_complete, i.e. no more cells could be filled in a single run.
 *
 * Returns the number of auto_complete rounds needed to fill the board with obvious values.
 */

#endif /* AUTOCOMPLETE_H_ */
