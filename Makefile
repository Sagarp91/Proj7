# CMPSC 311 Project 7, makefile for use with GCC
# Erich Stoekl and Andrew Moyer
# ems5311@psu.edu and abm5149@psu.edu
#
#

SRC = pr7.4.c
LIB = cmpsc311.c pr7_signal.c builtin.c pr7_table.c
INC = cmpsc311.h pr7_signal.h builtin.h pr7_table.h
STD = -std=c99

ALL = $(SRC) $(LIB) $(INC)

pr7: $(ALL)
	gcc -std=gnu99 -Wall -Wextra -o pr7 $(SRC) $(LIB)

clean:
	rm -f pr7 a.out
