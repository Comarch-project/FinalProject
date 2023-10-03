#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define NUMMEMORY 65536 /* maximum number of words in memory */
#define NUMREGS 8 /* number of machine registers */
#define MAXLINELENGTH 1000

char* decimalToBinary(int);
int binaryToDecimal(char *);
int binaryToDecimalSign(char *);
char* nand(char * ,char * );
char* decimalToBinaryFlex(int ) ;
     
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

    /* read in the entire machine-code file into memory */
    for (int i = 0; i < NUMREGS; i++) {
        state.reg[i] = 0;  // Set all registers to 0
    }
    int insCnt=0;
    int endOfPro = 0;
        for (state.pc=0;endOfPro!=1;state.pc++) {
            if(state.reg[0]!=0) state.reg[0]=0;
            while (getchar() != '\n'); 
            char *bipo;
            bipo=decimalToBinary(state.mem[state.pc]);
            printf("%s\n",bipo);
            insCnt++;
            if((bipo[7]=='0') && (bipo[8]=='0') && (bipo[9]=='0'))//add
            {
                printf("ADD");
                printState(&state);
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

                int DecRs = binaryToDecimal(rs);
                int DecRt = binaryToDecimal(rt);
                int DestRd = binaryToDecimal(rd);
                printf(">>>>>>>>%d + %d = %d\n",state.reg[DecRs], state.reg[DecRt],state.reg[DecRs]+state.reg[DecRt]);
                state.reg[DestRd] = state.reg[DecRs]+state.reg[DecRt];

                continue;
            }
            else if((bipo[7]=='0') && (bipo[8]=='0') && (bipo[9]=='1'))//nand
            {
                printf("NAND");
                printState(&state);
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

                int rsi = state.reg[binaryToDecimal(rs)];
                int rti = state.reg[binaryToDecimal(rt)];
                int rdi = binaryToDecimal(rd);
                char* biOfrsi = decimalToBinaryFlex(rsi);
                printf(">>>>>>>>%s %d\n",biOfrsi, rsi);
                char* biOfrti = decimalToBinaryFlex(rti);
                printf(">>>>>>>>%s %d\n",biOfrti, rti);
                char* res = nand(biOfrsi,biOfrti);
                int lenOfres = strlen(res);
                char *res16b = (char *)malloc((16 + 1) * sizeof(char)); // +1 for the null terminator
                for (int i = 16 - 1; i >= 0; i--) {
                    res16b[i] = '0'; 
                }
                res16b[16] = '\0';
                for (int i = 16 - 1; i >= 0; i--) {
                    if(res[lenOfres-1]=='1') res16b[i] = '1'; 
                    else res16b[i] = '0'; 

                    lenOfres--;
                    if(lenOfres<0) break;
                }
                int saveAtReg = binaryToDecimalSign(res16b);
                printf(">>>>>>>>+%s\n",res16b);
                printf(">>>>>>>>%d\n",saveAtReg);

                state.reg[rdi] = saveAtReg;
                free(biOfrsi);
                free(biOfrti);
                continue;
            }
            else if((bipo[7]=='0') && (bipo[8]=='1') && (bipo[9]=='0'))//lw
            {   
                printf("LW");
                printState(&state);

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

                int regA = state.reg[binaryToDecimal(rs)];
                printf("+++++++++++++++%d\n",regA);
                int regB = binaryToDecimal(rt);
                printf("+++++++++++++++%d\n",regB);
                int offseti = binaryToDecimalSign(offset);
                printf("+++++++++++++++%d\n",offseti);
                int src = regA + offseti;
                printf("+++++++++++++++%d\n",src);
                printf("+++++++++++++++%d\n",state.mem[0]);
                state.reg[regB] = state.mem[src];
                continue;
            }
            else if((bipo[7]=='0') && (bipo[8]=='1') && (bipo[9]=='1'))//sw
              {
                printf("SW");
                printState(&state);
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
                int regB = binaryToDecimal(rt);
                int regA = state.reg[binaryToDecimal(rs)];
                int offseti = binaryToDecimalSign(offset);
                int memaddr = regA+offseti;
                while(state.numMemory<=memaddr){
                    state.numMemory++;
                }
                state.mem[memaddr]=state.reg[regB];
                continue;
              }
            else if((bipo[7]=='1') && (bipo[8]=='0') && (bipo[9]=='0'))//beq
              {
                printf("BEQ");
                printState(&state);
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
                int argB = state.reg[binaryToDecimal(rt)];
                int argA = state.reg[binaryToDecimal(rs)];
                int offseti = binaryToDecimalSign(offset);
                printf("A%d\n",argA);
                printf("B%d\n",argB);
                if(argA == argB){
                    //insert action here
                    state.pc = state.pc+offseti;
                    printf("Equal!Goto %d\n",state.pc);
                }
                continue;

              }
            else if((bipo[7]=='1') && (bipo[8]=='0') && (bipo[9]=='1'))//jalr
             {
                printf("JALR");
                printState(&state);
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
                int regA=binaryToDecimal(rs);
                int regB=binaryToDecimal(rt);
                if(regA==regB){
                    state.reg[regB]=state.pc+1;
                }else{
                    printf("[regA] : %d\n",regA);
                    printf("state.reg[regA] : %d\n",state.reg[regA]);
                    state.reg[regB]=state.pc+1;
                    state.pc=state.reg[regA]-1;
                    printf("PC : %d\n",state.pc);
                } 
                continue;
             }
            else if((bipo[7]=='1') && (bipo[8]=='1') && (bipo[9]=='0'))//halt
             {
                printf("HALT");
                printState(&state);
                endOfPro =1;
                continue;
             }
            else if ((bipo[7]=='1') && (bipo[8]=='1') && (bipo[9]=='1'))//noop
             {
                printf("NOOP");
                printState(&state);
                continue;
             }

        }
    printState(&state);
    printf(">>>Instruction used : %d<<<",insCnt);
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
            if(binaryStr[i]=='0') binaryStr[i] = '1';
            else if(binaryStr[i]=='1') binaryStr[i] = '0'; // Invert bits
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
            else if(binaryString[i]=='1') binaryString[i] = '0';
        }
        int carry = 1; // Initialize carry to 1 for addition
        for (int i = 16-1; i >= 1; i--) {
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

int binaryToDecimalSign32b(char *biString) {
    int decimal = 0;
    char binaryString[32];
    strcpy(binaryString,biString);
    if(binaryString[0]=='1'){
        for (int i = 1; i < 32; i++) {
            if(binaryString[i]=='0') binaryString[i] = '1';
            else if(binaryString[i]=='1') binaryString[i] = '0';
        }
        int carry = 1; // Initialize carry to 1 for addition
        for (int i = 32-1; i >= 1; i--) {
            if (binaryString[i] == '0' && carry == 1) {
                binaryString[i] = '1';
                carry = 0; // No need to carry anymore
            } else if (binaryString[i] == '1' && carry == 1) {
                binaryString[i] = '0'; // Carry over to the next bit
            }
        }
        for (int i = 1; i < 32; i++) {
            if (binaryString[i] == '1') {
                decimal += 1 << (32 - 1 - i);
            } else if (binaryString[i] != '0') {
                printf("Invalid binary input: %c\n", binaryString[i]);
                return -1; // Error: Invalid character in binary string
            }

        }
        decimal = -decimal;
    }else{
        for (int i = 1; i < 32; i++) {
            if (binaryString[i] == '1') {
                decimal += 1 << (32 - 1 - i);
            } else if (binaryString[i] != '0') {
                printf("Invalid binary input: %c\n", binaryString[i]);
                return -1; // Error: Invalid character in binary string
            }
        }
    }
    return decimal;
}

char* decimalToBinaryFlex(int n) {
    char *binaryStr = (char *)malloc((16 + 1) * sizeof(char)); // +1 for the null terminator
    for (int i = 16 - 1; i >= 0; i--) {
    binaryStr[i] = '0'; 
    }
    // Handle negative numbers
    if (n < 0) {
        n = -n;  // Make n positive
        // Convert the absolute value of n to binary and store it in binaryStr
        for (int i = 16 - 1; i >= 0; i--) {
            binaryStr[i] = (n % 2) + '0'; // Convert remainder to character '0' or '1'
            n = n / 2;
        }
        binaryStr[16] = '\0'; // Null-terminate the string
        // Perform two's complement to get the negative binary representation
        for (int i = 0; i < 16; i++) {
            if(binaryStr[i]=='0') binaryStr[i] = '1';
            else if(binaryStr[i]=='1') binaryStr[i] = '0';// Invert bits
        }
        int carry = 1; // Initialize carry to 1 for addition
        for (int i = 16 - 1; i >= 0; i--) {
            if (binaryStr[i] == '0' && carry == 1) {
                binaryStr[i] = '1';
                carry = 0; // No need to carry anymore
            } else if (binaryStr[i] == '1' && carry == 1) {
                binaryStr[i] = '0'; // Carry over to the next bit
            }
        }
    }else{
    // Convert the decimal number to binary and store it in binaryStr
        for (int i = 16 - 1; i >= 0; i--) {
        binaryStr[i] = (n % 2) + '0'; // Convert remainder to character '0' or '1'
        n = n / 2;
        }
    binaryStr[16] = '\0';
    }
    return binaryStr;
}

// char* nand(char *a,char *b){
//     int alen = strlen(a);
//     int blen = strlen(b);
//     int maxstrlen;
//     if(alen>=blen){
//         maxstrlen=alen;
//     }else{
//         maxstrlen=blen;
//     }
//     char *ret = (char *)malloc((maxstrlen + 1) * sizeof(char)); // +1 for the null terminator
//     for (int i = maxstrlen -(maxstrlen-alen)- 1; i >= 0; i--) {
//         ret[i] = '0'; 
//     }
//     char *regA = (char *)malloc((maxstrlen + 1) * sizeof(char)); // +1 for the null terminator
//     for (int i = maxstrlen - 1; i >= 0; i--) {
//     regA[i] = '0'; 
//     }
//     regA[maxstrlen] = '\0';
//     for (int i = maxstrlen - 1; i >= 0; i--) {
//         if(a[alen-1]=='1') regA[i] = '1'; 
//         else regA[i] = '0'; 

//         alen--;
//         if(alen<0) break;
//     }

//     char *regB = (char *)malloc((maxstrlen + 1) * sizeof(char)); // +1 for the null terminator
//     for (int i = maxstrlen - 1; i >= 0; i--) {
//     regB[i] = '0'; 
//     }
//     regB[maxstrlen] = '\0';
//     for (int i = maxstrlen - 1; i >= 0; i--) {
//         if(b[blen-1]=='1') regB[i] = '1'; 
//         else regB[i] = '0'; 

//         blen--;
//         if(blen<0) break;
//     }
//     printf("%s??%s\n",regA,regB);

//     for(int i=0;i<maxstrlen;i++){
//         if(regA[i]=='1'&&regB[i]=='1'){
//             ret[i]='0';
//         }else{
//             ret[i]='1';
//         }
//     }
//     ret[maxstrlen] = '\0';
//     return ret;
// }

char* nand(char *a,char *b){
    int alen = strlen(a);
    int blen = strlen(b);
    int maxstrlen;
    if(alen>=blen){
        maxstrlen=alen;
    }else{
        maxstrlen=blen;
    }
    char *ret = (char *)malloc((16 + 1) * sizeof(char)); // +1 for the null terminator
    for (int i = 16 - 1; i >= 0; i--) {
        ret[i] = '0'; 
    }
    char *regA = (char *)malloc((16 + 1) * sizeof(char)); // +1 for the null terminator
    for (int i = 16 - 1; i >= 0; i--) {
    regA[i] = '0'; 
    }
    regA[16] = '\0';
    for (int i = 16 - 1; i >= 0; i--) {
        if(a[alen-1]=='1') regA[i] = '1'; 
        else regA[i] = '0'; 

        alen--;
        if(alen<0) break;
    }

    char *regB = (char *)malloc((16 + 1) * sizeof(char)); // +1 for the null terminator
    for (int i = 16 - 1; i >= 0; i--) {
    regB[i] = '0'; 
    }
    regB[16] = '\0';
    for (int i = 16 - 1; i >= 0; i--) {
        if(b[blen-1]=='1') regB[i] = '1'; 
        else regB[i] = '0'; 

        blen--;
        if(blen<0) break;
    }
    printf("%s??%s\n",regA,regB);

    for(int i=0;i<16;i++){
        if(regA[i]=='1'&&regB[i]=='1'){
            ret[i]='0';
        }else{
            ret[i]='1';
        }
    }
    ret[16] = '\0';
    return ret;
}