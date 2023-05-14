//
// Created by graini on 4/4/22.
//
#include "semaphores.h"

struct semaphore semaphHead;
link available_semaph_list = LIST_HEAD_INIT(available_semaph_list);
link used_semaph_list = LIST_HEAD_INIT(used_semaph_list);
link semaphBlocked_head_list = LIST_HEAD_INIT(semaphBlocked_head_list);


void liberate_semaph(int sem) {
    struct process *ps;
    queue_for_each(ps, &semaphBlocked_head_list, struct process, mainlink) {
        if (ps->idSem == sem) {
            ps = queue_out(&semaphBlocked_head_list, struct process, mainlink);
            queue_add(ps, &ready_head_list, struct process, mainlink, priority);
            ps->idWake = 1;
        }
    }
}


void liberate_one_semaph(int sem, int awake) {
    struct process *ps;
    queue_for_each(ps, &semaphBlocked_head_list, struct process, mainlink) {
        if (ps->idSem == sem) {
            ps = queue_out(&semaphBlocked_head_list, struct process, mainlink);
            queue_add(ps, &ready_head_list, struct process, mainlink, priority);
            ps->status = 1;
            ps->idWake = awake;
            scheduler();
            return;
        }
    }
}


void wake_reset(int sem) {
    struct process *ps;
    queue_for_each(ps, &semaphBlocked_head_list, struct process, mainlink) {
        if (ps->idSem == sem) {
            ps = queue_out(&semaphBlocked_head_list, struct process, mainlink);
            ps->idWake = 2;
            queue_add(ps, &ready_head_list, struct process, mainlink, priority);
        }
    }
}

// Initialiser 16 Sémaphores
void sInit() {
    int index;
    struct semaphore *new_semaph;
    INIT_LIST_HEAD(&semaphHead.next);
    semaphHead.id = 0;
    semaphHead.value = 0;
    for (index = 1; index <= NUMBER_SEM; index ++) {
        new_semaph = (struct semaphore *)mem_alloc(sizeof(struct semaphore));
        new_semaph->id = index;
        new_semaph->value = 0;
        queue_add(new_semaph, &available_semaph_list, struct semaphore, next, id);
    }
}

// Créer un Sémaphore
int screate(short int count) {
    struct semaphore *new_semaph;
    if (count < 0)
        return -1;
    new_semaph = queue_out(&available_semaph_list, struct semaphore, next);
    if (!new_semaph)
        return -1;
    new_semaph->value = count & 0xffff;
    queue_add(new_semaph, &used_semaph_list, struct semaphore, next, id);
    return new_semaph->id;
}

// Supprimer un sémaphore
int sdelete(int sem) {
    struct semaphore *new_semaph;
    if (sem < 0)
        return -1;
    liberate_semaph(sem);
    queue_for_each(new_semaph, &used_semaph_list, struct semaphore, next) {
        if (new_semaph->id == sem) {
            queue_del(new_semaph, next);
            break;
        }
    }
    return 0;
}


int try_wait(int sem) {
    struct semaphore *semaphore;
    if (sem < 0 || sem > MAXSEMAPH)
        return -1;
    semaphore = getSemByIdAndDelete(sem);
    if (!semaphore) //Semaphore already deleted
        return -1;
    if (semaphore->value - 1 <= INT16_MIN)
        return -2;
    if (semaphore->value - 1 <= 0)
        return -3;
    semaphore->value -= 1;
    return 0;
}


int signal(int sem) {
    struct semaphore *semaphore;
    if (sem < 0 || sem > MAXSEMAPH)
        return -1;
    semaphore = getSemByIdWithoutDelete(sem);
    if (!semaphore) //Semaphore already deleted
        return -1;
    if (semaphore->value + 1 >= INT16_MAX)
        return -2;
    semaphore->value += 1;
    if (scount(sem) >= 0 ) // Positive maybe ??
        liberate_one_semaph(sem, 3);
    return 0;
}

int wait(int sem) {
    struct semaphore *semaphore;
    if (sem < 0 || sem > MAXSEMAPH)
        return -1;
    semaphore = getSemByIdWithoutDelete(sem);
    if (!semaphore) //Semaphore already deleted
        return -1;
    if (semaphore->value - 1 <= INT16_MIN )
        return -2;
    queue_add(running_proc, &semaphBlocked_head_list, struct process, mainlink, priority);
    running_proc->idSem = sem;
    running_proc->idWake = 0;
    while (!running_proc-> idWake) {
        running_proc->status = 3;
        scheduler();
    }
    if (running_proc -> idWake == 1) {
        // SDELETE
        running_proc = 0;
        return -3;
    }
    if (running_proc -> idWake == 2) {
        running_proc = 0;
        return -4;
    }
    // idWake 3 pour Signal
    return 0;
}


int scount(int sem) {
    int value = getSemByIdWithoutDelete(sem)->value;
    return value & 0xffff;
}


int sreset(int sem, int count) {
    struct semaphore *new_semaph;
    if (count < 0 || sem < 0 || sem > MAXSEMAPH)
        return -1;
    new_semaph  = getSemByIdAndDelete(sem);
    wake_reset(sem);
    queue_add(new_semaph, &available_semaph_list, struct semaphore, next, id);
    new_semaph->value = count & 0xffff;
    return 0;
}


struct semaphore *getSemByIdAndDelete(int sem) {
    struct semaphore *new_semaph  = queue_out(&available_semaph_list, struct semaphore, next);
    if (sem < 0 || sem > MAXSEMAPH)
        return new_semaph;
    queue_for_each(new_semaph, &used_semaph_list, struct semaphore, next) {
        if (new_semaph->id == sem) {
            queue_del(new_semaph, next);
            return new_semaph;
        }
    }
    queue_for_each(new_semaph, &available_semaph_list, struct semaphore, next) {
        if (new_semaph->id == sem) {
            queue_del(new_semaph, next);
            return new_semaph;
        }
    }
    return new_semaph;
}


struct semaphore *getSemByIdWithoutDelete(int sem) {
    struct semaphore *new_semaph = 0;
    if (sem < 0 || sem > MAXSEMAPH)
        return new_semaph;
    queue_for_each(new_semaph, &used_semaph_list, struct semaphore, next)
        if (new_semaph->id == sem)
            return new_semaph;
    queue_for_each(new_semaph, &available_semaph_list, struct semaphore, next)
        if (new_semaph->id == sem)
            return new_semaph;
    return new_semaph;
}


void displayUsed(void) {
    struct semaphore *new_semaph;
    printf("------ Semaphore Arrays ------\n%10s %10s %10s\n", "semid", "owner", "capacity");
    queue_for_each(new_semaph, &used_semaph_list, struct semaphore, next)
        printf("%10d %10s %10d\n", new_semaph->id, "user", new_semaph->value);
}


int signaln(int sem, short int count) {
    int value, i;
    for (i = 0; i <= count; i++) {
        value = signal(sem);
        if (value != 0)
            return value;
    }
    return 0;
}


void displayAvailable(void) {
    struct semaphore *new_semaph;
    printf("------ Semaphore Arrays ------\n%10s %10s %10s\n", "semid", "owner", "capacity");
    queue_for_each(new_semaph, &available_semaph_list, struct semaphore, next)
        printf("%10d %10s %10d\n", new_semaph->id, "user", new_semaph->value);
}
