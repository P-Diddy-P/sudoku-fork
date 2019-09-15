CC = gcc
OBJS = main.o userOp.o undoRedo.o autocomplete.o autocomplete.o stackTracking.o LPsolver.o\
       ILPsolver.o parser.o gurobi_interface.o gameStruct.o errors.o
EXEC = sudoku-console
COMP_FLAG = -ansi -Wall -Wextra -Werror -pedantic-errors
GUROBI_COMP = -I/usr/local/lib/gurobi563/include
GUROBI_LIB = -L/usr/local/lib/gurobi563/lib -lgurobi56

.PHONY: clean all

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(OBJS) $(GUROBI_LIB) -o $@ -lm
main.o: main.c userOp.c userOp.h parser.c parser.h SPBufferset.h
	$(CC) $(COMP_FLAG) -c $*.c
userOp.o: userOp.c userOp.h LPsolver.c LPsolver.h ILPsolver.c ILPsolver.h autocomplete.c autocomplete.h \
undoRedo.c undoRedo.h
	$(CC) $(COMP_FLAG) -c $*.c
undoRedo.o: undoRedo.c undoRedo.h gameStruct.c gameStruct.h
	$(CC) $(COMP_FLAG) -c $*.c
autocomplete.o: autocomplete.c autocomplete.h stackTracking.c stackTracking.h
	$(CC) $(COMP_FLAG) -c $*.c
stackTracking.o: stackTracking.c stackTracking.h gameStruct.c gameStruct.h
	$(CC) $(COMP_FLAG) -c $*.c
LPsolver.o: LPsolver.c LPsolver.h gurobi_interface.c gurobi_interface.h
	$(CC) $(COMP_FLAG) -c $*.c	
ILPsolver.o: ILPsolver.c ILPsolver.h gurobi_interface.c gurobi_interface.h
	$(CC) $(COMP_FLAG) -c $*.c
parser.o: parser.c parser.h auxi.h
	$(CC) $(COMP_FLAG) -c $*.c
gurobi_interface.o: gurobi_interface.c gurobi_interface.h 
	$(CC) $(COMP_FLAG)$(GUROBI_COMP) -c $*.c
gameStruct.o: gameStruct.c gameStruct.h auxi.h
	$(CC) $(COMP_FLAG) -c $*.c
errors.o: errors.c errors.h
	$(CC) $(COMP_FLAG) -c $*.c
clean:
	rm -f $(OBJS) $(EXEC)