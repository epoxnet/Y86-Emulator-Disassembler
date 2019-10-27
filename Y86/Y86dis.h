//<Y86.h>
#ifndef Y86_H
#define Y86_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>

typedef union bitfeild{
	struct Bfield{
		unsigned int hi: 4;
		unsigned int lo: 4;
	}Bfield;
	 unsigned char  byte;
} BitField;



char * loadmem (char *filename, char* buf);//takes in the file & load into mem

void setFlags(int x);//

void halt();

void nop();

void printregister(int x);

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

void readX();

void writeX();

void movsbl();

void op();

void jmps();

int  execute();

int main(int argc, char**argv);

#endif //Y86_H
