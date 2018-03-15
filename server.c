#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/select.h>
#include <pthread.h>
#include <sys/time.h>
#include "addrpool.h"
#include "history.h"
//#include "message.h"
#include "tools.h"

#define BUF_SIZE 512

#define HOST_SIZE_STRING 256
#define SERVICE_SIZE_STRING 256
//Je commente car en posix un tel typedef redefinit un autre typedef systeme nomé addrpool...
//typedef struct addrpool addrpool;


typedef struct _Handler_Struct {
    int sockfd;
    struct sockaddr* addr;
    char msg[350];
} Handler_Struct;

void* handler_pthread(void* arg){
    if(arg==NULL)
        return NULL;
    Handler_Struct* hs = (Handler_Struct*)(arg);
    sendto(hs->sockfd, hs->msg, sizeof(hs->msg), 0, hs->addr, sizeof(struct sockaddr_in));
    return NULL;
}

int main(void) {

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
   /* fd_set input_set;
    FD_ZERO(&input_set);
    FD_SET(s, &input_set);
    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    int retselect = 0; */

    //Init du pool d'adresse
    addrpool *ap = init_pool();
    //La boucle principale.
    for(;;) {
        //On reçoit le message
        char m[350];
        //retselect = select(1, &input_set, NULL, NULL, &timeout);
        struct sockaddr_in addr_recept;
        socklen_t *csize = malloc(sizeof(*csize));
        int retrecv = (int)recvfrom(s, m, sizeof(m), 0, (struct sockaddr*)&addr_recept, csize);
        free(csize);
        if (m == NULL) {
            continue; // à traiter
        }
        if(retrecv==-1) {
            perror("retrecv");
            return EXIT_FAILURE;
        }
        char actualsender[50];

        char tmp[350];
        strcpy(tmp, m);
        char* token = strtok(tmp, ";");
        strcpy(actualsender, token);
        //!!! TODO Penser à l'entrée dans l'historique

        //On ajoute au pool l'écrivain si il n'y ai pas déjà

        if (!isInPool(ap, actualsender)) {
            putInPool(ap, actualsender, (struct sockaddr*) &addr_recept);
            //!!!TODO ENVOYER INFO (30 derniers messages)
            char histobrief[8192];
            strcpy(histobrief,get_history_brief());
            if(histobrief==NULL){
                fprintf(stderr, "Failed to retrieve history\n");
            }
            socklen_t socksize = sizeof(get_addr(ap,actualsender));
            if(sendto(s,histobrief,strlen(histobrief),0,get_addr( ap, actualsender),socksize)==-1){
                fprintf(stderr, "Failed to send history brief\n");
            }
            
        }

        int sizepool = poolSize(ap);
        //ICI boucle qui énumère toute les adresses du pool.
        for (int i = 0; i <= sizepool; ++i){
            Handler_Struct* hs = malloc(sizeof(struct Handler_Struct*));
            struct sockaddr *ad = get_addr_at(ap, i);
            memcpy(hs->addr,ad,sizeof(struct sockaddr));
            strcpy(hs -> msg, m);
            hs->sockfd = s;
            pthread_t thread;
            pthread_detach(thread);
            if(pthread_create(&thread, NULL, &handler_pthread , hs) == -1) {
                perror("pthread_create");
                return EXIT_FAILURE;
            }
        }
    }
    if (close(s) == -1) {
        perror("close");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}


