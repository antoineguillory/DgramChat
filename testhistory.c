#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "message.h"
#include "history.h"
#include "tools.h"

int main(void){
    printf("(Test écriture fichier log)\n");
    //Message* msg = malloc(sizeof(Message*)); 
    //strcpy(msg->sender,"Antoine");
    //strcpy(msg->message,"Oui bonjour ceci est un test");
    if(put_entry("Antoine; Oui bonjour ceci est un test")==-1){
        printf("Echec du test\n");
    } else {
        printf("Réussite du test\n");
    }
    printf("(Test récupération de la ligne numéro 1)\n");
    int fd;
    if((fd=open(HISTORY_PATH, O_RDONLY))<0){
        fprintf(stderr, "Failed to open History\n");
        return -1;
    }
    char* buf = get_line(fd,5);
    if(buf==NULL){
        printf("Echec du test de lecture\n");
    }
    printf("%s\n",buf);

    printf("%ud\n", countlines(HISTORY_PATH));
    //printf("%s\n", get_history_brief());
}
