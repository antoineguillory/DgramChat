#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "tools.h"

char* concat(char* str1, char* str2){
    char *s = malloc(strlen(str1)+strlen(str2)+1);
    if(s==NULL){
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    strcpy(s,str1);
    strcat(s,str2);
    return s;
}

char* get_line(int fd){
    return "";    
}