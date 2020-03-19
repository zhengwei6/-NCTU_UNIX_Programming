CC = gcc
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

all: hw1
	
%.o : %.c
	$(CC) -c $< -o $@

hw1 : $(OBJS)
	$(CC) -o hw1 $(OBJS)

clean:
	rm *.o

run:
	./hw1