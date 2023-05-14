#include "files.h"

struct messageQueue *tableFiles[NBQUEUE];

void addMessageToFile(struct message *msg, struct messageQueue *file)
{
    queue_add(msg, &file->messages, struct message, mainlink, priority);
    file->nbMessages++;
    file->fileVide = false;
    if (file->nbMessages == file->capacite)
        file->filePleine = true;
}


void retrieveMessageFromFile(struct messageQueue *file, int *message)
{
    struct message *msg = queue_out(&(file->messages), struct message, mainlink);
    file->nbMessages--;
    file->filePleine = false;
    if (file->nbMessages == 0)
        file->fileVide = true;
    *message = !msg ? 0 : msg->value;
}


int pcreate(int count)
{
    int index = -1;
    struct messageQueue *file;
    while (++index < NBQUEUE && tableFiles[index]);
    if (index == NBQUEUE || count <= 0 || count > NBPROC)
        return -1;
    file = mem_alloc(sizeof(struct messageQueue));
    INIT_LIST_HEAD(&file->processusBloqueSurFilePleine);
    INIT_LIST_HEAD(&file->processusBloqueSurFileVide);
    INIT_LIST_HEAD(&file->messages);
    file->capacite = count;
    file->nbMessages = 0;
    file->fileVide = true;
    file->filePleine = false;
    tableFiles[index] = file;
    return index;
}


int pdelete(int fid)
{
    int prio = running_proc->priority;
    running_proc->priority = MAXPRIO;
    if (fid < 0 || fid >= NBQUEUE || !tableFiles[fid])
        return -1;
    preset(fid);
    running_proc->priority = prio;
    mem_free(tableFiles[fid], sizeof(struct messageQueue));
    tableFiles[fid] = 0;
    scheduler();
    return 0;
}


int psend(int fid, int message)
{
    struct process *ps, *psdump;
    struct messageQueue *file;
    struct message *msg;
    if (fid < 0 || fid >= NBQUEUE || !tableFiles[fid])
        return -1;

    file = tableFiles[fid];

    msg = mem_alloc(sizeof(struct message));
    msg->value = message;

    if (file->filePleine)
    {
        running_proc->status = BLOCKED_ON_FULL_QUEUE;
        running_proc->message = (int *)mem_alloc(sizeof(int));
        *running_proc->message = message;
        queue_add(running_proc, &file->processusBloqueSurFilePleine, struct process, mainlink, priority);
        scheduler();
        if (!(file = tableFiles[fid]) || running_proc->sere)
        {
            running_proc->sere = 0;
            return -1;
        }
    }
    else
    {
        ps = queue_out(&file->processusBloqueSurFileVide, struct process, mainlink);
        if (ps)
        {
            queue_add(ps, &ready_head_list, struct process, mainlink, priority);
            while (ps->status == ZOMBIE)
            {
                psdump = queue_out(&file->processusBloqueSurFileVide, struct process, mainlink);
                if (psdump)
                    ps = psdump;
                else
                    break;
                queue_add(ps, &ready_head_list, struct process, mainlink, priority);
            }
            if (ps->status != ZOMBIE)
            {
                ps->message = (int *)mem_alloc(sizeof(int));
                *ps->message = message;
            }
            ps->status = READY;
            scheduler();
        }
        else
            addMessageToFile(msg, file);
    }
    return 0;
}


int preceive(int fid, int *message)
{
    struct process *ps, *psdump;
    struct messageQueue *file;
    struct message *msg;
    if (fid < 0 || fid >= NBQUEUE || !tableFiles[fid])
        return -1;

    file = tableFiles[fid];

    if (file->fileVide)
    {
        running_proc->status = BLOCKED_ON_EMPTY_QUEUE;
        queue_add(running_proc, &file->processusBloqueSurFileVide, struct process, mainlink, priority);
        scheduler();
        if (!(file = tableFiles[fid]) || running_proc->sere)
        {
            *message = -1;
            running_proc->sere = 0;
            return -1;
        }
        *message = *running_proc->message;
        mem_free(running_proc->message, sizeof(int));
        running_proc->message = 0;
    }
    else
    {
        ps = queue_out(&(file->processusBloqueSurFilePleine), struct process, mainlink);
        if (ps)
        {
            queue_add(ps, &ready_head_list, struct process, mainlink, priority);
            while (ps->status == ZOMBIE)
            {
                psdump = queue_out(&file->processusBloqueSurFilePleine, struct process, mainlink);
                if (psdump)
                    ps = psdump;
                else
                    break;
                queue_add(ps, &ready_head_list, struct process, mainlink, priority);
            }
            retrieveMessageFromFile(file, message);
            if (ps->status != ZOMBIE)
            {
                msg = (struct message *)mem_alloc(sizeof(struct message));
                msg->value = *ps->message;
                mem_free(ps->message, sizeof(int));
                ps->message = 0;
                addMessageToFile(msg, file);
            }
            ps->status = READY;
            scheduler();
        }
        else
            retrieveMessageFromFile(file, message);
    }
    return 0;
}


int preset(int fid)
{
    struct process *ps;
    struct messageQueue *file;
    if (fid < 0 || fid >= NBQUEUE || !tableFiles[fid])
        return -1;

    file = tableFiles[fid];

    while (!queue_empty(&file->processusBloqueSurFileVide))
    {
        ps = queue_out(&file->processusBloqueSurFileVide, struct process, mainlink);
        ps->status = READY;
        ps->sere = 1;
        queue_add(ps, &ready_head_list, struct process, mainlink, priority);
    }
    while (!queue_empty(&file->processusBloqueSurFilePleine))
    {
        ps = queue_out(&file->processusBloqueSurFilePleine, struct process, mainlink);
        ps->status = READY;
        ps->sere = 1;
        queue_add(ps, &ready_head_list, struct process, mainlink, priority);
    }
    while (!queue_empty(&file->messages))
        mem_free(queue_out(&file->messages, struct message, mainlink), sizeof(struct message));
    file->nbMessages = 0;
    file->filePleine = false;
    file->fileVide = true;
    scheduler();
    return 0;
}


int pcount(int fid, int *count)
{
    int result = 0;
    link *index;
    struct messageQueue *file;
    if (fid < 0 || fid >= NBQUEUE || !tableFiles[fid])
        return -1;
    file = tableFiles[fid];
    if (!count)
        return 0;
    for (index = file->processusBloqueSurFilePleine.next; index != &file->processusBloqueSurFilePleine; index = index->next)
        result++;
    for (index = file->processusBloqueSurFileVide.next; index != &file->processusBloqueSurFileVide; index = index->next)
        result--;
    result += file->nbMessages;
    *count = result;
    return 0;
}


void msgqueue_info(void)
{
    int index = 0;
    struct process *process;
    struct messageQueue *file;
    printf("------ Message Queues ------\n%10s %10s %10s %10s\n", "msqid", "usage", "messages", "blockprocs");
    while (index++ < NBQUEUE)
        if ((file = tableFiles[index]))
        {
            printf("%10d %10d %10d %10s\n", index, (file->nbMessages * 100) / file->capacite, file->nbMessages, "----");
            queue_for_each(process, &file->processusBloqueSurFilePleine, struct process, mainlink)
                printf("%10s %10s %10s %10s\n", "", "", "", process->name);
            queue_for_each(process, &file->processusBloqueSurFileVide, struct process, mainlink)
                printf("%10s %10s %10s %10s\n", "", "", "", process->name);
        }
}
