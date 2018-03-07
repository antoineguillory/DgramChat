CC = gcc
CFLAGS = -std=gnu99 -Wall -Wconversion -Werror -Wextra -Wpedantic -fPIC -D_FORTIFY_SOURCE
LDFLAGS = -pthread

all: server

testhistory.o: testhistory.c history.h message.h tools.h
	$(CC) -c $(CFLAGS) testhistory.c

history.o: history.c history.h message.h tools.h
	$(CC) -c $(CFLAGS) history.c

tools.o: tools.c tools.h
	$(CC) -c $(CFLAGS) tools.c

message.o: message.h
	$(CC) -c $(CFLAGS) message.h

testhistory: testhistory.o history.o tools.o message.o
	$(CC) $(CFLAGS) testhistory.c $(LDFLAGS) -o $@ history.o tools.o

server.o: addrpool.h message.h server.c
	$(CC) -c $(CFLAGS) server.c message.h addrpool.h

server: server.o addrpool.h message.h
	$(CC) -c $(CFLAGS) server.c $(LDFLAGS) -o $@ server.o

clean:
	$(RM) *.o
