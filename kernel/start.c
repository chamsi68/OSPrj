#include "semaphores.h"

void syscall(void);


void kernel_start(void)
{
    init_traitant_IT(0x49, syscall, 1);
    init_traitant_IT(32, traitant_IT_32, 0);
    init_traitant_IT(33, traitant_IT_33, 0);
    init_traitant_IT(44, traitant_IT_44, 0);
    PIC1_IRQ(0, 0);
    PIC1_IRQ(1, 0);
    PIC1_IRQ(2, 0);
    PIC2_IRQ(12, 0);
    set_mode_0x13();
    clearscreen(0);
    set_clock();
    initMouse();
    sInit();
    initShell();
    init_user_mem();
    init();
    //On saute à la pile de idle, on fait un ret pour placer le registre eip à l'adresse de idle
    __asm__("movl\t%0, %%esp\n\tret" :: "r" ((unsigned)pidle.stack + 16384 - 12));
}
