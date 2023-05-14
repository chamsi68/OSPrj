#include "clock.h"

unsigned VGAMode = 0x3, Mode0x13curs, MinX = 0, MinY = 0, MaxX = 79, MaxY = 24, blinkc = 0, shell = 0, ctrl = 0, lastcol[25] = { 0 };
char lastword[128] = { 0 };


void writechar(unsigned lig, unsigned col, char c, unsigned char backgroundcolor, unsigned char textcolor, unsigned char blink)
{
    unsigned index1, index2, tmp2;
    unsigned char *tmp1, tmp3, *tmpbuf;
    switch (VGAMode)
    {
    case 0x13:
        tmpbuf = font + 8 * (c - 32);
        for (index2 = 0; index2 < 15; index2++)
        {
            for (index1 = 0; index1 < 8; index1++)
            {
                tmp1 = VGA_ptr_mem(index2 + lig * 15, index1 + col * 8);
                tmp2 = 12 * (lig * 15 + index2 - y) + col * 8 + index1 - x;
                tmp3 = tmpbuf[760 * index2 + index1];
                if (lig * 15 + index2 < y || col * 8 + index1 < x || lig * 15 + index2 >= y + 19 || col * 8 + index1 >= x + 12)
                    *tmp1 = tmp3 ? tmp3 : backgroundcolor;
                else
                {
                    if (cursor[tmp2] == 1)
                        *tmp1 = tmp3 ? tmp3 : backgroundcolor;
                    tmp[tmp2] = tmp3 ? tmp3 : backgroundcolor;
                }
            }
        }
        break;
    default:
        *ptr_mem(lig, col) = c;
        ptr_mem(lig, col)[1] = blink << 7 | backgroundcolor << 4 | textcolor;
    }
}


void setcursor(unsigned lig, unsigned col)
{
    unsigned index, ligl, coll, tmp2;
    unsigned short curs;
    unsigned char *tmp1;
    switch (VGAMode)
    {
    case 0x13:
        ligl = Mode0x13curs / (320 * 15);
        coll = (Mode0x13curs % (320 * 15)) / 8;
        for (index = 0; index < 15; index++)
        {
            tmp1 = VGA_ptr_mem(index, Mode0x13curs);
            tmp2 = 12 * (ligl * 15 + index - y) + coll * 8 - x;
            if (ligl * 15 + index < y || coll * 8 < x || ligl * 15 + index >= y + 19 || coll * 8 >= x + 12)
                *tmp1 = 0;
            else
            {
                if (cursor[tmp2] == 1)
                    *tmp1 = 0;
                tmp[tmp2] = 0;
            }
        }
        Mode0x13curs = 320 * 15 * lig + 8 * col;
        break;
    default:
        curs = col + lig * 80;
        outb(0xf, 0x3d4);
        outb(curs & 0xff, 0x3d5);
        outb(0xe, 0x3d4);
        outb(curs >> 8, 0x3d5);
    }
}


void getcursor(unsigned *lig, unsigned *col)
{
    unsigned short pos;
    switch (VGAMode)
    {
    case 0x13:
        *lig = Mode0x13curs / (320 * 15);
        *col = (Mode0x13curs % (320 * 15)) / 8;
        break;
    default:
        outb(0xf, 0x3d4);
        pos = inb(0x3d5);
        outb(0xe, 0x3d4);
        pos |= inb(0x3d5) << 8;
        *lig = pos / 80;
        *col = pos % 80;
    }
}


void initShell(void)
{
    unsigned short screen_offset, bitmap_offset = 0;
    unsigned j, a = 15, b = 25;
    keyboard_init();
    shell = 1;
    if (VGAMode == 0x13)
    {
        screen_offset = (b << 8) + (b << 6) + a;
        bitmap_offset = 0;
        for (j = 0; j < 162; j++)
        {
            memcpy(VGA_ptr_mem(0, screen_offset), gui + bitmap_offset, 289);
            bitmap_offset += 289;
            screen_offset += 320;
        }
        for (j = 0; j < 19; j++)
            memcpy(tmp + 12 * j, VGA_ptr_mem(j + 100, 160), 12);
        MinX = 3;
        MinY = 4;
        MaxX = 36;
        MaxY = 11;
        setcursor(MinY, MinX);
    }
}


void blink(void)
{
    unsigned index, lig, col, tmp2;
    unsigned char *tmp1, tmp3;
    if (VGAMode == 0x13)
    {
        lig = Mode0x13curs / (320 * 15);
        col = (Mode0x13curs % (320 * 15)) / 8;
        for (index = 0; index < 15; index++)
        {
            tmp1 = VGA_ptr_mem(index, Mode0x13curs);
            tmp2 = 12 * (lig * 15 + index - y) + col * 8 - x;
            tmp3 = !(blinkc % 50) * 0xff;
            if (lig * 15 + index < y || col * 8 < x || lig * 15 + index >= y + 19 || col * 8 >= x + 12)
                *tmp1 = tmp3;
            else
            {
                if (cursor[tmp2] == 1)
                    *tmp1 = tmp3;
                tmp[tmp2] = tmp3;
            }
        }
    }
}


