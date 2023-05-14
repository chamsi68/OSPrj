#include "semaphores.h"

int pidc = -1, exception = 0;
struct process pidle, *running_proc = 0, *previous_running_proc = 0;
link ready_head_list = LIST_HEAD_INIT(ready_head_list);
link suspended_head_list = LIST_HEAD_INIT(suspended_head_list);
link dead_head_list = LIST_HEAD_INIT(dead_head_list);
link dead_pids = LIST_HEAD_INIT(dead_pids);
link msgs = LIST_HEAD_INIT(msgs);


void scheduler(void)
{
    unsigned clock = current_clock();
    struct process *ps;
    for (; !queue_empty(&suspended_head_list); )
    {
        if (-queue_entry(suspended_head_list.prev, struct process, mainlink)->waketime > clock)
            break;
        ps = queue_out(&suspended_head_list, struct process, mainlink);
        ps->status = READY;
        if (ps != running_proc)
            queue_add(ps, &ready_head_list, struct process, mainlink, priority);
    }
    previous_running_proc = running_proc;
    if (previous_running_proc && previous_running_proc->status < 3)
    {
        if (previous_running_proc->status < 2)
            previous_running_proc->status = READY;
        queue_add(previous_running_proc, &ready_head_list, struct process, mainlink, priority);
    }
    running_proc = queue_out(&ready_head_list, struct process, mainlink);
    running_proc->status = RUNNING;
    ctx_sw();

}

#if (!VM)
int start(int (*pt_func)(void*), unsigned long ssize, int prio, const char *name, void *arg)
{
    int pid;
    unsigned esp;
    struct process *process;
    struct message *msg;
    if (pidc < 0 || (pidc >= NBPROC && queue_empty(&dead_pids)) || ssize > 8180)
        return -1;
    process = (struct process *)mem_alloc(sizeof(struct process));
    process->parent = running_proc;
    INIT_LIST_HEAD(&process->heap_page_list);
    INIT_LIST_HEAD(&process->childs_list);
    if (running_proc)
        queue_add(process, &running_proc->childs_list, struct process, childlink, priority);
    process->status = READY;
    strcpy(process->name, name);
    process->sustack = first_stack;
    esp = (unsigned)process->sustack + 0x4000;
    process->registers[8] = esp;
    process->stack = (int *)mem_alloc(ssize + 3 * sizeof(int));
    esp = (unsigned)process->stack + ssize;
    process->nbpages = 4 + ((ssize + 3 * sizeof(int)) / 0x1000 + 1) * 0x1000;
    process->sudo = 1;
    process->registers[7] = esp;
    process->ssize = ssize;
    process->priority = prio % MAXPRIO;
    process->mqd = -1;
    if (!queue_empty(&dead_pids))
    {
        pid = (msg = queue_out(&dead_pids, struct message, mainlink))->value;
        mem_free(msg, sizeof(struct message));
    }
    else
        pid = ++pidc;
    process->pid = pid;
    process->pgdir = pgdir;
    *((unsigned *)esp) = (unsigned)pt_func;
    ((unsigned *)esp)[1] = (unsigned)sys_exit;
    ((unsigned *)esp)[2] = (unsigned)arg;
    queue_add(process, &ready_head_list, struct process, mainlink, priority);
    scheduler();
    return pid;
}
#endif

void wait_clock(unsigned clock)
{
    previous_running_proc = running_proc;
    previous_running_proc->status = SUSPENDED;
    previous_running_proc->waketime = -clock;
    queue_add(previous_running_proc, &suspended_head_list, struct process, mainlink, waketime);
    scheduler();
}


void exit(int retval)
{
    int mqd, prio;
    struct message *msg;
    prio = running_proc->priority;
    running_proc->priority = MAXPRIO;
    if (running_proc->parent)
    {
        mqd = running_proc->parent->mqd;
        if (mqd == -1)
            for (;;)
            {
                mqd = pcreate(NBPROC);
                if (mqd == -1)
                    wait_clock(current_clock() + 1);
                else
                {
                    running_proc->parent->mqd = mqd;
                    break;
                }
            }
        msg = (struct message *)mem_alloc(sizeof(struct message));
        msg->value = (retval & 0xffe00000) >> 10 | (running_proc->pid & 0x7ff);
        queue_add(msg, &msgs, struct message, mainlink, priority);
        psend(mqd, retval << 11 | (running_proc->pid & 0x7ff));
    }
    running_proc->priority = prio;
    running_proc->status = DEAD;
    queue_add(running_proc, &dead_head_list, struct process, mainlink, priority);
    if (!strcmp(running_proc->name, "shell"))
        pshell = 0;
    scheduler();
    for (;;);
}


