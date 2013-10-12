CC=g++
CFLAGS=-Wall
LDFLAGS=-Wall

SRCS=main.cpp agent.cpp pacman.cpp environment.cpp predict.cpp search.cpp util.cpp
OBJS=$(SRCS:.cpp=.o)

all: aixi

pacman: $(OBJS)
	$(CC) -c -o test_pacman.o test_pacman.cpp
	$(CC) $(LDFLAGS) -o test pacman.o util.o test_pacman.o

aixi: $(OBJS)
	$(CC) $(LDFLAGS) -o aixi $(OBJS)

.PHONY: clean

clean:
	rm -f $(OBJS)
