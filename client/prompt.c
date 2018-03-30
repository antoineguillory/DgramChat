//
// Created by zartant on 05/03/18.
//

#include "prompt.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void clear_a_table(char *t, int length) {
	for (int i = 0; i < length; i++) {
		t[i] = '1';
	}
}

void print_message (char *m, int taille) {
		char bufsender[50];
		char bufmsg[300];
		int rname = 1;
		int namesize = 0;
		int messsize = 0;
		for(int i = 0; i <= taille; i++){
			if(m[i] == ';') {
				if (rname == 1) {
					rname = 0;
					bufsender[namesize] = '\0';
				} else {
					bufmsg[messsize] = m[i];
					messsize++;
				}
			} else if(m[i] == '\n') {
				rname = 1;
				bufmsg[messsize] = '\0';
				printf("(%s): %s\n", bufsender, bufmsg);
				messsize = 0;
				namesize = 0;
				clear_a_table(bufsender, 50);
				clear_a_table(bufmsg, 300);
			} else if (m[i] == '\0' && messsize != 0) {
				bufmsg[messsize] = '\0';
				printf("(%s): %s\n", bufsender, bufmsg);
				break;
			} else {
				if (rname == 1) {
					bufsender[namesize] = m[i];
					namesize++;
				} else {
					bufmsg[messsize] = m[i];
					messsize++;
				}
			}
		} 
}
