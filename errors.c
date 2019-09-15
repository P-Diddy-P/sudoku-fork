/*
 * errors.c
 *
 *  Created on: 14 Sep 2019
 *      Author: itait
 */

# include "errors.h"

void memory_alloc_error(){
	if (errno) {
		printf("Error during game memory allocation\n");
		exit(errno);
	}
}
