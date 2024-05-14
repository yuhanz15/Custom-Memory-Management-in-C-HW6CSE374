# Yuhan Zhang
# 12/16/2022
# CSE 374 Homework 6
# HW6 Makefile

CC = gcc
CARGS = -Wall -std=c11

all: bench

bench: bench.o getmem.o freemem.o mem_utils.o
	$(CC) $(CARGS) -o bench $^

noassert: CARGS += -D NDEBUG
noassert: bench

debug: CARGS += -g
debug: bench

test: 
	./bench 1000

test1:
	./bench 100

test2:
	./bench 10

test3:
	./bench 1

test4:
	./bench 1 100 0 160 1600 1

clean:
	rm -f bench *.o *~ 
