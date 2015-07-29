# 
# =======================================================================
# Function.c
#
# Jesse Bannon
#
# TODO
# =======================================================================
#
CC=gcc
I=

all: parser

parser: test.o Function.o StringExtensions.o Parse.o ErrorMessage.o
	$(CC) -o parser test.o StringExtensions.o ErrorMessage.o Function.o Parse.o #-$(I)
	rm -f *.o

test.o: test.c
	$(CC) -c test.c

Function.o: function/Function.c function/Function.h
	$(CC) -c function/Function.c

StringExtensions.o: utilities/StringExtensions.c utilities/StringExtensions.h
	$(CC) -c utilities/StringExtensions.c

ErrorMessage.o: utilities/ErrorMessage.c utilities/ErrorMessage.h
	$(CC) -c utilities/ErrorMessage.c

Parse.o: function/parse/Parse.c function/parse/Parse.h
	$(CC) -c function/parse/Parse.c

#Need dis
