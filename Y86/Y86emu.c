//<Y86emu.c>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include "Y86emu.h"

bool wyd = true;
int registers[8];
unsigned int program_counter = 0;
unsigned int inital = 0;
char *memory;
Bitfield *kek;
int ZF;
int OF;
int SF;
unsigned int  memsize;

char * loadmem (char *filename, char* buf){
	FILE* filetoread = fopen(filename,"r");
       	//maxx legnth
	int lineno = 0;
		
	char* memory;
	while(1){
		if(fgets(buf,1000,filetoread) == NULL) break;
		if(lineno == 0){
			//exculde everything but digits and plop it as a hex in memsize
		//TODO Possibbly have to ignore size
			sscanf(buf,"%*[^0123456789]%x",&memsize);
			memory = malloc(sizeof(char) * memsize);
		}
		lineno++;
		//read text directive
		if(strstr(buf,".text") != NULL){
			char *ptr = buf;
			//read program counter
			while(!isdigit(*ptr)){
				ptr ++;		
			}
			//hit program counter
			sscanf(ptr,"%x",&program_counter);
			inital = program_counter;
			//now readevery two and plop it in the shit
			while(!isspace(*ptr)){
				ptr ++;
			}

			while(isspace(*ptr)){
				ptr ++;
			}
			while(*ptr != '\0'){

				if(isspace(*ptr)){
					break;
				}
				Bitfield* test = malloc(sizeof(Bitfield));
				unsigned int blah;	
				//ideally points to first irl instruction
				sscanf(ptr,"%2x",&blah);
				test->byte = (unsigned char)blah;
				memory[program_counter] = test->byte;
				test->byte = memory[program_counter];
				program_counter ++;
				ptr +=2;
				
				free(test);
			}
		}
		//byte
		if(strstr(buf,".byte") != NULL){
			char *ptr = buf;
			ptr +=5;
			unsigned int adress;
			 int value;
			sscanf(ptr,"%x %x",&adress,&value);
			memory[adress] = (unsigned char)value;

		}
		//long>
		if(strstr(buf,".long") != NULL){
			char *ptr = buf;
			ptr +=6;
			unsigned int adress;
			unsigned int actualval;
			 char  value [1999];
			sscanf(ptr,"%x %s",&adress,value);
			actualval = atoi(value);
			unsigned int* pt = (unsigned int*)&memory[adress];
			*pt = actualval;	
		//	memcpy(&memory[adress],&actualval,4);

		}

		if(strstr(buf,".string") != NULL){
			char *ptr = buf;
			ptr +=7;
			unsigned int adress;

			 char  value [1999];

			sscanf(ptr,"%x  \"%[^\"]\"",&adress,value);
			int i;
			unsigned int it = adress;

			for(i=0;i<strlen(value);i++){
				memory[it] = value[i];
				it++;	
			}
			memory[it] = '\0';

		}
		
	}	
	fclose(filetoread);
	return memory;

}
void setFlags(int x){
	if(x<0){
		SF =1;
	}
	if(x>0){
		SF = 0;
	}
	if(x==0){
		ZF =1;
	}
	if(x!=0){
		ZF = 0;
	}
}

void halt(){
	printf("HALT\n");
}
void nop(){
	inital ++;
	return;
}
void irmovl(){
	inital++;
	Bitfield *reg = malloc(sizeof(Bitfield));
	reg ->byte = memory[inital];
	int index = reg->Bfield.hi;
	inital++;
	int * val =  (int *)&memory[inital];
	registers[index] = * val  ;
	inital +=4;
	free(reg);
}
void rrmovl(){
	inital++;
	Bitfield *reg = malloc(sizeof(Bitfield));
	reg ->byte = memory[inital];
	int  rB = reg->Bfield.hi;
	int rA = reg->Bfield.lo;
	registers[rB] = registers[rA];
	free(reg);
	inital ++;
}
void rmmovl(){
	//register to memory move;
	inital ++;
	Bitfield *reg = malloc(sizeof(Bitfield));
	reg->byte = memory[inital];
	int rA = reg->Bfield.lo;
	int rB = reg->Bfield.hi;
	inital++;
	int *offset = (int *)&memory[inital];
	memcpy(&memory[registers[rB] + *offset],&registers[rA],4);
	free(reg);
	inital +=4;
}

void mrmovl(){
	inital ++;
	Bitfield *reg = malloc(sizeof(Bitfield));
	reg->byte = memory[inital];
	int rA = reg->Bfield.lo;
	int rB = reg->Bfield.hi;
	inital ++;
	int *offset = (int *)&memory[inital];
	memcpy(&registers[rA],&memory[registers[rB] + *offset],4);	
	free(reg);
	inital +=4;

}


