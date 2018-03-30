CC = gcc
CFLAGS = -Wall -Wconversion -Werror -Wextra -Wpedantic -fPIC -std=gnu99
LDFLAGS = -pthread

all: client alpha

main.o: main.c prompt.h
	$(CC) -c $(CFLAGS) main.c

prompt.o: prompt.c prompt.h
	$(CC) -c $(CFLAGS) prompt.c


client: main.o prompt.o
	$(CC) $(CFLAGS) main.c $(LDFLAGS) -o $@ prompt.o
	
alpha.o: alpha.c prompt.h
	$(CC) -c $(CFLAGS) alpha.c

alpha: alpha.o prompt.o
	$(CC) $(CFLAGS) alpha.c $(LDFLAGS) -o $@ prompt.o

clean:
	$(RM) *.o