void clearscreen(unsigned char backgroundcolor)
{
    unsigned lig, col;
    setcursor(MinY, MinX);
    for (lig = MinY; lig <= MaxY; lig++)
        for (col = MinX; col <= MaxX; col++)
            writechar(lig, col, 0x20, backgroundcolor, WHITE, 0);
    if (VGAMode == 0x13 && MaxY == 12)
        for (lig = 0; lig < 5; lig++)
            memset(VGA_ptr_mem(15 * 13 + lig, MinX * 8), backgroundcolor, (MaxX - MinX + 1) * 8);
}


void scroll(void)
{
    unsigned col, lig, tmp1, tmp2, tmp3, tmp4, tmp5;
    switch (VGAMode)
    {
    case 0x13:
        for (lig = 15 * MinY; lig < 15 * MaxY; lig++)
            memcpy(VGA_ptr_mem(lig, MinX * 8), VGA_ptr_mem(lig + 15, MinX * 8), (MaxX - MinX + 1) * 8);
        tmp1 = 15 * (MinY + 1) > y ? 15 * (MinY + 1) - y : 0;
        tmp2 = 15 * MaxY < y + 4 ? 15 * (MaxY + 1) > y ? 15 * (MaxY + 1) - y : tmp1 : 19;
        tmp3 = 8 * MinX > x ? 8 * MinX - x : 0;
        tmp4 = 8 * MaxX < x + 4 ? 8 * (MaxX + 1) > x ? 8 * (MaxX + 1) - x : tmp3 : 12;
        tmp5 = tmp4 > tmp3 ? tmp4 - tmp3 : 0;
        for (lig = tmp1; lig < tmp2; lig++)
            memcpy(VGA_ptr_mem(y + lig - 15, x + tmp3), tmp + 12 * lig, tmp5);
        for (lig = tmp1; lig < tmp2; lig++)
            memcpy(tmp + 12 * lig, VGA_ptr_mem(y + lig, x + tmp3), tmp5);
        for (lig = 0; lig < 19; lig++)
        {
            for (col = 0; col < 12; col++)
            {
                tmp1 = 12 * lig + col;
                if (cursor[tmp1] != 1)
                    *VGA_ptr_mem(y + lig, x + col) = cursor[tmp1];
            }
        }
        for (lig = 0; lig < 15; lig++)
            *VGA_ptr_mem(lig - 15, Mode0x13curs) = 0;
        break;
    default:
        for (lig = MinY; lig < MaxY; lig++)
            memcpy(ptr_mem(lig, MinX), ptr_mem(lig + 1, MinX), (MaxX - MinX + 1) * 2);
    }
    for (col = MinX; col <= MaxX; col++)
        writechar(MaxY, col, 0x20, BLACK, WHITE, 0);
}


void treatchar(unsigned lig, unsigned col, char c, unsigned char backgroundcolor, unsigned char textcolor)
{
	unsigned index, str = strlen(lastword);
    switch (c)
    {
    case '\b':
        if (lig + col > MinY + MinX)
        {
            if (str)
                lastword[str - 1] = 0;
            if (col == MinX)
            {
                setcursor(lig - 1, lastcol[lig - 1]);
                writechar(lig - 1, lastcol[lig - 1], 0x20, backgroundcolor, textcolor, 0);
            }
            else
            {
                setcursor(lig, col - 1);
                writechar(lig, col - 1, 0x20, backgroundcolor, textcolor, 0);
            }
        }
        break;
    case '\t':
        *lastword = 0;
        if (col < MaxX - 6)
            setcursor(lig, 8 * (col / 8 + 1));
        else
            setcursor(lig, MaxX);
        break;
    case '\n':
        lastcol[lig] = col;
        *lastword = 0;
        if (lig < MaxY)
            setcursor(lig + 1, MinX);
		else
        {
            scroll();
            setcursor(MaxY, MinX);
        }
        break;
    case '\f':
        *lastword = 0;
        clearscreen(backgroundcolor);
        break;
    case '\r':
        *lastword = 0;
        setcursor(lig, MinX);
        for (index = MinX; index <= MaxX; index++)
            writechar(lig, index, 0x20, backgroundcolor, textcolor, 0);;
	    break;
    default:
        if (c == ' ')
        {
            lastcol[lig] = col;
            *lastword = 0;
            str = 0;
        }
        else
        {
            if (str + 1 > MaxX - MinX)
            {
                lastcol[lig] = col;
                *lastword = 0;
                str = 0;
            }
            else
            {
                lastword[126] = c;
                strcat(lastword, lastword + 126);
            }
        }
        if (col < MaxX)
        {
            if (col == MinX && str)
            {
                setcursor(lig, MinX + str + 1);
                for (index = 0; index <= str; index++)
                {
                    writechar(lig - 1, MaxX - str + index, 0x20, backgroundcolor, textcolor, 0);
                    writechar(lig, MinX + index, lastword[index], backgroundcolor, textcolor, 0);
                }
                col += str;
            }
            else
                setcursor(lig, col + 1);
		}
        else
        {
            if (lig < MaxY)
                setcursor(lig + 1, MinX);
            else
            {
                writechar(lig, col, c, backgroundcolor, textcolor, 0);
                scroll();
                setcursor(MaxY, MinX);
                goto label;
            }
        }
        writechar(lig, col, c, backgroundcolor, textcolor, 0);
label:
        if (str > 0)
            str++;
    }
}


void console_putbytes(const char *s, int len)
{
    int index;
    unsigned lig, col;
    for (index = 0; index < len; index++)
    {
        getcursor(&lig, &col);
        treatchar(lig, col, s[index], BLACK, WHITE);
    }
}
