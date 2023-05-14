#include "clock.h"

unsigned CLOCK = 0;


void tic_PIT(void)
{
    char char_horloge[9];
    unsigned index, secondes, minutes, heures;
    secondes = (++CLOCK / 50) % 60;
    minutes = (CLOCK / 3000) % 60;
    heures = (CLOCK / 180000) % 60;
    sprintf(char_horloge, "%02d:%02d:%02d", heures, minutes, secondes);
    for (index = 0; index < 8; index++)
        writechar(0, MaxX - 7 + index, char_horloge[index], BLACK, WHITE, 0);
    if (!(blinkc % 25))
        blink();
    blinkc++;
    scheduler();
}


void init_traitant_IT(unsigned num_IT, void (*traitant)(void), unsigned user)
{
    *IDT(num_IT) = (KERNEL_CS << 16) | ((unsigned)traitant & 0xffff);
    IDT(num_IT)[1] = ((unsigned)traitant & 0xff0000) | 0x8e00;
    if (user)
        IDT(num_IT)[1] |= 0x6000;
}


void set_clock(void)
{
    outb(0x34, 0x43);
    outb((QUARTZ / CLOCKFREQ) & 0xff, 0x40);
    outb((QUARTZ / CLOCKFREQ) >> 8, 0x40);
}


void PIC1_IRQ(unsigned num_IRQ, unsigned mask)
{
    uint8_t valeur_masque = inb(0x21);
    if (!mask)
        valeur_masque &= ~(1 << num_IRQ);
    else
        valeur_masque |= 1 << num_IRQ;
    outb(valeur_masque, 0x21);
}


void clock_settings(unsigned long *quartz, unsigned long *ticks)
{
    *quartz = CLOCKFREQ;
    *ticks = 2;
}


unsigned current_clock(void)
{
    return CLOCK;
}
