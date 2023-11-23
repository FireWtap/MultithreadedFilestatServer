OBJS	= basicUtilities.o main.o unboundedqueue.o
SOURCE	= basicUtilities.c main.c unboundedqueue.c
HEADER	= basicUtilities.h unboundedqueue.h util.h
OUT	= a.out
CC	 = gcc
FLAGS	 = -g -c -w
LFLAGS	 = -lpthread -lm

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

basicUtilities.o: basicUtilities.c
	$(CC) $(FLAGS) basicUtilities.c

main.o: main.c
	$(CC) $(FLAGS) main.c

unboundedqueue.o: unboundedqueue.c
	$(CC) $(FLAGS) unboundedqueue.c


clean:
	rm -f $(OBJS) $(OUT)

test1:
	./a.out path 1
test2:
	./a.out path 5
test3:
	valgrind --leak-check=full ./a.out path 5

