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

parser: test.o Function.o StringExtensions.o Parse.o
	$(CC) -o parser test.o Function.o StringExtensions.o Parse.o #-$(I)

test.o: test.c
	$(CC) -c test.c

Function.o: Function.c Function.h
	$(CC) -c Function.c

StringExtensions.o: StringExtensions.c StringExtensions.h Boolean.h
	$(CC) -c StringExtensions.c

Parse.o: Parse.c Parse.h
	$(CC) -c Parse.c

#Need dis
