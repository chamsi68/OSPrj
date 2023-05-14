#include "stdint.h"
#include "string.h"
#include "stdbool.h"
#include "stdio.h"
#include "cpu.h"
#include "segment.h"

#define CLOCKFREQ 50
#define QUARTZ 0x1234DD
#define IDT(num_IT) ((unsigned *)0x1000 + 2 * num_IT)
#define ptr_mem(lig, col) ((unsigned char *)0xb8000 + (80 * (lig) + col) * sizeof(short))
#define VGA_ptr_mem(lig, col) ((unsigned char *)0xa0000 + 320 * (lig) + col)

extern unsigned x, y, MaxX, blinkc;
extern unsigned char tmp[228], cursor[228], font[11400], gui[46818];

enum colors { BLACK, BLUE, GREEN, CYAN, RED, MAGENTA, BROWN, GREY, DARKGREY, LIGHTBLUE, LIGHTGREEN, LIGHTCYAN, LIGHTRED, LIGHTMAGENTA, YELLOW, WHITE };

void writechar(unsigned lig, unsigned col, char c, unsigned char backgroundcolor, unsigned char textcolor, unsigned char flash);

void clearscreen(unsigned char backgroundcolor);

unsigned long cons_read(char *string, unsigned long length);

void wait_clock(unsigned clock);

unsigned nbr_secondes(void);

void set_mode_0x3(void);

void set_mode_0x13(void);

void traitant_IT_44(void);

void traitant_IT_33(void);

void scheduler(void);

int keyboard_init(void);

void initShell(void);

void blink(void);

void PIC2_IRQ(unsigned num_IRQ, unsigned mask);

void initMouse(void);

void traitant_IT_32(void);

void tic_PIT(void);

void init_traitant_IT(unsigned num_IT, void (*traitant)(void), unsigned user);

void set_clock(void);

void PIC1_IRQ(unsigned num_IRQ, unsigned mask);

void clock_settings(unsigned long *quartz, unsigned long *ticks);

unsigned current_clock(void);
