//
// Created by graini on 4/3/22.
//

#ifndef PSYS_BASE_SEMAPHORES_H
#define PSYS_BASE_SEMAPHORES_H

#include "memm.h"

#define MAXSEMAPH 16

#define  NUMBER_SEM 16

extern link semaphBlocked_head_list;

struct semaphore {
    int id, value;
    link next;
};

void wake_reset(int sem);

void liberate_semaph(int sem);

void liberate_one_semaph(int sem, int awake);
//Initialize the 16 semaphores disponibles
void sInit(void);
//Retourne id d'un sémaphore de valeur count, si Count <=0 ou il y'a pas de semaphore disponible retourne -1;
int screate(short int count);
// Détruit un semaphore et libére tous les processus processus bloqués par ce dernier, retourne 0 au succés et -1 au echec,
int sdelete(int sem);
// Libére un semaphore et libére tous les processus processus bloqués par ce dernier, retourne 0 au succés et -1 au echec,
int sreset(int sem, int count);
// Operation V() du sémpahore.
int signal(int sem); // V()

int signaln(int sem, short count);

int try_wait(int sem); // P() non bloquant

int wait(int sem); // P() le wait bloquant

int wait_semaph(int sem);
// Donne la valeur d'un semaphore d'aprés la formule donné.
int scount(int sem);
// Cherche le semaphore identifié par Id, dans les sémaphores utilisé, puis les sémpahores libres, il le supprime de la liste, puis le retourne !
struct semaphore *getSemByIdAndDelete(int sem);
// Cherche le semaphore identifié par Id, dans les sémaphores utilisé, puis les sémpahores libres, puis le retourne !
struct semaphore *getSemByIdWithoutDelete(int sem);
//Affiche les sémpahores utilisé
void displayUsed(void);
//Affiche les sémpahores disponbiles
void displayAvailable(void);

#endif //PSYS_BASE_SEMAPHORES_H
