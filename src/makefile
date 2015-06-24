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

test.o: test.c
	$(CC) -c test.c

Function.o: Function.c Function.h
	$(CC) -c Function.c

StringExtensions.o: StringExtensions.c StringExtensions.h
	$(CC) -c StringExtensions.c

ErrorMessage.o: ErrorMessage.c ErrorMessage.h
	$(CC) -c ErrorMessage.c

Parse.o: Parse.c Parse.h
	$(CC) -c Parse.c

#Need dis
