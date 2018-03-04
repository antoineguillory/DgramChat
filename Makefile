CC = gcc
CFLAGS = -std=gnu99 -Wall -Wconversion -Werror -Wextra -Wpedantic -fPIC -D_XOPEN_SOURCE=500 -D_FORTIFY_SOURCE
LDFLAGS = -pthread

all: testhistory

testhistory.o: testhistory.c history.h message.h tools.h
	$(CC) -c testhistory.c history.h message.h tools.h

testhistory:
	$(CC) $(CFLAGS) testhistory.c $(LDFLAGS) -o $@

clean:
	$(RM) testhistory.o