void addl(){

	inital++;
	Bitfield *reg = malloc(sizeof(Bitfield));
	reg->byte = memory[inital];
	int rA = reg->Bfield.lo;
	int rB = reg->Bfield.hi;
	//overflow detection
	if (registers[rA] > 0 && registers[rB] > INT_MAX - registers[rA]) {
		OF = 1;
	 } 
	else if (registers[rA] < 0 && registers[rB] < INT_MIN - registers[rA]) {
	                    /* handle underflow */
				OF =1;
	                       }
	else{
		//reset OF
		OF = 0;
	}
	//check ZERO FLAG
        registers[rB] = (int)registers[rA] + (int)registers[rB];
	setFlags(registers[rB]);
	free(reg);
	inital ++;	
}
void subl(){
	inital ++;
	Bitfield *reg = malloc(sizeof(Bitfield));
	reg->byte = memory[inital];
	int rA = reg->Bfield.lo;
	int rB = reg->Bfield.hi;
	registers[rB] = registers[rB] - registers[rA];
	setFlags(registers[rB]);	
	free(reg);
	inital ++;	
}

void andl(){
	OF = 0;  /*bitwise operators get a 0 */

	inital++;
	Bitfield *reg = malloc(sizeof(Bitfield));
	reg->byte = memory[inital];
	int rA = reg->Bfield.lo;
	int rB = reg->Bfield.hi;
	registers[rB] = registers[rB]&registers[rA];
	setFlags(registers[rB]);
	free(reg);
	inital ++;

}
void xorl(){
	OF = 0;
	//set OF to 0
	inital++;
	Bitfield *reg = malloc(sizeof(Bitfield));
	reg->byte = memory[inital];
	int rA = reg->Bfield.lo;
	int rB = reg->Bfield.hi;
	registers[rB] = registers[rB]^registers[rA];
	setFlags(registers[rB]);
	free(reg);
	inital ++;
}
void mull(){
	inital++;
	Bitfield *reg = malloc(sizeof(Bitfield));
	reg->byte = memory[inital];
	int rA = reg->Bfield.lo;
	int rB = reg->Bfield.hi;
	if(registers[rA]>0 && registers[rB]>0 && registers[rA]*registers[rB]<=0) OF=1;
	if(registers[rA]<0 && registers[rB]<0 && registers[rA]*registers[rB]<=0) OF=1;
	if(registers[rA]>0 && registers[rB]<0 && registers[rA]*registers[rB]>=0) OF=1;
	else{
		OF = 0;
	}
	registers[rB] = registers[rB]*registers[rA];
	setFlags(registers[rB]);
	free(reg);
	inital ++;
}
void pushl(){
	inital ++;
	Bitfield *reg = malloc(sizeof(Bitfield));
	reg ->byte = memory[inital];
	int rA = reg->Bfield.lo;
	registers[4] -=4;
	int loc = registers[4];
	//subtracts four from ESP
	*((unsigned int *)&memory[loc]) = registers[rA];
	free(reg);
	inital ++;	
}
void popl(){
	inital ++;
	Bitfield *reg = malloc(sizeof(Bitfield));
	reg -> byte = memory[inital];
	int rA = reg->Bfield.lo;
	int place = registers[4];
	registers[rA] = *((unsigned int *)&(memory[place]));
	registers[4] +=4;
	free(reg);
	inital++;

}
void call(){
	inital ++;
	unsigned int offset = *((unsigned int *)&memory[inital]);
	//grab 32 bit dest and place it in dest
	registers[4] -= 4;
	inital +=4; 
	*((unsigned int *)&(memory[registers[4]])) = inital;
	inital = offset ;
	}

void ret(){
	memcpy(&inital,&memory[registers[4]],4);
	registers[4] +=4;
	//dscrement one from inital (will be added back anyway after the instruction)

}
void cmpl(){
	inital ++;
	Bitfield *reg = malloc(sizeof(Bitfield));
	reg->byte = memory[inital];
	int rA = reg->Bfield.lo;
	int rB = reg->Bfield.hi;
	int x = registers[rB] - registers[rA];
	if(x==0){
		ZF =1;
	}
	if(x!=0){
		ZF = 0;
	}
	if(x>0){
		SF = 0;
	}
	if(x<0){
		SF =1;
	}
	inital ++;
}


//MAKE SURE TO GOTO ADRESS -1, casue after instruction we add it back anyway, but we dont want to double count
void jmp(){
	inital ++;
	//JUMP
	//set inital to the ofset
	unsigned int dest; 
	memcpy(&dest,&memory[inital],4);
      	inital = dest;	


}

void jle(){
	if(ZF ==1 || (SF != OF)){
		inital ++;
		unsigned int dest;
		memcpy(&dest,&memory[inital],4);
		inital = dest;
	}
	else{
		inital +=4;
	}
}
void jl(){
	if(SF != OF){

		inital ++;
		unsigned int dest;
		memcpy(&dest,&memory[inital],4);
		inital = dest;

		//JUMP
	}
	else{
		inital +=4;
	}
}