int kill(int pid)
{
    unsigned esp, counter;
    link *index, *list = 0;
    struct process *ps = 0;
    struct messageQueue *file;
    if (!pid)
        return -1;
    if (running_proc->pid == pid)
        ps = running_proc;
    else
    {
        list = &ready_head_list;
        for (index = list->next; index != list; index = index->next)
            if ((ps = queue_entry(index, struct process, mainlink))->pid == pid)
            {
                list = 0;
                goto label;
            }
        list = &suspended_head_list;
        for (index = list->next; index != list; index = index->next)
            if ((ps = queue_entry(index, struct process, mainlink))->pid == pid)
            {
                ps = queue_out(index->next, struct process, mainlink);
                goto label;
            }
        list = &semaphBlocked_head_list;
        for (index = list->next; index != list; index = index->next)
            if ((ps = queue_entry(index, struct process, mainlink))->pid == pid)
            {
                ps = queue_out(index->next, struct process, mainlink);
                goto label;
            }
        for (counter = 0; counter < NBQUEUE; counter++)
            if ((file = tableFiles[counter]))
            {
                list = &file->processusBloqueSurFileVide;
                for (index = list->next; index != list; index = index->next)
                    if ((ps = queue_entry(index, struct process, mainlink))->pid == pid)
                    {
                        ps = queue_out(index->next, struct process, mainlink);
                        goto label;
                    }
                list = &file->processusBloqueSurFilePleine;
                for (index = list->next; index != list; index = index->next)
                    if ((ps = queue_entry(index, struct process, mainlink))->pid == pid)
                    {
                        ps = queue_out(index->next, struct process, mainlink);
                        goto label;
                    }
            }
        ps = 0;
        list = 0;
    }
label:
    if (ps && ps->status != ZOMBIE)
    {
        ps->status = ZOMBIE;
        if (ps == running_proc && !exception)
            exit(0);
#if (VM)
        ps->sudo = 1;
        esp = (unsigned)ps->sustack + 0x4000 - 4;
        ps->registers[8] = esp;
        if (exception)
        {
            for (index = ps->bin_page_list.next, counter = 0; index != &ps->bin_page_list; index = index->next, counter++);
            tss.cr3 = (unsigned)ps->pgdir;
            tss.eip = 0x40000000 + counter * 0x1000 - 14;
            tss.esp = (unsigned)ps->stack + ps->ssize + 44 + 12;
            return 0;
        }
#else
        esp = (unsigned)ps->stack + ps->ssize + 8;
        ps->registers[7] = esp;
#endif
        *((unsigned *)esp) = (unsigned)sys_exit - 5;
        if (list)
            queue_add(ps, &ready_head_list, struct process, mainlink, priority);
        scheduler();
        return 0;
    }
    return -1;
}


int getprio(int pid)
{
    unsigned counter;
    link *index, *list;
    struct process *ps = 0;
    struct messageQueue *file;
    if (running_proc->pid == pid)
        ps = running_proc;
    else
    {
        list = &ready_head_list;
        for (index = list->next; index != list; index = index->next)
            if ((ps = queue_entry(index, struct process, mainlink))->pid == pid)
                goto label;
        list = &suspended_head_list;
        for (index = list->next; index != list; index = index->next)
            if ((ps = queue_entry(index, struct process, mainlink))->pid == pid)
                goto label;
        list = &semaphBlocked_head_list;
        for (index = list->next; index != list; index = index->next)
            if ((ps = queue_entry(index, struct process, mainlink))->pid == pid)
                goto label;
        for (counter = 0; counter < NBQUEUE; counter++)
            if ((file = tableFiles[counter]))
            {
                list = &file->processusBloqueSurFileVide;
                for (index = list->next; index != list; index = index->next)
                    if ((ps = queue_entry(index, struct process, mainlink))->pid == pid)
                        goto label;
                list = &file->processusBloqueSurFilePleine;
                for (index = list->next; index != &file->processusBloqueSurFilePleine; index = index->next)
                    if ((ps = queue_entry(index, struct process, mainlink))->pid == pid)
                        goto label;
            }
        ps = 0;
    }
label:
    if (ps)
        return ps->priority;
    return -1;
}


