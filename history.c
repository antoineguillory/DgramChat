#define _GNU_SOURCE
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <semaphore.h>
#include <stdlib.h>
#include <ctype.h>
#include "message.h"
#include "history.h"
#include "tools.h"

int put_entry(char *msg) {
    int fd;
    //On ne veut pas que deux fichiers écrivent en meme temps
    /*sem_t semaphore;
    sem_init(&semaphore, 0, 1);
    sem_wait(&semaphore);*/

    if((fd=open(HISTORY_PATH, O_WRONLY | O_APPEND))<0){
        fprintf(stderr, "Failed to open History\n");
        return -1;
    }
	size_t len = strlen(msg);
    if(write(fd,msg,len)==-1){
        fprintf(stderr, "Failed to write in history\n");
        return -1;
    }
    if(close(fd)==-1){
        fprintf(stderr, "Failed to close the file descriptor\n");
        return -1;
    }
    /*sem_post(&semaphore);
    sem_destroy(&semaphore);*/
    return 0;
}

char* get_history_brief(void) {
    sem_t semhistory;
    sem_init(&semhistory, 0, 1);
    sem_wait(&semhistory);
    char *cmd = "tail -n 30 history.log 2> /dev/null";
    char buf[8192] = "";
    FILE *fp;
    if ((fp = popen(cmd, "r")) == NULL) {
        printf("popen error\n");
        return NULL;
    }
    char currentchar;
    while ((currentchar=(char)fgetc(fp)) != EOF) {
        char tmp[2];
        tmp[0]= currentchar;
        tmp[1]='\0';
        strncat(buf,tmp,2);
    }
    if(pclose(fp)){
        printf("pclose error\n");
        return NULL;
    }
    size_t buflen = sizeof(buf);
    if(buflen==0){
        return NULL;
    }
    char* finalbuf = malloc(buflen);
    if(finalbuf==NULL){
        return NULL;
    }
    strncpy(finalbuf,buf,buflen);
    sem_post(&semhistory);
    sem_destroy(&semhistory);
    
    return finalbuf;
}