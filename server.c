#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
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
    struct sockaddr_in *addr;
    char msg[350];
} Handler_Struct;

void* handler_pthread(void* arg){
    if(arg==NULL)
        return NULL;
    Handler_Struct* hs = (Handler_Struct*)(arg);
    size_t size = strlen(hs->msg);
    socklen_t adsize = sizeof(struct sockaddr_in);
    sendto(hs->sockfd, hs->msg, size, 0, (struct sockaddr *)hs->addr, adsize );
    return NULL;
}

int main(void) {

    //Initialisation du socket de reception
    int s;
    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket");
        return EXIT_FAILURE;
    }

    /*struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(50000);
    server.sin_addr.s_addr = htonl(INADDR_ANY);*/
  
    // On construit l'adresse de notre serveur.
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

    // On affecte l'adresse à notre serveur
    if (bind(s, (struct sockaddr*) res->ai_addr, sizeof(struct sockaddr)) == -1) {
        perror("bind");
        return EXIT_FAILURE;
    }
    freeaddrinfo(res);
    addrpool *ap = init_pool();
    for( ; ; ) {
        //On reçoit le message
        char m[350];
        struct sockaddr_in addr_recept;
        socklen_t slen = (socklen_t)sizeof(struct sockaddr_in);
        size_t msize = sizeof(m);
        int retrecv = (int)recvfrom(s, m, msize, 0, (struct sockaddr*)&addr_recept, &slen);
        printf("addresse = %s\n", inet_ntoa(addr_recept.sin_addr));
        if(retrecv==-1) {
            perror("retrecv");
            return EXIT_FAILURE;
        } else {
            printf("recu %s : \n",m);
        }
        if (m == NULL) {
            continue; // à traiter
        }
        char actualsender[50];
        char tmp[350];
        strcpy(tmp, m);
		strcat(tmp,"\n");
		if(put_entry(tmp)==-1) {
			fprintf(stderr,"failed to put entry\n");
		}
        char* token = strtok(tmp, ";");
        strcpy(actualsender, token);
        //On ajoute au pool l'écrivain si il n'y ai pas déjà
        printf("test isinpool\n");
        if (!isInPool(ap, actualsender)) {
            //Si l'écrivain n'est pas dans le pool on l'ajoute dedans
            printf("on ajoute %s au pool\n", actualsender);
            putInPool(ap, actualsender, (struct sockaddr*) &addr_recept);
            //!!!TODO ENVOYER INFO (30 derniers messages)
            //Récupération de l'historique pour l'envoyer a l'écrivain
            char* histobrief = get_history_brief();
            size_t histosize = strlen(histobrief);
            if(histobrief==NULL){
                fprintf(stderr, "Failed to retrieve history\n");
            }
            printf("%zu", histosize);
            printf("try to get addr...\n");
            struct sockaddr *sockrespond;
            if(( sockrespond=get_addr(ap,actualsender))==NULL){
                printf("failed to get addr..\n");
            }
            printf("addresse = %s\n", inet_ntoa(((struct sockaddr_in *) sockrespond) -> sin_addr));
            sockrespond->sa_family = AF_INET;
            //sockrespond->sa_port = 
            socklen_t socksize = sizeof(struct sockaddr_in);
            printf("size = %zu\n",(size_t)socksize);
            int r;
            if((r = (int)sendto(s,histobrief,histosize,0,sockrespond,socksize))==-1){
                fprintf(stderr, "Failed to send history brief\n");
                perror("sendto");
                exit(EXIT_FAILURE);
            }
            printf("sendto passé with %d.\n", r);
    	}
        int sizepool = poolSize(ap);
        //ICI boucle qui énumère toute les adresses du pool.
        printf("Taille du pool (%d)\n",sizepool);
        for (int i = 0; i < sizepool; ++i){
		    printf("envoi aux clients (%d)\n",sizepool);
		    Handler_Struct* hs = malloc(sizeof(struct Handler_Struct*));
		    hs->addr = malloc(sizeof(struct sockaddr_in));
		    if(hs==NULL){
		        perror("hs");
		        exit(EXIT_FAILURE);
		    }
		    struct sockaddr *ad = get_addr_at(ap, i);
		    if(ad==NULL){
		        exit(EXIT_FAILURE);
		    }
		    printf("getaddrat ok\n");
		    memcpy(hs->addr,ad,sizeof(struct sockaddr_in));
		    printf("memcpY ok\n");
		    strcpy(hs -> msg, m);

		    hs->sockfd = s;
		    printf("strcpy ok\n");
		    //sizeof(struct sockaddr_in)
		    pthread_attr_t attr;
		    int errnum;
			if ((errnum = pthread_attr_init(&attr)) != 0) {
				fprintf(stderr, "pthread_attr_init: %s\n", strerror(errnum));
				exit(EXIT_FAILURE);
			}
				
			if ((errnum = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED)) != 0) {
				fprintf(stderr, "pthread_attr_setdetachstate: %s\n", strerror(errnum));
				exit(EXIT_FAILURE);
			}
			  
		    pthread_t thread;
		    //pthread_detach(thread);
		    printf("detach state ok\n");
		    if(pthread_create(&thread, &attr, &handler_pthread , hs) == -1) {
		        perror("pthread_create");
		        return EXIT_FAILURE;
		    }
        } // for
    }

    if (close(s) == -1) {
        perror("close");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}