void je(){
	if(ZF ==1){
		inital ++;
		unsigned int dest;
		memcpy(&dest,&memory[inital],4);			
		inital = dest;
	//decremnet inital by one because 
		//JUMP
	}
	else{
		inital +=5;
		//DONT JUMP
	}
}
void jne(){
	if(ZF ==0){
		
		inital ++;
		unsigned int dest;
		memcpy(&dest,&memory[inital],4);			
		inital = dest;
		//JUMP
	}
	else{
		inital +=4;//DONT JUMP
	}
}
void jge(){
	if(SF == OF){
		
		inital ++;
		unsigned int dest;
		memcpy(&dest,&memory[inital],4);			
		inital = dest;
		//JUMP
	}
	else{
		inital +=4;
	}
}
void jg(){
	if(ZF ==0 &&(SF == OF)){

		inital ++;
		unsigned int dest;
		memcpy(&dest,&memory[inital],4);			
		inital = dest;
		//JUMP
	}

	else{
		inital +=4;
	}	

}
void writeb(){
	inital ++;
	Bitfield *reg = malloc(sizeof(Bitfield));
	reg->byte = memory[inital];
	int rA = reg->Bfield.lo;
	inital ++;
	//get offset
	unsigned int *offset = (unsigned int *)&memory[inital];
	printf("%c",memory[registers[rA] +*offset]);
	inital +=4;
	free(reg);
}
void writel(){
	
	inital ++;
	Bitfield *reg = malloc(sizeof(Bitfield));
	reg->byte = memory[inital];
	int rA = reg->Bfield.lo;
	inital ++;
	//get offset
	unsigned int *offset = (unsigned int *)&memory[inital];
	printf("%d",*(int *)&memory[registers[rA] +*offset]);
	inital +=4;
	free(reg);
}
void readb(){
	inital ++;
	Bitfield *reg = malloc(sizeof(Bitfield));
	reg->byte = memory[inital];
	int rA = reg->Bfield.lo;
	inital ++;
	unsigned int *offset = (unsigned int *)&memory[inital];
	char byte;
	if(scanf("%c",&byte) == EOF){
		ZF =1;
	}	
	else{
		ZF =0;
	}
	memcpy(&memory[registers[rA] + *offset],&byte,1);
	inital +=4;
	
}

void readl(){
	
	inital ++;
	Bitfield *reg = malloc(sizeof(Bitfield));
	reg->byte = memory[inital];
	int rA = reg->Bfield.lo;
	inital ++;
	unsigned int *offset = (unsigned int *)&memory[inital];
	int  lon;
	if(scanf("%d",&lon) == EOF){
		ZF =1;
	}
	else{
		ZF = 0;
	}
	memcpy(&memory[registers[rA] + *offset],&lon,4);
	inital +=4;

}
void(*readX[2])() = {readb,readl};
void(*writeX[2])() = {writeb,writel};
void reads(){
	(*readX[kek->Bfield.hi])();
}
void writes(){
	(*writeX[kek->Bfield.hi])();
}
void movsbl(){
	inital ++;
	Bitfield *reg = malloc(sizeof(Bitfield));
	reg->byte = memory[inital];
        int rA = reg->Bfield.lo;
	int rB = reg->Bfield.hi;
	inital ++;
	 int *offset = ( int *)(&memory[inital]);
	registers[rA] = (int)(int8_t)memory[registers[rB] + *offset];
	inital +=4;
		
}
void (*jXX[7]) () = {jmp,jle,jl,je,jne,jge,jg};
void (*op1[6])() = {addl,subl,andl,xorl,mull,cmpl};
void op(){
	if(kek->Bfield.hi >5){
		wyd = false;
	}
	else{
		(*op1[kek->Bfield.hi])();
	}

}
void jmps(){

	if(kek->Bfield.hi >6){
		wyd = false;
	}
	else{
		(*jXX[kek->Bfield.hi])();
	}
}
void (*y86table[16])() = {nop,halt,rrmovl,irmovl,rmmovl,mrmovl,op,jmps,call,ret,pushl,popl,reads,writes,movsbl};
int  execute(){
	 unsigned int x =0;

	while(x!=0x10){
		x = memory[inital];
		if(inital>memsize){

			printf("ADR\n");
			printf("Invalid Adress \n");
			break;
		}
		if(wyd == false){
			printf("INS\n");
			printf("Invalid instruction\n");
			break;
		}	
		kek = malloc(sizeof(Bitfield));
		kek -> byte = x;
		(*y86table[kek->Bfield.lo])();
		free(kek);
	}	
	return 0;
}
int main(int argc, char**argv){
	char buf[1000];
	if(argc !=2){
		fprintf(stderr,"Invalid No Of Args\n");
		return -1;
	}

	if(strcmp("-h",argv[1]) == 0){
		printf("Usage: ./y86emu <inputfile>\n");
		return 0;
	}
	memory = loadmem(argv[1],buf);
	execute();
	free(memory);
	return 0;


}
