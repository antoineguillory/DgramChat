#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <pthread.h>
#include "message.h"

#define CHOSEN_PORT "45532"


struct argument{
    int socket;
    struct sockaddr *addr;
};


void *run (void *arg) {
    struct argument *info_th = (struct argument *)arg;
    while(1) {
        Message mess;
        socklen_t size;
        recvfrom(info_th -> socket, &mess, sizeof(mess), 0, info_th -> addr, &size);
        printf("%s :> %s", mess . sender, mess . message);
    }
}




int main() {
    //Collecte du nom d'utilisateur
    uid_t id = geteuid();
    struct passwd *info = getpwuid(id);
    char sendername[50];
    strncpy(sendername, info -> pw_name, 50);

    //Initialisation de la socket du client
    int s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s == -1) {
        perror("primary socket");
        return EXIT_FAILURE;
    }

    //Initialisation adresse serveur
    struct addrinfo *hints = NULL;
    memset(hints, 0, sizeof(struct addrinfo));
    hints -> ai_family = AF_INET;
    hints -> ai_socktype = SOCK_DGRAM;

    struct addrinfo *res;
    int r;
    if ((r = getaddrinfo(NULL, CHOSEN_PORT, hints, &res)) != 0) {
        perror("getaddrinfo");
        return EXIT_FAILURE;
    }
    if (res == NULL) {
        perror("getaddrinfo");
        return EXIT_FAILURE;
    }
    struct sockaddr *server_addr = res -> ai_addr;

    //Préparation du thread de réception de message;
    struct argument *arg = malloc(sizeof(*arg));
    arg -> socket = s;
    arg -> addr = server_addr;
    pthread_t th;
    pthread_attr_t attr_th;
    pthread_attr_init(&attr_th);
    pthread_create(&th, &attr_th, run, arg);
    while(1) {
        printf("%s :> ", sendername);
        char data[300];
        scanf("%s", data);
        Message *mess = malloc(sizeof(*mess));
        strncpy(mess -> message,data, 300);
        strncpy(mess -> sender, sendername, 50);
        sendto(s, mess, sizeof(Message), 0, server_addr, sizeof(struct sockaddr_in));
    }
    return 0;

}