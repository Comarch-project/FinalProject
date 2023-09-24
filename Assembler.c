
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAXLINELENGTH 1000

int readAndParse(FILE *, char *, char *, char *, char *, char *);
int isNumber(char *);
char* decToBiSign16b(char *);
char* decToBiUnsign3b(char *);
struct KeyValuePair {
    char key[50];
    int value;
};
int main(int argc, char *argv[]) //argv = argument vector, argc = argument count 
{
    char *inFileString, *outFileString;
    FILE *inFilePtr, *outFilePtr;
    char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH],
            arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];
    char binaryOp[4];
    char binaryMachCode[33] = "00000000000000000000000000000000";

        // Define the maximum number of key-value pairs in the list
    int maxPairs = 32;
    // Create an array of KeyValuePair structs to store the data
    struct KeyValuePair keyValueList[maxPairs];

    if (argc != 3) { 
        printf("error: usage: %s <assembly-code-file> <machine-code-file>\n",
            argv[0]);
        exit(1);
    }

    inFileString = argv[1];
    outFileString = argv[2];

    inFilePtr = fopen(inFileString, "r");                        //เปิดไฟล์
    if (inFilePtr == NULL) {                                     // ถ้า ไฟล์ว่าง
        printf("error in opening %s\n", inFileString);
        exit(1);
    }
    outFilePtr = fopen(outFileString, "w");
    if (outFilePtr == NULL) {
        printf("error in opening %s\n", outFileString);
        exit(1);
    }

    // /* here is an example for how to use readAndParse to read a line from
    //     inFilePtr */
    // if (! readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2) ) {
    //     /* reached end of file */
    // }

    // /* this is how to rewind the file ptr so that you start reading from the
    //     beginning of the file */
    // rewind(inFilePtr);

    /* after doing a readAndParse, you may want to do the following to test the
        opcode */
    int linecnt = 0;
    int keyvalpt = 0;
    while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)){
        if(!strcmp(opcode,".fill")){
            printf("%s \n",opcode);
            strcpy(keyValueList[keyvalpt].key, label);
            if(isNumber(arg0)){
                keyValueList[keyvalpt].value = atoi(arg0);
            }else{
                int i;
                for (i = 0; i < maxPairs; i++) {
                    if (!strcmp(keyValueList[i].key, arg0)) {
                        keyValueList[keyvalpt].value = keyValueList[i].value;
                        break;
                    }
                }
            }
            keyvalpt++;
        }else if(strcmp(label,"")){
            printf("%s \n",label);
            strcpy(keyValueList[keyvalpt].key, label);
            keyValueList[keyvalpt].value = linecnt;
            keyvalpt++;
        }
        linecnt++;
    }
    for (int i = 0; i < keyvalpt; i++) {
        printf("Value for key '%s' is %d\n", keyValueList[i].key, keyValueList[i].value);
    }
    rewind(inFilePtr);

    linecnt = 0;
    while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)) {
        printf("Label: %s, Opcode: %s, Arg0: %s, Arg1: %s, Arg2: %s\n", label, opcode, arg0, arg1, arg2);
        if (!strcmp(opcode, "add")) {
            strcpy(binaryOp,"000");
            if(!isNumber(arg2)){
                
            }
            char *biArg0 = decToBiSign16b(arg1);
            char *biArg1 = decToBiSign16b(arg1);
            char *biArg2 = decToBiUnsign3b(arg1);
            binaryMachCode[11]=biArg1[0];
            binaryMachCode[12]=biArg2[1];
            binaryMachCode[13]=biArg1[2];
        }else if(!strcmp(opcode, "nand")){
            strcpy(binaryOp,"001");
            
            
        }else if(!strcmp(opcode, "lw")){
            strcpy(binaryOp,"010");
            
            
        }else if(!strcmp(opcode, "sw")){
            strcpy(binaryOp,"011");
            
            
        }else if(!strcmp(opcode, "beq")){
            strcpy(binaryOp,"100");
        }else if(!strcmp(opcode, "jalr")){
            strcpy(binaryOp,"101");
            binaryMachCode[11]=binaryOp[2];
            binaryMachCode[10]=binaryOp[1];
            binaryMachCode[9]=binaryOp[0];
        }else if(!strcmp(opcode, "halt")){
            strcpy(binaryOp,"110");
            binaryMachCode[11]=binaryOp[2];
            binaryMachCode[10]=binaryOp[1];
            binaryMachCode[9]=binaryOp[0];
        }else if(!strcmp(opcode, "noop")){
            strcpy(binaryOp,"111");
            binaryMachCode[11]=binaryOp[2];
            binaryMachCode[10]=binaryOp[1];
            binaryMachCode[9]=binaryOp[0];
        }else if(!strcmp(opcode, ".fill")){
            
        }
        //char *biResult = decToBiUnsign(arg2);
        char *biResult = decToBiSign16b(arg2);
        printf("decToBi: %s\n", biResult);
        free(biResult); // Free the allocated memosry
        printf("binaryOp: %s\n",binaryOp);
        printf("address : %d\n",linecnt);
        printf("%s \n",binaryMachCode);
        biToHex(binaryMachCode);
        printf("--------------------------------------------\n");
        strcpy(binaryMachCode, "00000000000000000000000000000000");
        linecnt++;
    }
    return(0);
}

