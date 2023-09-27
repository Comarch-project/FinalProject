#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define NUMMEMORY 65536 /* maximum number of words in memory */
#define NUMREGS 8 /* number of machine registers */
#define MAXLINELENGTH 1000

char* decimalToBinary(int);
int binaryToDecimalSign(char *);

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
    for(state.numMemory =0;fgets(line,MAXLINELENGTH,filePtr)!=NULL;state.numMemory++){
        if (sscanf(line, "%d", state.mem+state.numMemory) != 1) {
            printf("error in reading address %d\n", state.numMemory);
            exit(1);
        }else{
            printf("memory[%d]=%d\n",state.numMemory,state.mem[state.numMemory]);  
        }
    }
    /* read in file to check where is halt and insert .fill value*/
    //state.numMemory = address
    //state.mem[state.numMemory] = machine code
    int keyvalpt =0;
    int hlted =0;
    for(int i=0;i<state.numMemory;i++){
            char *bipo;
            bipo=decimalToBinary(state.mem[i]);
            if((bipo[7]=='1')&&bipo[8]=='1'&&(bipo[9]=='0')){
                hlted =1;
                continue;
            }
            if(hlted == 1){
                keyValueList[keyvalpt].address = i;
                keyValueList[keyvalpt].value =  state.mem[i];
                keyvalpt++;
            }
        
    }

    // for(int i=0;i<keyvalpt;i++){
    //     printf("%d\n",keyValueList[i].address);
    //     printf("%d\n",keyValueList[i].value);
    // }

    /* read in the entire machine-code file into memory */
    for (int i = 0; i < NUMREGS; i++) {
        state.reg[i] = 0;  // Set all registers to 0
    }

    int endOfPro = 0;
        for (state.pc=0;endOfPro!=1;state.pc++) {
            char *bipo;
            bipo=decimalToBinary(state.mem[state.pc]);
            printf("%s\n",bipo);
            if((bipo[7]=='0') && (bipo[8]=='0') && (bipo[9]=='0'))//add
            {
                printState(&state);
                char rs[4];
                rs[2]=bipo[12];
                rs[1]=bipo[11];
                rs[0]=bipo[10];
                char rt[4];
                rt[2]=bipo[15];
                rt[1]=bipo[14];
                rt[0]=bipo[13];
                char bit15to3[14];
                for (int i = 0 ; i>13 ; i++){
                    bit15to3[i] = '0';
                }
                char rd[4];
                rd[2]=bipo[31];
                rd[1]=bipo[30];
                rd[0]=bipo[29];
                
                continue;
            }
            else if((bipo[7]=='0') && (bipo[8]=='0') && (bipo[9]=='1'))//nand
            {
                // char rs[4];
                // rs[2]=bipo[12];
                // rs[1]=bipo[11];
                // rs[0]=bipo[10];
                // char rt[4];
                // rt[2]=bipo[15];
                // rt[1]=bipo[14];
                // rt[0]=bipo[13];
                // char rd[4];
                // rd[2]=bipo[31];
                // rd[1]=bipo[30];
                // rd[0]=bipo[29];
                // for(int i=0; i<3; i++)
                // { 
                //     if(rs[i]=='1' && rt[i]=='1'){
                //     rd[i]='0';
                //     }else rd[i]='1';
                // }
                continue;
            }
            else if((bipo[7]=='0') && (bipo[8]=='1') && (bipo[9]=='0'))//lw
            {   
                printState(&state);
                printf("lw\n");
                char rs[4];
                rs[2]=bipo[12];
                rs[1]=bipo[11];
                rs[0]=bipo[10];
                
                char rt[4];
                rt[2]=bipo[15];
                rt[1]=bipo[14];
                rt[0]=bipo[13];

                char offset[17];
                for(int i=16;i<32;i++){
                    offset[i-16]=bipo[i];
                }

                int dest = binaryToDecimal(rt);
                int rss = state.reg[binaryToDecimal(rs)];
                int offseti = binaryToDecimalSign(offset);
                int src = rss + offseti;
                state.reg[dest] = state.mem[src];
                continue;
            }
            //  else if((bipo[7]=='0') && (bipo[8]=='1') && (bipo[9]=='1'))//sw
            //  {
            //     break;
            //  }
            //  else if((bipo[7]=='1') && (bipo[8]=='0') && (bipo[9]=='0'))//beq
            //  {
            //     break;
            //  }
            //  else if((bipo[7]=='1') && (bipo[8]=='0') && (bipo[9]=='1'))//jalr
            //  {
            //     break;
            //  }
            else if((bipo[7]=='1') && (bipo[8]=='1') && (bipo[9]=='0'))//halt
             {
                printState(&state);
                endOfPro =1;
                continue;
             }
             else if ((bipo[7]=='1') && (bipo[8]=='1') && (bipo[9]=='1'))//noop
             {
                printState(&state);
                continue;
             }

        }

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

int binaryToDecimal(char *binaryString) {
    int decimal = 0;
    for (int i = 0; i < 3; i++) {
        if (binaryString[i] == '1') {
            decimal += 1 << (3 - 1 - i);
        } else if (binaryString[i] != '0') {
            printf("Invalid binary input: %c\n", binaryString[i]);
            return -1; // Error: Invalid character in binary string
        }
    }
    return decimal;
}

int binaryToDecimalSign(char *biString) {
    int decimal = 0;
    char binaryString[16];
    strcpy(binaryString,biString);
    if(binaryString[0]=='1'){
        for (int i = 1; i < 16; i++) {
            if(binaryString[i]=='0') binaryString[i] = '1';
            if(binaryString[i]=='1') binaryString[i] = '0';
        }
        int carry = 1; // Initialize carry to 1 for addition
        for (int i = 15 - 1; i >= 1; i--) {
            if (binaryString[i] == '0' && carry == 1) {
                binaryString[i] = '1';
                carry = 0; // No need to carry anymore
            } else if (binaryString[i] == '1' && carry == 1) {
                binaryString[i] = '0'; // Carry over to the next bit
            }
        }
        
        for (int i = 1; i < 16; i++) {
            if (binaryString[i] == '1') {
                decimal += 1 << (16 - 1 - i);
            } else if (binaryString[i] != '0') {
                printf("Invalid binary input: %c\n", binaryString[i]);
                return -1; // Error: Invalid character in binary string
            }

        }
        decimal = -decimal;
    }else{
        for (int i = 1; i < 16; i++) {
            if (binaryString[i] == '1') {
                decimal += 1 << (16 - 1 - i);
            } else if (binaryString[i] != '0') {
                printf("Invalid binary input: %c\n", binaryString[i]);
                return -1; // Error: Invalid character in binary string
            }
        }
    }
    return decimal;
}

int isNumber(char *string)
{
    /* return 1 if string is a number */
    int i;
    return( (sscanf(string, "%d", &i)) == 1);
}


