#define _GNU_SOURCE
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <semaphore.h>

#include "message.h"
#include "history.h"
#include "tools.h"

int put_entry(char *msg) {
    int fd;
    //On ne veut pas que deux fichiers écrivent en meme temps
    sem_t semaphore;
    sem_init(&semaphore, 0, 1);
    sem_wait(&semaphore);
    if((fd=open(HISTORY_PATH, O_WRONLY))<0){
        fprintf(stderr, "Failed to open History\n");
        return -1;
    }
    char bufsender[50];
    char bufmsg[300];
    char finalbuf[351];
    char tmp[351];
    strcpy(tmp, msg);
    char* token = strtok(tmp, ";");
    strcpy(bufsender, token);
    token = strtok(NULL, ";");
    strcpy(bufmsg, token);
    strcpy(bufsender,concat("(",bufsender));
    strcpy(bufsender,concat(bufsender,"): "));
    strcpy(finalbuf,concat(bufsender,bufmsg));
    strcpy(finalbuf,concat(finalbuf,CARRIAGE_RETURN));
    if(write(fd,finalbuf,strlen(finalbuf))==-1){
        fprintf(stderr, "Failed to write in history\n");
        return -1;
    }
    if(close(fd)==-1){
        fprintf(stderr, "Failed to close the file descriptor\n");
        return -1;
    }
    sem_post(&semaphore);
    sem_destroy(&semaphore);
    return 0;
}

char* get_history_brief(void) {
    sem_t semhistory;
    sem_init(&semhistory, 0, 1);
    sem_wait(&semhistory);
    char buf[8192] = "";
    unsigned int count=0;
    if((count=countlines(HISTORY_PATH))==0){
        return NULL;
    }
    printf("countline ok. %d lignes\n",count);
    for(unsigned int i=count-HISTORY_LINE_SAMPLE; i!=count; ++i){
        if(realloc(buf, strlen(buf)+strlen(get_line(i))+1)==NULL){
	    perror("realloc");
            return NULL;
	}
        concat(buf,get_line(i));
    }
    printf("realloc+concat ok. \n");
    sem_post(&semhistory);
    sem_destroy(&semhistory);
    char* truebuf = malloc(strlen(buf));
    strncpy(truebuf,buf,strlen(buf));
    return truebuf;
}