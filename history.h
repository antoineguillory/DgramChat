#ifndef HISTORY_H
#define HISTORY_H
#include "message.h"

#define HISTORY_PATH "./history.log"
#define FORMAT_NAME_SUPPLEMENT 4 // [{sender}]: ([]: )

/*
 * Ajoute une entree dans l'historique
 * @returns -1 si erreur 0 sinon
 */
int put_entry(Message* msg);

/*
 * Recupere les derniers messages
 */
char* get_history_brief(void);

#endif
