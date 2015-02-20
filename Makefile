CC=g++
INC=-I ./include/
FLAG=-std=c++11
LIB=
SRC=./src
OBJ=$(SRC)/HOReMap.o
EXE=main

default: $(EXE)

$(EXE): $(EXE).o $(OBJ)
	$(CC) $(FLAG) $(LIB) $(INC) -o $@ $@.o $(OBJ)

.C.o:
	$(CC) -c $(FLAG) $(INC) -o $@ $<

clean: 
	rm -f $(EXE)
	rm -f $(EXE).o
	rm -f $(OBJ)

