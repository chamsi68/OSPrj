#include "sh_mem.h"

unsigned shmcreation = 0;
hash_t hashmap = { 0, 0, 0, 0, 0, 0 };


void *shm_create(const char *key)
{
    void *result;
#if (VM)
    result = page_alloc();
#else
    result = mem_alloc(0x1000);
#endif
    hash_set(&hashmap, (void *)key, (void *)result);
    shmcreation++;
    result = shm_acquire(key);
    shmcreation--;
    return result;
}


void *shm_acquire(const char *key)
{
    int index;
    unsigned *pg_dir, *pg_tab;
    void *result;
    struct message *msg;
    struct page_frame *rpage;
#if (VM)
    (void)msg;
    index = -1;
    pg_dir = running_proc->pgdir + 0x200;
    pg_tab = running_proc->heap_pg_tab->physical;
    rpage = (struct page_frame *)hash_get(&hashmap, (void *)key, 0);
    running_proc->nbpages++;
    result = rpage->physical;
    while (++index < 0x20000 && pg_tab[index]);
    if (index == 0x20000)
        return 0;
    pg_tab[index] = (unsigned)result | 0x7;
    result = (void *)0x80000000 + index * 0x1000;
    index /= 0x400;
    pg_dir[index] = ((unsigned)pg_tab + 0x1000 * index) | 0x7;
    if (shmcreation)
        queue_add(rpage, &running_proc->heap_page_list, struct page_frame, mainlink, priority);
#else
    (void)index;
    (void)pg_dir;
    (void)pg_tab;
    (void)msg;
    (void)rpage;
    result = hash_get(&hashmap, (void *)key, 0);
    if (shmcreation)
    {
        msg = (struct message *)mem_alloc(sizeof(struct message));
        msg->value = (unsigned)result;
        queue_add(msg, &running_proc->heap_page_list, struct message, mainlink, priority);
    }
#endif
    return result;
}


void shm_release(const char *key)
{
    int counter;
    unsigned *pg_dir, *pg_tab;
    void *result;
    link *index;
    struct page_frame *rpage;
#if (VM)
    rpage = (struct page_frame *)hash_get(&hashmap, (void *)key, 0);
    running_proc->nbpages--;
    result = rpage->physical;
    counter = -1;
    pg_dir = running_proc->pgdir + 0x200;
    pg_tab = running_proc->heap_pg_tab->physical;
    while (++counter < 0x20000 && (pg_tab[counter] & ~0xfff) != (unsigned)result);
    if (counter == 0x20000)
        return;
    pg_tab[counter] = 0;
    if (!(counter % 0x400))
        pg_dir[counter / 0x400] = 0;
    for (index = running_proc->heap_page_list.next; index != &running_proc->heap_page_list; index = index->next)
        if (queue_entry(index, struct page_frame, mainlink) == rpage)
        {
            page_free(queue_out(index->next, struct page_frame, mainlink));
            hash_del(&hashmap, (void *)key);
            break;
        }
#else
        (void)counter;
        (void)pg_dir;
        (void)pg_tab;
        (void)index;
        (void)rpage;
        result = hash_get(&hashmap, (void *)key, 0);
        for (index = running_proc->heap_page_list.next; index != &running_proc->heap_page_list; index = index->next)
            if (*(void **)((unsigned)index - 8) == result)
            {
                mem_free(queue_out(index->next, struct message, mainlink), sizeof(struct message));
                hash_del(&hashmap, (void *)key);
                mem_free(result, 0x1000);
                break;
            }
#endif
}


void shm_info(void)
{
    long counter;
    struct page_frame *page;
    printf("------ Shared Memory Segments ------\n%10s %10s %10s\n", "key", "location", "bytes");
    for (counter = 0; counter < hashmap.count; counter++)
        if ((page = (struct page_frame *)hashmap.table[counter].hash))
            printf("%10s %10p %10d\n", (char *)hashmap.table[counter].key, page->physical, 4096);
}
