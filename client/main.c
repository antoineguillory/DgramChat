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
        char mess[350];
        socklen_t size;
        recvfrom(info_th -> socket, mess, sizeof(mess), 0, info_th -> addr, &size);
        printf("%s\n", mess);
    }
}




int main() {
    uid_t id = geteuid();
    struct passwd *info = malloc(sizeof(*info));
    info = getpwuid(id);
    char sendername[50];
    strcpy(sendername, info -> pw_name);

    //Initialisation de la socket du client
    int s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s == -1) {
        perror("primary socket");
        return EXIT_FAILURE;
    }
    //Initialisation adresse serveur
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    sleep(1);
    struct addrinfo *res;
    int r;
    if ((r = getaddrinfo(NULL, CHOSEN_PORT, &hints, &res)) != 0) {
        perror("getaddrinfo");
        return EXIT_FAILURE;
    }
    if (res == NULL) {
        perror("getaddrinfo");
        return EXIT_FAILURE;
    }

    freeaddrinfo(res);
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
        char data[300];
        scanf("%s", data);
        char mess[350];
        sprintf(mess, "%s;%s", sendername, data);
        sendto(s, mess, 350, 0, server_addr, sizeof(struct sockaddr_in));
    }
    return 0;

}