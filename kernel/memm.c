#include "memm.h"

struct process *pshell;
unsigned upgtab[UNPAGES] __attribute__ ((aligned(0x1000)));
struct page_frame user_mem_map[UNPAGES];


void init_user_mem(void)
{
    unsigned index;
    struct page_frame *page = user_mem_map;
    hash_init_string(&hashmap);
    for (index = 0; index < UNPAGES; index++)
    {
        page->status = FREE;
        page->physical = user_start + 0x1000 * index;
        page++;
    }
}


struct page_frame *page_alloc(void)
{
    int index = -1;
    struct page_frame *page;
    while (++index < UNPAGES && (page = user_mem_map + index)->status);
    if (index == UNPAGES)
        return 0;
    page->status = USED;
    index = ((unsigned)page->physical - (unsigned)user_start) / 0x1000;
    upgtab[index] = (unsigned)page->physical | 0x3;
    pgdir[0x100 + index / 0x400] = ((unsigned)upgtab + 0x1000 * (index / 0x400)) | 0x3;
    memset(page->physical, 0, 0x1000);
    return page;
}


void page_free(struct page_frame *page)
{
    int index = -1;
    unsigned temp;
    page->status = FREE;
    temp = ((unsigned)page->physical - (unsigned)user_start) / 0x1000;
    upgtab[temp] = 0;
    temp /= 0x400;
    while (++index < 0x400 && !upgtab[0x400 * temp + index]);
    if (index == 0x400)
        pgdir[0x100 + temp] = 0;
}

#if (VM)
int start(const char *process_name, int ssize, int prio, void *arg)
{
    int pid, index = -1, temp;
    unsigned esp, *pg_dir, *pg_tab, sys_exit;
    char exit[14] = { 0xb8, 0, 0, 0, 0, 0x89, 0xc3, 0xb8, 0x1, 0, 0, 0, 0xcd, 0x49 };
    const struct uapps *app;
    struct process *process;
    struct message *msg;
    struct page_frame *page;

    while ((app = symbols_table + ++index)->name && strcmp(app->name, process_name));
    if (!app->name || pidc < 0 || (pidc >= NBPROC && queue_empty(&dead_pids)) || (unsigned)ssize > 8180)
        return -1;
    process = (struct process *)mem_alloc(sizeof(struct process));
    if (!strcmp(process_name, "shell"))
        pshell = process;
    INIT_LIST_HEAD(&process->stack_page_list);
    INIT_LIST_HEAD(&process->heap_page_list);
    INIT_LIST_HEAD(&process->bin_page_list);
    INIT_LIST_HEAD(&process->childs_list);
    process->parent = running_proc;
    if (running_proc)
        queue_add(process, &running_proc->childs_list, struct process, childlink, priority);
    process->status = READY;
    strcpy(process->name, process_name);
    process->sustack = (int *)mem_alloc(0x4000);
    esp = (unsigned)process->sustack + 0x4000;
    process->registers[8] = esp;
    process->stack = (int *)0xc0000000;
    esp = (unsigned)process->stack + ssize + 44;
    process->nbpages = 8 + ((ssize + 44 + 3 * sizeof(int)) / 0x1000 + 1) * 0x1000;
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
    process->pg_dir = page_alloc();
    process->pgdir = process->pg_dir->physical;
    process->bin_pg_tab = page_alloc();
    process->heap_pg_tab = page_alloc();
    process->stack_pg_tab = page_alloc();

    //Mapping the kernel
    pg_dir = process->pgdir;
    memcpy(pg_dir, pgdir, 64 * sizeof(unsigned));
    //Mapping the kernel

    //Loading & mapping the binary
    pg_dir = process->pgdir + 0x100;
    pg_tab = process->bin_pg_tab->physical;
    temp = (unsigned)app->end - (unsigned)app->start + 1;
    for (index = 0; index < temp / 0x1000 + 1; index++)
    {
        page = page_alloc();
        process->nbpages++;
        queue_add(page, &process->bin_page_list, struct page_frame, mainlink, priority);
        memcpy(page->physical, app->start + 0x1000 * index, index < temp / 0x1000 ? 0x1000 : temp % 0x1000);
        pg_tab[index] = (unsigned)page->physical | 0x7;
    }
    memcpy(page->physical + 0x1000 - 14, exit, 14);
    sys_exit = 0x40000000 + index * 0x1000 - 9;
    *pg_dir = (unsigned)pg_tab | 0x7;
    //Loading & mapping the binary
    
    //Stack allocation
    pg_dir = process->pgdir + 0x300;
    pg_tab = process->stack_pg_tab->physical;
    page = page_alloc();
    queue_add(page, &process->stack_page_list, struct page_frame, mainlink, priority);
    esp = (unsigned)page->physical + ssize + 44;
    *pg_tab = (unsigned)page->physical | 0x7;
    for (index = 1; index < (ssize + 44 + 12) / 0x1000 + 1; index++)
    {
        page = page_alloc();
        queue_add(page, &process->stack_page_list, struct page_frame, mainlink, priority);
        pg_tab[index] = (unsigned)page->physical | 0x7;
    }
    *pg_dir = (unsigned)pg_tab | 0x7;
    //Stack allocation

    *((unsigned *)esp) = 0x40000000;
    ((unsigned *)esp)[1] = sys_exit;
    ((unsigned *)esp)[2] = (unsigned)arg;
    queue_add(process, &ready_head_list, struct process, mainlink, priority);
    scheduler();
    return pid;
}
#endif

void *vaddr_to_phyaddr(void *vaddr)
{
    unsigned index, index1 = (unsigned)vaddr & 0xfff, index2 = ((unsigned)vaddr >> 12) & 0x3ff, index3 = (unsigned)vaddr >> 22;
    if (!vaddr)
        return vaddr;
    if ((unsigned)vaddr < 0x40000000)
        return (void *)1;
    if (!(index = running_proc->pgdir[index3] & ~0xfff))
        return (void *)1;
    if (!(index = ((unsigned *)index)[index2]))
        return (void *)1;
    return (void *)((index & ~0x3ff) + index1);
}
