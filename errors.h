/*
 * errors.h
 *
 *  Created on: 14 Sep 2019
 *      Author: itait
 */

#ifndef ERRORS_H_
#define ERRORS_H_

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

/* Module purpose:
 * 	include s error functions to be used by all modules.
 * 	since all modules directly or indirectly include this module,
 * 	it serves a kolboinik module for all standard libraries includes */

void memory_alloc_error();

#endif /* ERRORS_H_ */
