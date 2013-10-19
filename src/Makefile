CC=g++
CXXFLAGS=-Wall -O2
LDFLAGS=-lncurses

SRCS=main.cpp agent.cpp pacman.cpp environment.cpp predict.cpp search.cpp util.cpp
OBJS=$(SRCS:.cpp=.o)

all: aixi

debug: $(OBJS)
	$(CC) $(CXXFLAGS) -pg -o aixi $(OBJS) $(LDFLAGS)

gdb: $(OBJS)
	$(CC) $(CXXFLAGS) -pg -g $(SRCS) -o aixi $(LDFLAGS)

pacman:
	$(CC) -c -o pacman.o pacman.cpp
	$(CC) -c -o environment.o environment.cpp
	$(CC) -c -o util.o util.cpp
	$(CC) -c -o test_pacman.o test_pacman.cpp
	$(CC) $(CXXFLAGS) -o test pacman.o util.o test_pacman.o $(LDFLAGS)

aixi: $(OBJS)
	$(CC) $(CXXFLAGS) -o aixi $(OBJS) $(LDFLAGS)

.PHONY: clean

clean:
	rm -f *.o aixi test