int chprio(int pid, int newprio)
{
    unsigned counter;
    int oldprio;
    link *index, *list = 0;
    struct process *ps = 0;
    struct messageQueue *file;
    if (!pid || !newprio)
        return -1;
    if (running_proc->pid == pid)
        ps = running_proc;
    else
    {
        list = &ready_head_list;
        for (index = list->next; index != list; index = index->next)
            if (queue_entry(index, struct process, mainlink)->pid == pid)
            {
                ps = queue_out(index->next, struct process, mainlink);
                goto label;
            }
        list = &suspended_head_list;
        for (index = list->next; index != list; index = index->next)
            if (queue_entry(index, struct process, mainlink)->pid == pid)
            {
                ps = queue_out(index->next, struct process, mainlink);
                goto label;
            }
        list = &semaphBlocked_head_list;
        for (index = list->next; index != list; index = index->next)
            if (queue_entry(index, struct process, mainlink)->pid == pid)
        {
            ps = queue_out(index->next, struct process, mainlink);
            goto label;
        }
        for (counter = 0; counter < NBQUEUE; counter++)
            if ((file = tableFiles[counter]))
            {
                list = &file->processusBloqueSurFileVide;
                for (index = list->next; index != list; index = index->next)
                    if (queue_entry(index, struct process, mainlink)->pid == pid)
                    {
                        ps = queue_out(index->next, struct process, mainlink);
                        goto label;
                    }
                list = &file->processusBloqueSurFilePleine;
                for (index = list->next; index != list; index = index->next)
                    if (queue_entry(index, struct process, mainlink)->pid == pid)
                    {
                        ps = queue_out(index->next, struct process, mainlink);
                        goto label;
                    }
            }
        ps = 0;
        list = 0;
    }
label:
    if (ps && ps->status != ZOMBIE)
    {
        oldprio = ps->priority;
        ps->priority = newprio;
        if (list)
            queue_add(ps, list, struct process, mainlink, priority);
        scheduler();
        return oldprio;
    }
    else if (list)
        queue_add(ps, list, struct process, mainlink, priority);
    return -1;
}


int waitpid(int pid, int *retvalp)
{
    int temp, mqd;
    link *index, *head, messages = LIST_HEAD_INIT(messages);
    struct message *msg;
    struct process *ps;
    if (queue_empty(&running_proc->childs_list))
        return -1;
    mqd = running_proc->mqd;
    if (mqd == -1)
        for (;;)
        {
            mqd = pcreate(NBPROC);
            if (mqd == -1)
                wait_clock(current_clock() + 1);
            else
            {
                running_proc->mqd = mqd;
                break;
            }
        }
    if (pid < 0)
    {
        preceive(running_proc->mqd, &temp);
        pid = temp & 0x7ff;
    }
    else
    {
        for (;;)
        {
            preceive(running_proc->mqd, &temp);
            if ((temp & 0x7ff) != pid)
            {   msg = (struct message *)mem_alloc(sizeof(struct message));
                msg->value = temp;
                queue_add(msg, &messages, struct message, mainlink, priority);
            }
            else
                break;
        }
        while (!queue_empty(&messages))
        {
            psend(running_proc->mqd, (msg = queue_out(&messages, struct message, mainlink))->value);
            mem_free(msg, sizeof(struct message));
        }
    }
    for (index = msgs.next; index != &msgs; index = index->next)
        if ((queue_entry(index, struct message, mainlink)->value & 0x7ff) == pid)
        {
            msg = queue_out(index->next, struct message, mainlink);
            break;
        }
    if (retvalp)
        *retvalp = (msg->value & 0xfffff800) << 10 | (temp >> 11 & 0x1fffff);
    mem_free(msg, sizeof(struct message));
    for (index = dead_head_list.next; index != &dead_head_list; index = index->next)
        if (queue_entry(index, struct process, mainlink)->pid == pid)
        {
            ps = queue_out(index->next, struct process, mainlink);
            break;
        }
    if (ps->parent)
    {
        head = &ps->parent->childs_list;
        for (index = head->next; index != head; index = index->next)
            if (index == &ps->childlink)
            {
                queue_out(index->next, struct process, childlink);
                break;
            }
    }
    msg = (struct message *)mem_alloc(sizeof(struct message));
    msg->value = ps->pid;
    queue_add(msg, &dead_pids, struct message, mainlink, priority);
#if (VM)
    while (!queue_empty(&ps->stack_page_list))
        page_free(queue_out(&ps->stack_page_list, struct page_frame, mainlink));
    while (!queue_empty(&ps->heap_page_list))
        page_free(queue_out(&ps->heap_page_list, struct page_frame, mainlink));
    while (!queue_empty(&ps->bin_page_list))
        page_free(queue_out(&ps->bin_page_list, struct page_frame, mainlink));
    page_free(ps->bin_pg_tab);
    page_free(ps->heap_pg_tab);
    page_free(ps->stack_pg_tab);
    page_free(ps->pg_dir);
    mem_free(ps->sustack, 0x4000);
#else
    while (!queue_empty(&ps->heap_page_list))
    {
        msg = queue_out(&ps->heap_page_list, struct message, mainlink);
        mem_free((void *)msg->value, 0x1000);
        mem_free(msg, sizeof(struct message));
    }
    mem_free(ps->stack, ps->ssize + 3 * sizeof(int));
#endif
    mem_free(ps, sizeof(struct process));
    pcount(running_proc->mqd, &temp);
    if (!temp)
    {
        pdelete(running_proc->mqd);
        running_proc->mqd = -1;
    }
    return pid;
}


