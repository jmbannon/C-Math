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

parser: test.o Function.o StringExtensions.o ErrorMessage.o ParseUtils.o Parse.o
	$(CC) -o parser test.o StringExtensions.o ErrorMessage.o Function.o ParseUtils.o Parse.o #-$(I)
	rm -f *.o

test.o: test.c
	$(CC) -c test.c -o test.o

Function.o: function/Function.c function/Function.h
	$(CC) -c function/Function.c -o Function.o

StringExtensions.o: utilities/StringExtensions.c utilities/StringExtensions.h
	$(CC) -c utilities/StringExtensions.c -o StringExtensions.o

ErrorMessage.o: utilities/ErrorMessage.c utilities/ErrorMessage.h
	$(CC) -c utilities/ErrorMessage.c -o ErrorMessage.o

Parse.o: function/parse/Parse.c function/parse/Parse.h
	$(CC) -c function/parse/Parse.c -o Parse.o

ParseUtils.o: function/parse/ParseUtilities.c function/parse/ParseUtilities.h
	$(CC) -c function/parse/ParseUtilities.c -o ParseUtils.o

#Need dis