/*
 * Read and parse a line of the assembly-language file.  Fields are returned
 * in label, opcode, arg0, arg1, arg2 (these strings must have memory already
 * allocated to them).
 *
 * Return values:
 *     0 if reached end of file
 *     1 if all went well
 *
 * exit(1) if line is too long.
 */


int readAndParse(FILE *inFilePtr, char *label, char *opcode, char *arg0,
    char *arg1, char *arg2)
{
    char line[MAXLINELENGTH];
    char *ptr = line;

    /* delete prior values */
    label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';

    /* read the line from the assembly-language file */
    if (fgets(line, MAXLINELENGTH, inFilePtr) == NULL) {
	/* reached end of file */
        return(0);
    }

    /* check for line too long (by looking for a \n) */
    if (strchr(line, '\n') == NULL) {
        /* line too long */
	printf("error: line too long\n");
	exit(1);
    }

    /* is there a label? */
    ptr = line;
    if (sscanf(ptr, "%[^\t\n ]", label)) {
	/* successfully read label; advance pointer over the label */
        ptr += strlen(label);
    }

    /*
     * Parse the rest of the line.  Would be nice to have real regular
     * expressions, but scanf will suffice.
     */
    sscanf(ptr, "%*[\t\n ]%[^\t\n ]%*[\t\n ]%[^\t\n ]%*[\t\n ]%[^\t\n ]%*[\t\n ]%[^\t\n ]",
        opcode, arg0, arg1, arg2);
    return(1);
}

int isNumber(char *string)
{
    /* return 1 if string is a number */
    int i;
    return( (sscanf(string, "%d", &i)) == 1);
}

void biToHex(char bin[]){
    // Convert binary to integer using strtol
    unsigned int decimal = strtol(bin, NULL, 2);
    printf("Hexadecimal: %X\n", decimal);         //write to text here
}

char* decToBiSign16b(char *string) {
    long int n =atol(string);
    // Determine the number of bits dynamically
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
            binaryStr[i] = (binaryStr[i] == '0') ? '1' : '0'; // Invert bits
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


char* decToBiUnsign3b(char *string) { 
    long int n = atol(string); // Use atol to convert string to long int
    
    // Allocate memory for the binary string
    char *binaryStr = (char *)malloc((3+ 1) * sizeof(char)); // +1 for the null terminator
    for (int i = 3 - 1; i >= 0; i--) {
        binaryStr[i] = '0'; 
    }
    // Convert the decimal number to binary and store it in binaryStr
    for (int i = 3 - 1; i >= 0; i--) {
        binaryStr[i] = (n % 2) + '0'; // Convert remainder to character '0' or '1'
        n = n / 2;
    }

    binaryStr[3] = '\0'; // Null-terminate the string

    return binaryStr;
}
