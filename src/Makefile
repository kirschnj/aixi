CC=g++
CFLAGS=-Wall
LDFLAGS=-Wall

SRCS=main.cpp agent.cpp pacman.cpp environment.cpp predict.cpp search.cpp util.cpp
OBJS=$(SRCS:.cpp=.o)

all: aixi

aixi: $(OBJS)
	$(CC) $(LDFLAGS) -o aixi $(OBJS)

.PHONY: clean

clean:
	rm -f $(OBJS)
