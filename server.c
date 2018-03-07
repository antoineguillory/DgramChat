#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/select.h>
#include <sys/time.h>
#include "addrpool.h"
#include "message.h"
#define BUF_SIZE 512

#define HOST_SIZE_STRING 256
#define SERVICE_SIZE_STRING 256

typedef struct addrpool addrpool;

int main(int argc, char* argv[]) {

    //Initialisation du socket de reception
    int s;
    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket");
        return EXIT_FAILURE;
    }

    //Recherche d'adresse valable
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;
    struct addrinfo *res;
    int r;
    if ((r = getaddrinfo(NULL, "50000", &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo(): %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
    }
    if (res == NULL) {
        fprintf(stderr, "Erreur lors de la construction de l'adresse\n");
        exit(EXIT_FAILURE);
    }
    //On bind cette adresse au socket
    if (bind(s, (struct sockaddr*) res->ai_addr, sizeof(struct sockaddr)) == -1) {
        perror("bind");
        return EXIT_FAILURE;
    }
    freeaddrinfo(res);

    //Préparation du select
    fd_set input_set;
    FD_ZERO(&input_set);
    FD_SET(s, &input_set);
    struct timeval timeout;
    timeout.tv_sec = 15;
    timeout.tv_usec = 0;
    int retselect = 0;

    //Init du pool d'adresse
    addrpool *ap = init_pool();
    //La boucle principale.
    while (retselect != -1) {
        //On reçoit le message
        Message *m = malloc(sizeof(*m));
        retselect = select(1, &input_set, NULL, NULL, &timeout);
        struct sockaddr_in addr_recept;
        int retrecv = recvfrom(s, m, sizeof(*m), 0, (struct sockaddr*)&addr_recept, NULL);

        if (m == NULL) continue; // à traiter

        char actualsender[50];

        //On ajoute au pool l'écrivain si il n'y ai pas déjà
        strncpy(actualsender, m -> sender, 50);
        if (!isInPool(ap, actualsender)) {
            putInPool(ap, actualsender, (struct sockaddr*) &addr_recept);
        }

        
        int sizepool = poolSize(ap);
        //ICI boucle qui énumère toute les adresses du pool.
        for (int i = 0; i <= sizepool; i++){
            struct sockaddr *ad = get_addr_at(ap, i);
            //ici t'as une adresse, c'est là qui faut gérer les threads
        }
    }



    if (close(s) == -1) {
        perror("close");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
