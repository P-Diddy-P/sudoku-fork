/*
 * errors.h
 *
 *  Created on: 14 Sep 2019
 *      Author: itait
 */

#ifndef ERRORS_H_
#define ERRORS_H_
# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <errno.h>
# include <stdarg.h>
# include <ctype.h>
# include <math.h>
# include <time.h>

/* Module purpose:
 * 	include s error functions to be used by all modules.
 * 	since all modules directly or indirectly include this module,
 * 	it serves a kolboinik module for all standard libraries includes */

void memory_alloc_error();

#endif /* ERRORS_H_ */
