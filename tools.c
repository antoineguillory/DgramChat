#define _GNU_SOURCE
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "tools.h"

char* concat(char* str1, char* str2){
    if(str1==NULL || str2==NULL){
        perror("null str");
        return NULL;
    }
    char *s = malloc(strlen(str1)+strlen(str2)+1);
    if(s==NULL){
        perror("malloc");
        return NULL;
    }
    strcpy(s,str1);
    strcat(s,str2);
    return s;
}

char* get_line(unsigned int linenumber){
    char* ptr = malloc(2048);
    char* buf = malloc(65536);
    size_t size = 0;
    FILE* fdf = fopen("history.log","r");
    if(fdf==NULL){
	perror("fdf");
        return NULL;
    }
    for(unsigned int i=0; i!=linenumber; ++i){
        ptr = NULL;
        if(getline(&ptr,&size,fdf)==-1){
	   perror("getline");
	   return NULL;
	}
        strcpy(buf,concat(buf,ptr));
    }
    fclose(fdf);
    return ptr;
}

unsigned int countlines(char *filename){
    FILE *in_file;
    char buffer[1025];
    char lastchar = '\n';
    size_t bytes;
    unsigned int lines = 0;
    if (NULL == (in_file = fopen(filename, "r"))) {
        perror(filename);
        return EXIT_FAILURE;
    }
    while ((bytes = fread(buffer, 1, sizeof(buffer) - 1, in_file))) {
        lastchar = buffer[bytes-1];
        for (char *c = buffer;(c = memchr(c,'\n',(unsigned long)(bytes)-(unsigned long)(c-buffer))); ++c) {
            ++lines;
        }
    }
    if (lastchar != '\n') {
        ++lines;
    }
    if (ferror(in_file)) {
        perror(filename);
        fclose(in_file);
        return EXIT_FAILURE;
    }
    fclose(in_file);
    return lines;
}
