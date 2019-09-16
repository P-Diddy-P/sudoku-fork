/*
 * main.c
 *
 *  Created on: 31 Jul 2019
 *      Author: itait
 */
# include "SPBufferset.h"
# include "userOp.h"
# include "parser.h"

/**/
int main() {
	int flags[NUM_FLAGS] = { 0 }, error;
	char *strings[NUM_STRINGS] = { 0 };

	game *gptr;
	node *currentMove;
	GRBenv *env=NULL;

	SP_BUFF_SET();
	srand(10);

	gptr = calloc(1, sizeof(game));
	memory_alloc_error();
	currentMove = calloc(1, sizeof(node));
	memory_alloc_error();

	error = GRBloadenv(&env, NULL);
	if (error) {
		printf("ERROR %d GRBloadenv: %s\n", error, GRBgeterrormsg(env));
		return -1;
	} else {
		errno = 0;
	}

	error = GRBsetintparam(env, GRB_INT_PAR_LOGTOCONSOLE, 0);
	if (error) {
		printf("ERROR %d GRBsetintattr(): %s\n", error, GRBgeterrormsg(env));
		return -1;
	} else {
		errno = 0;
	}

	flags[MODE] = MODE_INIT;
	print_art();

	while (1) {
		parse_user(flags, strings);
		user_op(gptr, flags, strings, &currentMove,env);

		if (flags[EOF_EXIT]) {
			break;
		}
	}
	return 0;
}

/*
 int main() {
 game *gptr;
 game new_game;
 int flags[] = {1, 0, 0, 0, 0, 0, 0, 0, 0};

 gptr = &new_game;
 init_board(gptr, 4, 4);

 gptr->user[0][4] = 3;
 gptr->user[0][11] = 2;
 gptr->user[1][0] = 16;
 gptr->user[1][3] = 11;
 gptr->user[1][5] = 8;
 gptr->user[1][10] = 3;
 gptr->user[1][12] = 13;
 gptr->user[1][15] = 4;
 gptr->user[2][1] = 9;
 gptr->user[2][2] = 14;
 gptr->user[2][3] = 12;
 gptr->user[2][12] = 2;
 gptr->user[2][13] = 7;
 gptr->user[2][14] = 1;
 gptr->user[3][1] = 2;
 gptr->user[3][2] = 3;
 gptr->user[3][4] = 7;
 gptr->user[3][6] = 4;
 gptr->user[3][7] = 15;
 gptr->user[3][8] = 13;
 gptr->user[3][9] = 12;
 gptr->user[3][11] = 6;
 gptr->user[3][13] = 11;
 gptr->user[3][14] = 10;
 gptr->user[4][0] = 1;
 gptr->user[4][2] = 4;
 gptr->user[4][6] = 6;
 gptr->user[4][7] = 9;
 gptr->user[4][8] = 5;
 gptr->user[4][9] = 16;
 gptr->user[4][13] = 2;
 gptr->user[4][15] = 13;
 gptr->user[5][3] = 10;
 gptr->user[5][6] = 15;
 gptr->user[5][9] = 8;
 gptr->user[5][12] = 5;
 gptr->user[6][0] = 11;
 gptr->user[6][1] = 16;
 gptr->user[6][5] = 13;
 gptr->user[6][7] = 8;
 gptr->user[6][8] = 10;
 gptr->user[6][10] = 2;
 gptr->user[6][14] = 4;
 gptr->user[6][15] = 15;
 gptr->user[7][1] = 5;
 gptr->user[7][2] = 9;
 gptr->user[7][4] = 1;
 gptr->user[7][5] = 7;
 gptr->user[7][7]  =10;
 gptr->user[7][8] = 15;
 gptr->user[7][10] = 12;
 gptr->user[7][11] = 4;
 gptr->user[7][13] = 8;
 gptr->user[7][14] = 14;
 gptr->user[8][1] = 1;
 gptr->user[8][2] = 13;
 gptr->user[8][4] = 8;
 gptr->user[8][5] = 2;
 gptr->user[8][7] = 14;
 gptr->user[8][8] = 11;
 gptr->user[8][10] = 16;
 gptr->user[8][11] = 12;
 gptr->user[8][13] = 15;
 gptr->user[8][14] = 7;
 gptr->user[9][0] = 7;
 gptr->user[9][1] = 11;
 gptr->user[9][5] = 6;
 gptr->user[9][7] = 13;
 gptr->user[9][8] = 9;
 gptr->user[9][10] = 15;
 gptr->user[9][14] = 16;
 gptr->user[9][15] = 2;
 gptr->user[10][3] = 14;
 gptr->user[10][6] = 12;
 gptr->user[10][9] = 10;
 gptr->user[10][12] = 6;
 gptr->user[11][0] = 6;
 gptr->user[11][2] = 16;
 gptr->user[11][6] = 3;
 gptr->user[11][7] = 5;
 gptr->user[11][8] = 2;
 gptr->user[11][9] = 7;
 gptr->user[11][13] = 4;
 gptr->user[11][15] = 10;
 gptr->user[12][1] = 15;
 gptr->user[12][2] = 5;
 gptr->user[12][4] = 6;
 gptr->user[12][6] = 10;
 gptr->user[12][7] = 4;
 gptr->user[12][8] = 1;
 gptr->user[12][9] = 11;
 gptr->user[12][11] = 13;
 gptr->user[12][13] = 9;
 gptr->user[12][14] = 2;
 gptr->user[13][2] = 4;
 gptr->user[13][2] = 1;
 gptr->user[13][3] = 3;
 gptr->user[13][12] = 15;
 gptr->user[13][13] = 6;
 gptr->user[13][14] = 13;
 gptr->user[14][0] = 14;
 gptr->user[14][3] = 6;
 gptr->user[14][5] = 15;
 gptr->user[14][10] = 8;
 gptr->user[14][12] = 4;
 gptr->user[14][15] = 1;
 gptr->user[15][4] = 12;
 gptr->user[15][11] = 15;

 update_board_errors(gptr);
 print_board_aux(gptr, flags);
 printf("num solutions %d", stack_tracking(gptr));

 return 0;
 }*/
