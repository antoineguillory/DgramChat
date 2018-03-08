CC = gcc
CFLAGS = -Wall -Wconversion -Werror -Wextra -Wpedantic -fPIC -D_POSIX_C_SOURCE=200112L
LDFLAGS = -pthread

all: client servertest

main.o: main.c prompt.h message.h
	$(CC) -c $(CFLAGS) main.c

prompt.o: prompt.c prompt.h message.h
	$(CC) -c $(CFLAGS) prompt.c

servertest.o : servertest.c message.h
	$(CC) -c $(CFLAGS) servertest.c

servertest : servertest.o
	$(CC) $(CFLAGS) servertest.c $(LDFLAGS) -o $@

client: main.o prompt.o
	$(CC) $(CFLAGS) main.c $(LDFLAGS) -o $@ prompt.o

clean:
	$(RM) *.o
