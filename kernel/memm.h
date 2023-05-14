#include "files.h"
#include "hash.h"
#include "userspace_apps.h"

#define UNPAGES 0xc0000

extern char user_start[];
extern unsigned pgdir[], pgtab[];
extern hash_t hashmap;
extern link dead_pids;
extern struct process *pshell;

enum _mflags { FREE, USED };

void init_user_mem(void);

struct page_frame *page_alloc(void);

void page_free(struct page_frame *page);

#if (VM)
int start(const char *process_name, int ssize, int prio, void *arg);
#endif

void *vaddr_to_phyaddr(void *vaddr);