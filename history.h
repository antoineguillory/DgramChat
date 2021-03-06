#define _GNU_SOURCE
#ifndef HISTORY_H
#define HISTORY_H
#include "message.h"

#define HISTORY_PATH "./history.log"
#define FORMAT_NAME_SUPPLEMENT 4 // [{sender}]: ([]: )
#define CARRIAGE_RETURN "\n"
#define HISTORY_LINE_SAMPLE 30

/*
 * Ajoute une entree dans l'historique
 * @returns -1 si erreur 0 sinon
 */
int put_entry(char *msg);

/*
 * Recupere les derniers messages
 */
char* get_history_brief(void);

#endif
