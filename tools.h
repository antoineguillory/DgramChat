#define _GNU_SOURCE
#ifndef TOOLS_PROJETRESEAU_H
#define TOOLS_PROJETRESEAU_H

/*
 * Concatène deux buffers
 */
char* concat(char* str1, char* str2);

/*
 * Recupere le contenu de la ligne n° line number sur le fichier FD
 */
char* get_line(unsigned int linenumber);

/*
 * Compte les lignes sur le fichier filename
 */
unsigned int countlines(char *filename);

#endif

