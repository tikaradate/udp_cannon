CFLAGS = -Wall
CC     = gcc
OBJS   = relatorios.o

all: client server

server: $(OBJS)

clean: 
	rm *.o

purge: clean
	rm client server