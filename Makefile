

CFLAGS =  -std=c99 -g

LDFLAGS = -lpthread -lm



.PHONY: all
all:  d1 d2 d3 d4 d5 d6 d7

portable_fputs_unlocked.o: portable_fputs_unlocked.c
	gcc ${CFLAGS} -std=gnu89 -c portable_fputs_unlocked.c

d1: d1.c portable_fputs_unlocked.o
	gcc ${CFLAGS} ${LDFLAGS} d1.c portable_fputs_unlocked.o -o d1

d2: d2.c portable_fputs_unlocked.o
	gcc ${CFLAGS} ${LDFLAGS} d2.c portable_fputs_unlocked.o -o d2

d3: d3.c portable_fputs_unlocked.o
	gcc ${CFLAGS} ${LDFLAGS} d3.c portable_fputs_unlocked.o -o d3
	
d4: d4.c portable_fputs_unlocked.o
	gcc ${CFLAGS} ${LDFLAGS} d4.c portable_fputs_unlocked.o -o d4

d5: d5.c portable_fputs_unlocked.o barrier.o
	gcc ${CFLAGS} ${LDFLAGS} d5.c barrier.o -o d5

d6: d6.c portable_fputs_unlocked.o barrier.o
	gcc ${CFLAGS} ${LDFLAGS} d6.c barrier.o -o d6

d7: d7.c portable_fputs_unlocked.o barrier.o
	gcc ${CFLAGS} ${LDFLAGS} d7.c barrier.o -o d7


	
barrier.o: barrier.c barrier.h
	gcc ${CFLAGS} ${LDFLAGS} -c barrier.c
	
	
.PHONY: clean
clean:
	rm -rf   d1 d2 d3 d4 d5 d6 d7 *.o *.dSYM

