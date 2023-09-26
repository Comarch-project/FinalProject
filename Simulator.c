#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define NUMMEMORY 65536 /* maximum number of words in memory */
#define NUMREGS 8 /* number of machine registers */
#define MAXLINELENGTH 1000

char* decimalToBinary(int);
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
        //printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
       
        printf("biToDec :%s\n",decimalToBinary(state.mem[state.numMemory]));

        for(int bitPosition=0; bitPosition<32; bitPosition++){
            //implement bicode to array and cotain array[33]  {[33], [33], [],}
            //int bitValue = getBit(state.mem[state.numMemory], bitPosition);
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

char* decimalToBinary(int n) {
    
    char *binaryStr = (char *)malloc((32 + 1) * sizeof(char));
    // Calculate binary representation
    for (int i = 32 - 1; i >= 0; i--) {
        binaryStr[i] = (n % 2)+'0';
        n = n/2;
    }
    binaryStr[32] = '\0';
    return binaryStr;
}

int getBit(int decimal, int bitPosition) {
    return (decimal & (1 << bitPosition)) >> bitPosition;
}


