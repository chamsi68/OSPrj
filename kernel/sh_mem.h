#ifndef __SHMEM__
#define __SHMEM__

#include "hash.h"
#include "memm.h"

extern struct page_frame user_mem_map[UNPAGES];

void *shm_create(const char *key);

void *shm_acquire(const char *key);

void shm_release(const char *key);

#endif
