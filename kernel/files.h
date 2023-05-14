#include "process.h"

#define NBQUEUE 50

struct message {
    int value, priority;
    link mainlink;
};

struct messageQueue {
    link messages; //liste de messages
    link processusBloqueSurFilePleine; // liste de processus bloqués sur file pleine
    link processusBloqueSurFileVide; // liste de processus bloqués sur file vide
    int capacite;
    int nbMessages;
    bool filePleine;
    bool fileVide;
};

extern struct messageQueue *tableFiles[NBQUEUE];

void msgqueue_info(void);

int pcreate(int count);

int pdelete(int fid);

int psend(int fid, int message);

int preceive(int fid, int *message);

int preset(int fid);

int pcount(int fid, int *count);
