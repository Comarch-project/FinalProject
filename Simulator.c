#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define NUMMEMORY 65536 /* maximum number of words in memory */
#define NUMREGS 8 /* number of machine registers */
#define MAXLINELENGTH 1000

char decimalToBinary(int);
int getBit(int , int );

typedef struct stateStruct {
    int pc;
    int mem[NUMMEMORY];
    int reg[NUMREGS];
    int numMemory;
} stateType;
void printState(stateType *);

int main(int argc, char *argv[])
{
    char line[MAXLINELENGTH];
    stateType state;
    FILE *filePtr;

    if (argc != 2) {
	printf("error: usage: %s <machine-code file>\n", argv[0]);
	exit(1);
    }

    filePtr = fopen(argv[1], "r");
    if (filePtr == NULL) {
	printf("error: can't open file %s", argv[1]);
	perror("fopen");
	exit(1);
    }

    /* read in the entire machine-code file into memory */
    for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL;
	state.numMemory++) {

        char biCode;

        if (sscanf(line, "%d", state.mem+state.numMemory) != 1) {
            printf("error in reading address %d\n", state.numMemory);
            exit(1);
        }
        printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
       
        printf("bi = ",biCode);//test dec to binary 
        biCode=decimalToBinary(state.mem[state.numMemory]);//covert d2b
        
        for(int bitPosition=0; bitPosition<32; bitPosition++){
            //implement bicode to array and cotain array[33]  {[33], [33], [],}
            int bitValue = getBit(state.mem[state.numMemory], bitPosition);
            //printf("bit[%d] = %d\n", bitPosition, bitValue);//test each bit position

        }
        
    }

    state.pc = 0; // set pc to 0
    for (int i = 0; i < NUMREGS; i++) {
        state.reg[i] = 0;  // Set all registers to 0
    }

   // printState(&state);
    

    return(0);
}

void printState(stateType *statePtr)
{
    int i;
    printf("\n@@@\nstate:\n");
    printf("\tpc %d\n", statePtr->pc);
    printf("\tmemory:\n");
	for (i=0; i<statePtr->numMemory; i++) {
	    printf("\t\tmem[ %d ] %d\n", i, statePtr->mem[i]);
	}
    printf("\tregisters:\n");
	for (i=0; i<NUMREGS; i++) {
	    printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
	}
    printf("end state\n");
}

char decimalToBinary(int decimal) {
    
    // Determine the number of bits needed for the binary representation
    int numBits = sizeof(decimal) * 8;
    int binary[numBits]; // Array to store binary bits

    // Calculate binary representation
    for (int i = numBits - 1; i >= 0; i--) {
        binary[i] = decimal % 2;
        decimal /= 2;
    }

    // Print binary representation
    for (int i = 0; i < numBits; i++) {
        printf("%d", binary[i]);
        if ((i + 1) % 4 == 0) // Print a space every 4 bits for readability
            printf(" ");
    }
    printf("\n");
}

int getBit(int decimal, int bitPosition) {
    return (decimal & (1 << bitPosition)) >> bitPosition;
}