int getpid(void)
{
    return running_proc->pid;
}


void init(void)
{
    unsigned esp;
    INIT_LIST_HEAD(&pidle.childs_list);
    strcpy(pidle.name, "idle");
    pidle.stack = (int *)mem_alloc(0x4000);
    esp = (unsigned)pidle.stack + 16384 - 12;
    pidle.nbpages = 1;
    pidle.sudo = 1;
    pidle.mqd = -1;
    pidle.pid = ++pidc;
    pidle.pgdir = pgdir;
    pidle.ssize = 16384 - 12;
    pidle.priority = 0;
    *((unsigned *)esp) = (unsigned)idle;
    ((unsigned *)esp)[1] = 0;
    ((unsigned *)esp)[2] = 0;
    running_proc = &pidle;

}


int idle(void *argv)
{
    (void)argv;
#if (!VM)
    start(test14_sem, 8180, 128, "test14", 0);
#else
    start("shell", 8180, 128, 0);
#endif
    for (;;)
    {
        cli();
        sti();
        cli();
    }
}


void process_info(void)
{
    unsigned counter = 0;
    struct process *process;
    struct messageQueue *file;
    printf("%10s %10s %10s %10s %10s %10s\n", "USER", "PID", "PRIO", "%MEM", "STAT", "COMMAND");
    printf("%10s %10d %10d %10d %10s %10s\n", "user", running_proc->pid, running_proc->priority, running_proc->nbpages / 7864, "running", running_proc->name);
    queue_for_each(process, &ready_head_list, struct process, mainlink)
    {
        if (!process->pid)
            printf("%10s %10d %10d %10d %10s %10s\n", "system", process->pid, process->priority, process->nbpages / 7864, "running", process->name);
        else if (process->status == ZOMBIE)
            printf("%10s %10d %10d %10d %10s %10s\n", "user", process->pid, process->priority, process->nbpages / 7864, "zombie", process->name);
        else
            printf("%10s %10d %10d %10d %10s %10s\n", "user", process->pid, process->priority, process->nbpages / 7864, "running", process->name);
    }
    queue_for_each(process, &suspended_head_list, struct process, mainlink)
        printf("%10s %10d %10d %10d %10s %10s\n", "user", process->pid, process->priority, process->nbpages / 7864, "sleeping", process->name);
    for (counter = 0; counter < NBQUEUE; counter++)
        if ((file = tableFiles[counter]))
        {
            queue_for_each(process, &file->processusBloqueSurFileVide, struct process, mainlink)
                printf("%10s %10d %10d %10d %10s %10s\n", "user", process->pid, process->priority, process->nbpages / 7864, "suspended", process->name);
            queue_for_each(process, &file->processusBloqueSurFilePleine, struct process, mainlink)
                printf("%10s %10d %10d %10d %10s %10s\n", "user", process->pid, process->priority, process->nbpages / 7864, "suspended", process->name);
        }
}


void sys_info(int id)
{
    switch (id)
    {
    case 0:
        process_info();
        break;
    case 1:
        msgqueue_info();
        shm_info();
        displayUsed();
        break;
    case 2:
        msgqueue_info();
        break;
    case 3:
        shm_info();
        break;
    case 4:
        displayUsed();
    }
}

