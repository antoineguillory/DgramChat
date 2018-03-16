#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include "addrpool.h"

struct cell_addrpool {
    cell_addrpool *next;
    char name[50];
    struct sockaddr *addr;
};

struct addrpool {
    cell_addrpool *list;
};

addrpool *init_pool(){
    addrpool *ret = malloc(sizeof(*ret));
    ret ->list = NULL;
    return ret;
}

int putInPool(addrpool *m, char *user, struct sockaddr *add) {
    printf("Addr pool put started with user %s\n",user);
    cell_addrpool *c = m -> list;
    while (c != NULL) {
        c = c -> next;
    }
    c = malloc(sizeof(*c));
    if (c == NULL) {
        return -1;
    }
    strncpy(c->name, user, strlen(user));
    c -> addr = add;
    return 0;
}
struct sockaddr *get_addr(addrpool *m, char *user) {
    cell_addrpool *c = m  -> list;
    while (c != NULL && strcmp(user, c ->name) == 0) {
        c = c -> next;
    }
    if (c == NULL) return NULL;
    return c -> addr;
}

bool isInPool(addrpool *m, char *user) {
    return (get_addr(m, user)) != NULL;
}

int poolSize(addrpool *m) {
    cell_addrpool *c = m -> list;
    int i = 0;
    while (c != NULL) {
        c = c -> next;
        ++i;
    }
    return i;
}

struct sockaddr *get_addr_at(addrpool *m, int at) {
    cell_addrpool *c = m -> list;
    int i = 0;
    while (c != NULL && i <= at) {
        c = c -> next;
        ++i;
    }
    if (c == NULL) return NULL;
    return c -> addr;
}