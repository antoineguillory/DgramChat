#ifndef HISTORY_H
#define HISTORY_H
#include "message.h"

/*
 * Ajoute une entrée dans l'historique
 */
int put_entry(Message msg);

/*
 * Recupère les derniers messages
 */
char* get_history_brief(void);

#endif