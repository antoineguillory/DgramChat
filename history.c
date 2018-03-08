#define _GNU_SOURCE
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "message.h"
#include "history.h"
#include "tools.h"


int put_entry(char  *msg) {
    int fd;
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

    bufsender = concat("(",bufsender);
    bufsender = concat(bufsender,"): ");
    finalbuf = concat(bufsender,bufmsg);
    finalbuf = concat(finalbuf,CARRIAGE_RETURN);
    if(write(fd,finalbuf,strlen(finalbuf))==-1){
        fprintf(stderr, "Failed to write in history\n");
        return -1;
    }
    if(close(fd)==-1){
        fprintf(stderr, "Failed to close the file descriptor\n");
        return -1;
    }
    //Liberation des ressources
    free(bufsender);
    free(bufmsg);
    free(finalbuf);
    return 0;
}

char* get_history_brief(void) {
    int fd;
    char* buf = malloc(2048);
    if((fd=open(HISTORY_PATH, O_RDONLY))<0){
        fprintf(stderr, "Failed to open History\n");
        return NULL;
    }
    unsigned int count=0;
    if((count=countlines(HISTORY_PATH))==0){
        return NULL;
    }
    for(unsigned int i=count-HISTORY_LINE_SAMPLE; i!=count; ++i){
        realloc(buf, strlen(buf)+strlen(get_line(fd,i))+1);
        concat(buf,get_line(fd,i));
    }
    return buf;
}
