//
// Created by zartant on 06/03/18.
//

#ifndef PROJETRESEAU_ADDRPOOL_H
#define PROJETRESEAU_ADDRPOOL_H

#include <stdlib.h>
#include <stdbool.h>
typedef struct cell_addrpool cell_addrpool;
struct cell_addrpool {
    cell_addrpool *next;
    char name[50];
    struct sockaddr *addr;
};



struct addrpool {
    cell_addrpool *list;
};


typedef struct addrpool addrpool;
addrpool *init_pool();

struct sockaddr *get_addr(addrpool *m, char *user);

int putInPool(addrpool *m, char *user, struct sockaddr *add);

bool isInPool(addrpool *m, char *user);

int poolSize(addrpool *m);

struct sockaddr *get_addr_at(addrpool *m, int at);
#endif //PROJETRESEAU_ADDRPOOL_H
