//<Y86emu.h>>
#ifndef Y86EMU_H
#define Y86EMU_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <stdbool.h>

typedef union bitfeild{
	struct Bfield{
		unsigned int hi: 4;
		unsigned int lo: 4;
	}Bfield;
	 unsigned char  byte;
} Bitfield;


char * loadmem (char *filename, char* buf);

void setFlags(int x);

void halt();

void nop();

void irmovl();

void rrmovl();

void rmmovl();

void mrmovl();

void addl();

void subl();

void andl();

void xorl();

void mull();

void pushl();

void popl();

void call();

void ret();

void cmpl();

void jmp();

void jle();

void jl();

void je();

void jne();

void jge();

void jg();

void writeb();

void writel();

void readb();

void readl();

void reads();

void writes();

void movsbl();

void op();

void jmps();

int  execute();

int main(int argc, char**argv);



#endif //Y86EMU_H
