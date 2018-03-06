CC = gcc
CFLAGS = -Wall -Wconversion -Werror -Wextra -Wpedantic -fPIC -D_POSIX_C_SOURCE=200112L
LDFLAGS = -pthread

all: client

main.o: main.c prompt.h message.h
	$(CC) -c $(CFLAGS) main.c

prompt.o: prompt.c prompt.h message.h
	$(CC) -c $(CFLAGS) prompt.c


client: main.o prompt.o
	$(CC) $(CFLAGS) main.c $(LDFLAGS) -o $@ prompt.o

clean:
	$(RM) *.o
