#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define NUMMEMORY 65536 /* maximum number of words in memory */
#define NUMREGS 8 /* number of machine registers */
#define MAXLINELENGTH 1000

char* decimalToBinary(int);

typedef struct stateStruct {
    int pc;
    int mem[NUMMEMORY];
    int reg[NUMREGS];
    int numMemory;
} stateType;

struct KeyValuePair {
    int value;
    int address;
};
void printState(stateType *);

int main(int argc, char *argv[])
{
    char line[MAXLINELENGTH];
    stateType state;
    FILE *filePtr;

    // Define the maximum number of key-value pairs in the list
    int maxPairs = 32;
    // Create an array of KeyValuePair structs to store the data
    struct KeyValuePair keyValueList[maxPairs];
    
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
    int linecnt = 0;
    int keyvalpt =0;
    int hlted =0;
    char temp[50];
    /* read in file to check where is halt and insert .fill value*/
    for(state.numMemory =0;fgets(line,MAXLINELENGTH,filePtr)!=NULL;state.numMemory++){
        if (sscanf(line, "%d", state.mem+state.numMemory) != 1) {
            printf("error in reading address %d\n", state.numMemory);
            exit(1);
         }else{
             char *bipo;
             bipo=decimalToBinary(state.mem[state.numMemory]);
             if((bipo[7]=='1')&&bipo[8]=='1'&&(bipo[9]=='0')){
             hlted =1;
             linecnt++;
             continue;
            }
            if(hlted == 1){
            keyValueList[keyvalpt].address = linecnt;
            keyValueList[keyvalpt].value =  state.mem[state.numMemory];
            keyvalpt++;
            }
            linecnt++;
        }
    }

    for(int i=0;i<keyvalpt;i++){
        printf("address:[%d]->value:%d\n",keyValueList[i].address,keyValueList[i].value);
        
    }
    rewind(filePtr);
    /* read in the entire machine-code file into memory */
    linecnt = 0;
    for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL;
	state.numMemory++) {

        char *bipo;
        char pc=state.numMemory;

        if (sscanf(line, "%d", state.mem+state.numMemory) != 1) {
            printf("error in reading address %d\n", state.numMemory);
            exit(1);
        }
        //printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
       
        //printf("biToDec :%s\n",decimalToBinary(state.mem[state.numMemory]));
        bipo=decimalToBinary(state.mem[state.numMemory]);

        if((bipo[7]=='0') && (bipo[8]=='0') && (bipo[9]=='0'))//add
        {
            char rs[4];
            rs[2]=bipo[12];
            rs[1]=bipo[11];
            rs[0]=bipo[10];
            char rt[4];
            rt[2]=bipo[15];
            rt[1]=bipo[14];
            rt[0]=bipo[13];
            char rd[4];
            rd[2]=bipo[31];
            rd[1]=bipo[30];
            rd[0]=bipo[29];
            //printf("add %s,%s,%s",rs,rt,rd);

        }
        else if(!strcmp(bipo[7],"0") && !strcmp(bipo[8],"0") && !strcmp(bipo[9],"1"))//nand
        {

        }else if(!strcmp(bipo[7],"0") && !strcmp(bipo[8],"1") && !strcmp(bipo[9],"0"))//lw
        {

        }else if(!strcmp(bipo[7],"0") && !strcmp(bipo[8],"1") && !strcmp(bipo[9],"1"))//sw
        {

        }else if(!strcmp(bipo[7],"1") && !strcmp(bipo[8],"0") && !strcmp(bipo[9],"0"))//beq
        {

        }else if(!strcmp(bipo[7],"1") && !strcmp(bipo[8],"0") && !strcmp(bipo[9],"1"))//jalr
        {

        }else if(!strcmp(bipo[7],"1") && !strcmp(bipo[8],"1") && !strcmp(bipo[9],"0"))//halt
        {

        }
        // else (!strcmp(bipo[7],"1") && !strcmp(bipo[8],"1") && !strcmp(bipo[9],"1"))//noop

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
    
   char *binaryStr = (char *)malloc((32 + 1) * sizeof(char)); // +1 for the null terminator
    for (int i = 32 - 1; i >= 0; i--) {
    binaryStr[i] = '0'; 
    }
    // Handle negative numbers
    if (n < 0) {
        n = -n;  // Make n positive
        // Convert the absolute value of n to binary and store it in binaryStr
        for (int i = 32 - 1; i >= 0; i--) {
            binaryStr[i] = (n % 2) + '0'; // Convert remainder to character '0' or '1'
            n = n / 2;
        }
        binaryStr[32] = '\0'; // Null-terminate the string
        // Perform two's complement to get the negative binary representation
        for (int i = 0; i < 32; i++) {
            binaryStr[i] = (binaryStr[i] == '0') ? '1' : '0'; // Invert bits
        }
        int carry = 1; // Initialize carry to 1 for addition
        for (int i = 32 - 1; i >= 0; i--) {
            if (binaryStr[i] == '0' && carry == 1) {
                binaryStr[i] = '1';
                carry = 0; // No need to carry anymore
            } else if (binaryStr[i] == '1' && carry == 1) {
                binaryStr[i] = '0'; // Carry over to the next bit
            }
        }
    }else{
    // Convert the decimal number to binary and store it in binaryStr
        for (int i = 32 - 1; i >= 0; i--) {
        binaryStr[i] = (n % 2) + '0'; // Convert remainder to character '0' or '1'
        n = n / 2;
        }
    binaryStr[32] = '\0';
    }
    return binaryStr;
}



