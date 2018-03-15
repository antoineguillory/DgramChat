//
// Created by zartant on 06/03/18.
//

/*Implémente un pool d'adresse énumérable sous forme d'une liste.
 *
 *
 */
#ifndef PROJETRESEAU_ADDRPOOL_H
#define PROJETRESEAU_ADDRPOOL_H

#include <stdlib.h>
#include <stdbool.h>
//J'ajoute les définitions complètes par soucis de compilation, mais il faudrait à l'idéal les retirer pour le .c

typedef struct cell_addrpool cell_addrpool;

typedef struct addrpool addrpool;

//Initialise et alloue un pool prêt à l'envoi, renvoi son adresse
addrpool *init_pool();

//renvoi l'adresse correspondant à user si elle existe, NULL sinon
struct sockaddr *get_addr(addrpool *m, char *user);

//ajoute une valeur user/addresse au pool. Renvoi 0 si ça s'est bien passé, -1 sinon
int putInPool(addrpool *m, char *user, struct sockaddr *add);

//Renvoi vrai au sens booléen si il existe une valeur user/addresse correspondante. Faux sinon
bool isInPool(addrpool *m, char *user);

//Renvoi le nombre de cellule
int poolSize(addrpool *m);

//Renvoi l'adresse à l'index at. NULL si l'index n'est pas bon;
struct sockaddr *get_addr_at(addrpool *m, int at);
#endif //PROJETRESEAU_ADDRPOOL_H
