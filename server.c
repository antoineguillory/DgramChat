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
    if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        perror("socket");
        return EXIT_FAILURE;
    }


	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(50000);
	server.sin_addr.s_addr = INADDR_ANY;
    if (bind(s, (struct sockaddr*) /*res->ai_addr*/&server, sizeof(struct sockaddr)) == -1) {
        perror("bind");
        return EXIT_FAILURE;
    }
    addrpool *ap = init_pool();
    for(;;) {
        //On reçoit le message
        char m[350];
        //retselect = select(1, &input_set, NULL, NULL, &timeout);
        struct sockaddr_in addr_recept;
        socklen_t *csize = malloc(sizeof(&addr_recept));
        printf("j'attend un message...\n");
        int retrecv = (int)recvfrom(s, m, sizeof(m), 0, (struct sockaddr*)&addr_recept, csize);
		free(csize);
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
        char* token = strtok(tmp, ";");
        strcpy(actualsender, token);
        //On ajoute au pool l'écrivain si il n'y ai pas déjà
        printf("test isinpool\n");
        if (!isInPool(ap, actualsender)) {
            printf("on ajoute %s au pool\n", actualsender);
            putInPool(ap, actualsender, (struct sockaddr*) &addr_recept);
            printf("put in pool fonctionnel\n");
            //!!!TODO ENVOYER INFO (30 derniers messages)
            char histobrief[8192];
            strncpy(histobrief,get_history_brief(), strlen(get_history_brief()));
            if(histobrief==NULL){
                fprintf(stderr, "Failed to retrieve history\n");
            }

			printf("try to get addr...\n");
			struct sockaddr *sockrespond;
			if(( sockrespond=get_addr(ap,actualsender))==NULL){
				printf("failed to get addr..\n");
			}
			sockrespond->sa_family = AF_INET;
			socklen_t socksize = sizeof(*sockrespond);
			printf("size = %zu\n",socksize);
			
            if(sendto(s,histobrief,strlen(histobrief)+1,0,sockrespond,socksize)==-1){
                fprintf(stderr, "Failed to send history brief\n");
				perror("sendto");
				exit(EXIT_FAILURE);
            }
			printf("sendto passé.\n");
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


