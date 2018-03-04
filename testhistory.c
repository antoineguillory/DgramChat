#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "message.h"
#include "history.h"

int main(void){
    printf("(Test écriture fichier log)\n");
    Message* msg = malloc(sizeof(Message*)); 
    msg->sender = "Antoine";
    msg->message= "Oui bonjour ceci est un test";
    if(put_entry(msg)==-1){
        printf("Echec du test\n");
    } else {
        printf("Réussite du test\n");
    }

}
