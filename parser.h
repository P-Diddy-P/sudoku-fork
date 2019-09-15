/*
 * parser.h
 *
 *  Created on: 31 Jul 2019
 *      Author: itait
 *
 *
 * Parser module purpose:
 * Parsing user input, and decide if it's valid mostly SYNTACTICALLY,
 * i.e, if the command name and number of arguments are valid.
 * THAT SAID, because game-mode dependent errors need to be printed to
 * user without regard to correctness of arguments.
 * For example, using the command "generate" in SOLVE_STR mode should result in error,
 * without regard to the rest of the arguments.
 * The correctness of the arguments in regard to board dimensions,
 * current cell value and so on, will be treated in user_op module.
 */

#ifndef PARSER_H_
#define PARSER_H_
# include "auxi.h"
# include "errors.h"

# define BUFF_SIZE 257
# define DELIM " \t\r\n"

void parse_user(int *flags, char *strings[]);

#endif /* PARSER_H_ */
