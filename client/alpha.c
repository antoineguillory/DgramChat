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
#include <ifaddrs.h>
#include "message.h"
#include "prompt.h"

#define CHOSEN_PORT "50000"
#define NBADDR 1


struct argument{
    int socket;
    struct sockaddr_in *addr;
};


void *run (void *arg) {
    struct argument *info_th = (struct argument *)arg;
    while(1) {
        char mess[4096];
        mess[4095] = 0;
        struct sockaddr_in addr_from;
        socklen_t addr_len = sizeof(addr_from);
        int r = 0;
        r = (int)recvfrom(info_th -> socket, mess, sizeof(mess) - 1, 0, (struct sockaddr *)&addr_from, &addr_len);
        
        if (r < 0) {
			printf("Error msg\n");
		} else if (r == 0) {
			printf("Server Close\n");
			exit(EXIT_SUCCESS);
		} else {
			//printf("\nTaille : %d\n", r);
			mess[r] = '\0';
		}
        print_message(mess,r);
    }
}

int main() {
    /*uid_t id = geteuid();
    struct passwd *info = malloc(sizeof(*info));
    info = getpwuid(id);
    char sendername[50];
    strcpy(sendername, info -> pw_name);*/
    char sendername[50] = "Alpha";

    //Initialisation de la socket du client
    int s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s == -1) {
        perror("primary socket");
        return EXIT_FAILURE;
    }
    struct sockaddr *cli_addr = NULL;
    struct ifaddrs *fap;
    if (getifaddrs(&fap) == -1) {
		perror("getifaddrs");
		return EXIT_FAILURE;
	}
	int addnb = 0;
	for(struct ifaddrs *ifa = fap; ifa != NULL; ifa = ifa -> ifa_next) {
		if (ifa -> ifa_addr == NULL) continue;
		if (ifa -> ifa_addr -> sa_family == AF_INET) {
			if (addnb == NBADDR) {
				cli_addr = ifa -> ifa_addr;
			} else {
				printf("compute adresse..\n");
				addnb++;
			}
		}
	}
    if (cli_addr == NULL) {
		printf("Echec getifaddrs");
		return EXIT_FAILURE;
	}
    if (bind(s, (struct sockaddr *)cli_addr, sizeof(struct sockaddr_in)) < 0) {
		perror("bind :");
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

    struct sockaddr_in *server_addr = (struct sockaddr_in * )res -> ai_addr;
    //Préparation du thread de réception de message;
    struct argument *arg = malloc(sizeof(*arg));
    arg -> socket = s;
    arg -> addr = server_addr;
    pthread_t th;
    pthread_attr_t attr_th;
    pthread_attr_init(&attr_th);
    pthread_create(&th, &attr_th, run, arg);
    
    

    while(1) {
		char mess[350];
		char data[300];
		fgets(data , 50, stdin);
		 if ((strlen(data) > 0) && (data[strlen(data) - 1] == '\n')) {
				data[strlen(data) - 1] = '\0';
		}
		sprintf(mess, "%s;%s", sendername, data);
		/*for (int i = 0; i < strlen(data); i++) {
			printf("\b");
		}*/
		sendto(s, mess, 350, 0, (struct sockaddr * )server_addr, sizeof(struct sockaddr_in));
		
    }
    return 0;

}
