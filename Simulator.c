#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define NUMMEMORY 65536 /* maximum number of words in memory */
#define NUMREGS 8 /* number of machine registers */
#define MAXLINELENGTH 1000

char* decimalToBinary(int);
int binaryToDecimal(char *);
int binaryToDecimalSign(char *);
int combination (int,int);
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
    // Wrong input when calling simulator
    if (argc != 2) {
	printf("error: usage: %s <machine-code file>\n", argv[0]);
	exit(1);
    }
    //open input file with read mode
    filePtr = fopen(argv[1], "r");
    if (filePtr == NULL) {
	printf("error: can't open file %s", argv[1]);
	perror("fopen");
	exit(1);
    }
    /* read in the entire machine-code file into memory and simulate the input*/
    for(state.numMemory  = 0;fgets(line,MAXLINELENGTH,filePtr)!=NULL;state.numMemory++){
        if (sscanf(line, "%d", state.mem+state.numMemory) != 1) {
            printf("error in reading address %d\n", state.numMemory);
            exit(1);
        }else{
            printf("memory[%d]=%d\n",state.numMemory,state.mem[state.numMemory]);  
        }
    }

    /* Initialize all registers to 0 */
    for (int i = 0; i < NUMREGS; i++) {
        state.reg[i] = 0;  // Set all registers to 0
    }
    int insCnt=0;// instruction counter for checking the number of instructio used
    int endOfPro = 0;// variable to check if the program was ended or not(exacute halt will set this variable to 1)
        for (state.pc=0;endOfPro!=1;state.pc++) {// initializeprogram counter to 0 then increase the PC until program was ended (halted)
            if(state.reg[0]!=0) state.reg[0]=0; // fixed register 0 to 0 
            //while (getchar() != '\n'); // uncomment this line to step-by-step check
            char *bipo;
            bipo=decimalToBinary(state.mem[state.pc]);// convert intruction in form of decimal to binary
            insCnt++;
            if((bipo[7]=='0') && (bipo[8]=='0') && (bipo[9]=='0'))// if bipo has an opcode of add
            {
                //printf("ADD");
                printState(&state);
                // define rs
                char rs[4];
                rs[2]=bipo[12];
                rs[1]=bipo[11];
                rs[0]=bipo[10];
                // define rt
                char rt[4];
                rt[2]=bipo[15];
                rt[1]=bipo[14];
                rt[0]=bipo[13];
                // define rd
                char rd[4];
                rd[2]=bipo[31];
                rd[1]=bipo[30];
                rd[0]=bipo[29];

                int DecRs = binaryToDecimal(rs);// covert binary rs to decimal
                int DecRt = binaryToDecimal(rt);// covert binary rt to decimal
                int DestRd = binaryToDecimal(rd);// covert binary rd to decimal
                
                state.reg[DestRd] = state.reg[DecRs]+state.reg[DecRt];// set value of reg[rd] to value of reg[rs] + value of reg[rt]
                continue;
            }
            else if((bipo[7]=='0') && (bipo[8]=='0') && (bipo[9]=='1'))// if bipo has an opcode of nand
            {
                //printf("NAND");
                printState(&state);
                // define rs
                char rs[4];
                rs[2]=bipo[12];
                rs[1]=bipo[11];
                rs[0]=bipo[10];
                // define rt
                char rt[4];
                rt[2]=bipo[15];
                rt[1]=bipo[14];
                rt[0]=bipo[13];
                // define rd
                char rd[4];
                rd[2]=bipo[31];
                rd[1]=bipo[30];
                rd[0]=bipo[29];

                int rsi = state.reg[binaryToDecimal(rs)];// get the value of reg[rs]
                int rti = state.reg[binaryToDecimal(rt)];// get the value of reg[rt]
                int rdi = binaryToDecimal(rd);// covert binary rd to decimal
                char* biOfrsi = decimalToBinaryFlex(rsi);// convert rsi to binary
                char* biOfrti = decimalToBinaryFlex(rti);// convert rti to binary
                char* res = nand(biOfrsi,biOfrti);// perform nand operation between rsi and rti
                int lenOfres = strlen(res);// get the string length of nand output
                // Allocate memory for the binary string
                char *res16b = (char *)malloc((16 + 1) * sizeof(char)); // +1 for the null terminator
                for (int i = 16 - 1; i >= 0; i--) {
                    res16b[i] = '0'; 
                }
                res16b[16] = '\0';
                // convert the nand output(res) to 16 bit binary
                for (int i = 16 - 1; i >= 0; i--) {
                    if(res[lenOfres-1]=='1') res16b[i] = '1'; 
                    else res16b[i] = '0'; 
                    lenOfres--;
                    if(lenOfres<0) break;
                }
                // convert 16bit binary nand to decimal(signed)
                int saveAtReg = binaryToDecimalSign(res16b);
                // set value of reg[rd] to saveAtReg
                state.reg[rdi] = saveAtReg;
                // free the memory
                free(biOfrsi);
                free(biOfrti);
                continue;
            }
            else if((bipo[7]=='0') && (bipo[8]=='1') && (bipo[9]=='0'))// if bipo has an opcode of lw
            {   
                //printf("LW");
                printState(&state);
                // define rs
                char rs[4];
                rs[2]=bipo[12];
                rs[1]=bipo[11];
                rs[0]=bipo[10];
                // define rt
                char rt[4];
                rt[2]=bipo[15];
                rt[1]=bipo[14];
                rt[0]=bipo[13];
                // define offset
                char offset[17];
                for(int i=16;i<32;i++){
                    offset[i-16]=bipo[i];
                }

                int regA = state.reg[binaryToDecimal(rs)];// get the value of reg[rs]
                int regB = binaryToDecimal(rt);// convert binary rt to decimal
                int offseti = binaryToDecimalSign(offset);// convert binary offset to decimal
                int src = regA + offseti;// find the memory address source
                state.reg[regB] = state.mem[src];// set the value of reg[rt] to mem[src]
                continue;
            }
            else if((bipo[7]=='0') && (bipo[8]=='1') && (bipo[9]=='1'))// if bipo has an opcode of sw
              {
                //printf("SW");
                printState(&state);
                // define rs
                char rs[4];
                rs[2]=bipo[12];
                rs[1]=bipo[11];
                rs[0]=bipo[10];
                // define rt
                char rt[4];
                rt[2]=bipo[15];
                rt[1]=bipo[14];
                rt[0]=bipo[13];
                // define offset
                char offset[17];
                for(int i=16;i<32;i++){
                    offset[i-16]=bipo[i];
                }

                int regB = binaryToDecimal(rt);// convert binary rt to decimal
                int regA = state.reg[binaryToDecimal(rs)];// get the value of reg[rs]
                int offseti = binaryToDecimalSign(offset);// convert binary offset to decimal 
                int memaddr = regA+offseti;// find the memory address destination
                // increase state.numMemmory to match with the store address 
                while(state.numMemory<=memaddr){
                    state.numMemory++;
                }
                state.mem[memaddr]=state.reg[regB];// set mem[memaddr] to reg[rt]
                continue;
              }
            else if((bipo[7]=='1') && (bipo[8]=='0') && (bipo[9]=='0'))// if bipo has an opcode of beq
              {
                //printf("BEQ");
                printState(&state);
                // define rs
                char rs[4];
                rs[2]=bipo[12];
                rs[1]=bipo[11];
                rs[0]=bipo[10];
                // define rt
                char rt[4];
                rt[2]=bipo[15];
                rt[1]=bipo[14];
                rt[0]=bipo[13];
                // define offset
                char offset[17];
                for(int i=16;i<32;i++){
                    offset[i-16]=bipo[i];
                }
                int argB = state.reg[binaryToDecimal(rt)];// get value of reg[rt]
                int argA = state.reg[binaryToDecimal(rs)];// get value of reg[rs]
                int offseti = binaryToDecimalSign(offset);// convert binary offset to decimal
                // if rs == rt go to PC + 1 offset (in this case we don't +1 due to the loop already did this for us)
                if(argA == argB){
                    state.pc = state.pc+offseti;
                }
                continue;

              }
            else if((bipo[7]=='1') && (bipo[8]=='0') && (bipo[9]=='1'))// if bipo has an opcode of jalr
             {
                //printf("JALR");
                printState(&state);
                // define rs
                char rs[4];
                rs[2]=bipo[12];
                rs[1]=bipo[11];
                rs[0]=bipo[10];
                // define rt
                char rt[4];
                rt[2]=bipo[15];
                rt[1]=bipo[14];
                rt[0]=bipo[13];

                int regA=binaryToDecimal(rs);// convert binary rs to decimal
                int regB=binaryToDecimal(rt);// convert binary rt to decimal
                if(regA==regB){ // if register rs == register rt then set value of reg[rt] to PC + 1 and goto next pc(in this case the loop will do this for us)
                    state.reg[regB]=state.pc+1; 
                }else{// if register rs != register rt then set value of reg[rt] to PC + 1 and goto the address store in reg[rs](in this case we -1 due to the loop will +1 for us)
                    state.reg[regB]=state.pc+1;
                    state.pc=state.reg[regA]-1;
                } 
                continue;
             }
            else if((bipo[7]=='1') && (bipo[8]=='1') && (bipo[9]=='0'))// if bipo has an opcode of halt
             {
                printState(&state);
                printf("machine halted\n");
                endOfPro =1;// set endOfPro to 1 which will terminate the loop and end the program
                continue;

             }
            else if ((bipo[7]=='1') && (bipo[8]=='1') && (bipo[9]=='1'))// if bipo has an opcode of noop
             {
                //printf("NOOP");
                // do not thing 
                printState(&state);
                continue;
             }

        }

    printf("total of %d instruction executed \n",insCnt); // uncomment to see the instruction used
    printf("final state of machine:\n"); // uncomment to see the instruction used
    printState(&state);

    int n = 7;
    int r = 3;
    int com = combination(n,r);
    //printf(">>>Combination(%d,%d) : %d<<<\n",n,r,com); // change the value of n , r and uncomment to check the combination outcome
    return(0);
}
// printState print the state of program
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
// decimalToBinary convert decimal to 32 bit signed binary
char* decimalToBinary(int n) {
   // Allocate memory for the binary string
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
    }else{// if the input was positive or 0
    // Convert the decimal number to binary and store it in binaryStr
        for (int i = 32 - 1; i >= 0; i--) {
        binaryStr[i] = (n % 2) + '0'; // Convert remainder to character '0' or '1'
        n = n / 2;
        }
    binaryStr[32] = '\0';
    }
    return binaryStr;
}
// binaryToDecimal convert 3bit binary to decimal(unsigned)
int binaryToDecimal(char *binaryString) {
    int decimal = 0;
    for (int i = 0; i < 3; i++) {// convert binary to decimal bit-by-bit
        if (binaryString[i] == '1') {// if the binary at index i was 1
            decimal += 1 << (3 - 1 - i);// each of index that is 1 in binary will add the result with 1 shift left by (3-1-index i)
        } else if (binaryString[i] != '0') {
            printf("Invalid binary input: %c\n", binaryString[i]);
            return -1; // Error: Invalid character in binary string
        }
    }
    return decimal;
}
// binaryToDecimalSign convert 16bit binary to decimal(signed)
int binaryToDecimalSign(char *biString) {
    int decimal = 0;
    char binaryString[16];
    strcpy(binaryString,biString);

    if(binaryString[0]=='1'){// if the sign bit was 1 (negative number)
    // Perform two's complement to get the negative binary representation
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
        for (int i = 1; i < 16; i++) {// convert binary to decimal bit-by-bit
            if (binaryString[i] == '1') {// if the binary at index i was 1
                decimal += 1 << (16 - 1 - i);// each of index that is 1 in binary will add the result with 1 shift left by (16-1-index i)
            } else if (binaryString[i] != '0') {
                printf("Invalid binary input: %c\n", binaryString[i]);
                return -1; // Error: Invalid character in binary string
            }

        }
        // make result negative before return
        decimal = -decimal;
    }else{// if the sign bit was 0 (0 or positive number)
        for (int i = 1; i < 16; i++) {// convert binary to decimal bit-by-bit
            if (binaryString[i] == '1') {// if the binary at index i was 1
                decimal += 1 << (16 - 1 - i);// each of index that is 1 in binary will add the result with 1 shift left by (16-1-index i)
            } else if (binaryString[i] != '0') {
                printf("Invalid binary input: %c\n", binaryString[i]);
                return -1; // Error: Invalid character in binary string
            }
        }
    }
    return decimal;
}
// binaryToDecimalSign32b convert 32bit binary to decimal(signed)
int binaryToDecimalSign32b(char *biString) {
    int decimal = 0;
    char binaryString[32];
    strcpy(binaryString,biString);
    if(binaryString[0]=='1'){// if the sign bit was 1 (negative number)
    // Perform two's complement to get the negative binary representation
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
        for (int i = 1; i < 32; i++) {// convert binary to decimal bit-by-bit
            if (binaryString[i] == '1') {// if the binary at index i was 1
                decimal += 1 << (32 - 1 - i);// each of index that is 1 in binary will add the result with 1 shift left by (32-1-index i)
            } else if (binaryString[i] != '0') {
                printf("Invalid binary input: %c\n", binaryString[i]);
                return -1; // Error: Invalid character in binary string
            }

        }
        // make result negative before return
        decimal = -decimal;
    }else{
        for (int i = 1; i < 32; i++) {// convert binary to decimal bit-by-bit
            if (binaryString[i] == '1') {// if the binary at index i was 1
                decimal += 1 << (32 - 1 - i);// each of index that is 1 in binary will add the result with 1 shift left by (32-1-index i)
            } else if (binaryString[i] != '0') {
                printf("Invalid binary input: %c\n", binaryString[i]);
                return -1; // Error: Invalid character in binary string
            }
        }
    }
    return decimal;
}
// decimalToBinary convert decimal to 16 bit signed binary
char* decimalToBinaryFlex(int n) {
    // Allocate memory for the binary string
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
// nand perform nand between two binary input
char* nand(char *a,char *b){
    int alen = strlen(a);// find length of string a (previously used if the length of the input is not the same)
    int blen = strlen(b);// find length of string b (previously used if the length of the input is not the same)
     // Allocate memory for the ret
    char *ret = (char *)malloc((16 + 1) * sizeof(char)); // +1 for the null terminator
    for (int i = 16 - 1; i >= 0; i--) {
        ret[i] = '0'; 
    }
     // Allocate memory for the regA
    char *regA = (char *)malloc((16 + 1) * sizeof(char)); // +1 for the null terminator
    for (int i = 16 - 1; i >= 0; i--) {
    regA[i] = '0'; 
    }
    regA[16] = '\0';// Null-terminate the string
    //  initialize the value of regA (mostly use if the length of the input is not the same)
    for (int i = 16 - 1; i >= 0; i--) {
        if(a[alen-1]=='1') regA[i] = '1'; 
        else regA[i] = '0'; 
        alen--;
        if(alen<0) break;
    }
     // Allocate memory for the regB
    char *regB = (char *)malloc((16 + 1) * sizeof(char)); // +1 for the null terminator
    for (int i = 16 - 1; i >= 0; i--) {
    regB[i] = '0'; 
    }
    regB[16] = '\0';// Null-terminate the string
    //  initialize the value of regA (mostly use if the length of the input is not the same)
    for (int i = 16 - 1; i >= 0; i--) {
        if(b[blen-1]=='1') regB[i] = '1'; 
        else regB[i] = '0'; 
        blen--;
        if(blen<0) break;
    }
    //  nand each bit in regA and regB and put the result in ret for every index i
    for(int i=0;i<16;i++){
        if(regA[i]=='1'&&regB[i]=='1'){
            ret[i]='0';
        }else{
            ret[i]='1';
        }
    }
    ret[16] = '\0';// Null-terminate the string
    return ret;
}
//combination function that have been given use to perform a combination function use to check the out come of function
int combination(int n, int r)
        {

               if(r == 0 || n == r)

                   return(1);

               else

                   return(combination(n-1,r) + combination(n-1, r-1));

        }