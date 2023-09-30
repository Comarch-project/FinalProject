
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAXLINELENGTH 1000

int readAndParse(FILE *, char *, char *, char *, char *, char *);
int isNumber(char *);
char* decToBiSign16b(char *,int);
char* decToBiUnsign3b(char *);
char* decToBiSign32b(char *);
int biToHex(char[] ,FILE *,FILE *,int );
int biToHex4fill(char[] ,FILE *,FILE *,int ,int );

struct KeyValuePair {
    char type[50];
    char key[50];
    char value[50];
    char address[50];
};
int main(int argc, char *argv[]) //argv = argument vector, argc = argument count 
{
    char *inFileString, *outFileString,*outFileStringSim;
    FILE *inFilePtr, *outFilePtr,*outFilePtrSim;
    char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH],
            arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];
    char binaryOp[4];
    // Set out but binary to 32 bit
    char binaryMachCode[33] = "00000000000000000000000000000000";

    // Define the maximum number of key-value pairs in the list
    int maxPairs = 32;
    // Create an array of KeyValuePair structs to store the data
    struct KeyValuePair keyValueList[maxPairs];
    // Wrong input when calling assembler
    if (argc != 4) { 
        printf("error: usage: %s <assembly-code-file> <machine-code-file> <machine-code-simulator-file>\n",
            argv[0]);
        exit(1);
    }
    // Set input file and output file
    inFileString = argv[1];
    outFileString = argv[2];
    outFileStringSim = argv[3];

    inFilePtr = fopen(inFileString, "r");                       
    if (inFilePtr == NULL) {                                     
        printf("error in opening %s\n", inFileString);
        exit(1);
    }
    outFilePtr = fopen(outFileString, "w");
    if (outFilePtr == NULL) {
        printf("error in opening %s\n", outFileString);
        exit(1);
    }

    outFilePtrSim = fopen(outFileStringSim, "w");
    if (outFilePtrSim == NULL) {
        printf("error in opening %s\n", outFileStringSim);
        exit(1);
    }
     // Read all line in file to indicate false instruction
    int linecnt = 0;
    while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)){
        if(!strcmp(opcode,".fill")||!strcmp(opcode,"add")||!strcmp(opcode,"nand")||!strcmp(opcode,"lw")||!strcmp(opcode,"sw")||!strcmp(opcode,"beq")||!strcmp(opcode,"jalr")||!strcmp(opcode,"halt")||!strcmp(opcode,"noop")){
            linecnt++;
        }else{
            printf("Undefine instruction/opcode at address %d (line %d)",linecnt,linecnt+1);
            exit(1) ;
        }
    }
    rewind(inFilePtr);
    // Initiate value of each lable 
    linecnt = 0;// To indicate address
    int keyvalpt = 0;// To indicate current size of keyValue
    while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)){
        if(!strcmp(opcode,".fill")){
            strcpy(keyValueList[keyvalpt].type, opcode);
            strcpy(keyValueList[keyvalpt].key, label);
            if(isNumber(arg0)){
                strcpy(keyValueList[keyvalpt].value, arg0) ;
                strcpy(keyValueList[keyvalpt].address, itoa(linecnt,keyValueList[keyvalpt].address,50));
            }else{
                int i;
                for (i = 0; i < maxPairs; i++) {
                    if (!strcmp(keyValueList[i].key, arg0)) {
                        strcpy(keyValueList[keyvalpt].value, keyValueList[i].value);
                        strcpy(keyValueList[keyvalpt].address, itoa(linecnt,keyValueList[keyvalpt].address,50));
                        break;
                    }
                }
            }
            keyvalpt++;
        }else if(strcmp(label,"")){
            for (int i = 0; i < keyvalpt; i++) {
                if(!strcmp(label, keyValueList[i].key)){
                    printf("Duplicated lable at line %d",linecnt+1);
                    exit(1) ;
                }
            }
            strcpy(keyValueList[keyvalpt].type, opcode);
            strcpy(keyValueList[keyvalpt].key, label);
            strcpy(keyValueList[keyvalpt].value, itoa(linecnt,keyValueList[keyvalpt].value,50));
            strcpy(keyValueList[keyvalpt].address, itoa(linecnt,keyValueList[keyvalpt].address,50));
            keyvalpt++;
        }
        linecnt++;
    }
    for (int i = 0; i < keyvalpt; i++) {
        printf("> %s %s %s\n",keyValueList[i].key,keyValueList[i].value,keyValueList[i].address);
    }
    rewind(inFilePtr);

    linecnt = 0;
    while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)) {
        printf("Label: %s, Opcode: %s, Arg0: %s, Arg1: %s, Arg2: %s\n", label, opcode, arg0, arg1, arg2);
        if (!strcmp(opcode, "add")||!strcmp(opcode, "nand")) {
            char *biRS,*biRT,*biRD;
            //Add opcode
            if(!strcmp(opcode, "add")) strcpy(binaryOp,"000");
            if(!strcmp(opcode, "nand")) strcpy(binaryOp,"001");
            binaryMachCode[7]=binaryOp[0];
            binaryMachCode[8]=binaryOp[1];
            binaryMachCode[9]=binaryOp[2];
            //Add rs
            if(isNumber(arg0)){
                biRS = decToBiUnsign3b(arg0);
            }else{
                for (int i = 0; i <= keyvalpt; i++) {
                    if(!strcmp(arg0, keyValueList[i].key)){
                        biRS = decToBiUnsign3b(keyValueList[i].value);
                        break;
                    }else if(i == keyvalpt){
                        printf("Label %s was undefined",arg0);
                        exit(1);
                    }
                }
            }
            binaryMachCode[10]=biRS[0];
            binaryMachCode[11]=biRS[1];
            binaryMachCode[12]=biRS[2];
            //Add rt
            if(isNumber(arg1)){
                biRT = decToBiUnsign3b(arg1);
            }else{
                for (int i = 0; i <= keyvalpt; i++) {
                    if(!strcmp(arg1, keyValueList[i].key)){
                        biRT = decToBiUnsign3b(keyValueList[i].value);
                        break;
                    }else if(i == keyvalpt){
                        printf("Label %s was undefined",arg1);
                        exit(1);
                    }
                }
            }
            binaryMachCode[13]=biRT[0];
            binaryMachCode[14]=biRT[1];
            binaryMachCode[15]=biRT[2];
            //Add rd
            if(isNumber(arg2)){
                biRD = decToBiUnsign3b(arg2);
            }else{
                for (int i = 0; i <= keyvalpt; i++) {
                    if(!strcmp(arg2, keyValueList[i].key)){
                        biRD = decToBiUnsign3b(keyValueList[i].value);
                        break;
                    }else if(i == keyvalpt){
                        printf("Label %s was undefined",arg2);
                        exit(1);
                    }
                }
            }
            binaryMachCode[29]=biRD[0];
            binaryMachCode[30]=biRD[1];
            binaryMachCode[31]=biRD[2];

            free(biRS);
            free(biRT);
            free(biRD);
        }else if(!strcmp(opcode, "lw")||!strcmp(opcode, "sw")){
            char *biRS,*biRT,*biOff;
            if(!strcmp(opcode, "lw"))strcpy(binaryOp,"010");
            if(!strcmp(opcode, "sw"))strcpy(binaryOp,"011");
            binaryMachCode[7]=binaryOp[0];
            binaryMachCode[8]=binaryOp[1];
            binaryMachCode[9]=binaryOp[2];
            //Add rs
            if(isNumber(arg0)){
                biRS = decToBiUnsign3b(arg0);
            }else{
                for (int i = 0; i <= keyvalpt; i++) {
                    if(!strcmp(arg0, keyValueList[i].key)){
                        biRS = decToBiUnsign3b(keyValueList[i].value);
                        break;
                    }else if(i == keyvalpt){
                        printf("Label %s was undefined",arg0);
                        exit(1);
                    }
                }
            }
            binaryMachCode[10]=biRS[0];
            binaryMachCode[11]=biRS[1];
            binaryMachCode[12]=biRS[2];
            //Add rd
            if(isNumber(arg1)){
                biRT = decToBiUnsign3b(arg1);
            }else{
                for (int i = 0; i <= keyvalpt; i++) {
                    if(!strcmp(arg1, keyValueList[i].key)){
                        biRT = decToBiUnsign3b(keyValueList[i].value);
                        break;
                    }else if(i == keyvalpt){
                        printf("Label %s was undefined",arg1);
                        exit(1);
                    }
                }
            }
            binaryMachCode[13]=biRT[0];
            binaryMachCode[14]=biRT[1];
            binaryMachCode[15]=biRT[2];
            //Add offsetField
            if(isNumber(arg2)){
                biOff = decToBiSign16b(arg2,linecnt);
            }else{
                for (int i = 0; i <= keyvalpt; i++) {
                    if(!strcmp(arg2, keyValueList[i].key)){
                        biOff = decToBiSign16b(keyValueList[i].address,linecnt);
                        break;
                    }else if(i == keyvalpt){
                        printf("Label %s was undefined",arg2);
                        exit(1);
                    }
                }
            }
            for(int i=16;i<32;i++){
                binaryMachCode[i]=biOff[i-16];
            }

            free(biRS);
            free(biRT);
            free(biOff);
        }else if(!strcmp(opcode, "beq")){
            char *biRS,*biRT,*biOff;
            strcpy(binaryOp,"100");
            binaryMachCode[7]=binaryOp[0];
            binaryMachCode[8]=binaryOp[1];
            binaryMachCode[9]=binaryOp[2];
            //Add rs
            if(isNumber(arg0)){
                biRS = decToBiUnsign3b(arg0);
            }else{
                for (int i = 0; i <= keyvalpt; i++) {
                    if(!strcmp(arg0, keyValueList[i].key)){
                        biRS = decToBiUnsign3b(keyValueList[i].value);
                        break;
                    }else if(i == keyvalpt){
                        printf("Label %s was undefined",arg0);
                        exit(1);
                    }
                }
            }
            binaryMachCode[10]=biRS[0];
            binaryMachCode[11]=biRS[1];
            binaryMachCode[12]=biRS[2];
            //Add rd
            if(isNumber(arg1)){
                biRT = decToBiUnsign3b(arg1);
            }else{
                for (int i = 0; i <= keyvalpt; i++) {
                    if(!strcmp(arg1, keyValueList[i].key)){
                        biRT = decToBiUnsign3b(keyValueList[i].value);
                        break;
                    }else if(i == keyvalpt){
                        printf("Label %s was undefined",arg1);
                        exit(1);
                    }
                }
            }
            binaryMachCode[13]=biRT[0];
            binaryMachCode[14]=biRT[1];
            binaryMachCode[15]=biRT[2];
            //Add offsetField
            if(isNumber(arg2)){
                biOff = decToBiSign16b(arg2,linecnt);
            }else{
                for (int i = 0; i < keyvalpt; i++) {
                    if(!strcmp(arg2, keyValueList[i].key)){
                        int offseti = (-linecnt)-1+atoi(keyValueList[i].address);
                        char offsetc[32];
                        sprintf(offsetc, "%d", offseti);
                        printf("%s\n",offsetc);
                        biOff = decToBiSign16b(offsetc,linecnt);
                        printf("%s\n",biOff);
                        break;
                    }else if(i == keyvalpt){
                        printf("Label %s was undefined",arg2);
                        exit(1);
                    }
                }
            }
            for(int i=16;i<32;i++){
                binaryMachCode[i]=biOff[i-16];
            }

            free(biRS);
            free(biRT);
            free(biOff);
            
        }else if(!strcmp(opcode, "jalr")){
            char *biRS,*biRT;
            strcpy(binaryOp,"101");
            binaryMachCode[7]=binaryOp[2];
            binaryMachCode[8]=binaryOp[1];
            binaryMachCode[9]=binaryOp[0];
            //Add rs
            if(isNumber(arg0)){
                biRS = decToBiUnsign3b(arg0);
            }else{
                for (int i = 0; i <= keyvalpt; i++) {
                    if(!strcmp(arg0, keyValueList[i].key)){
                        biRS = decToBiUnsign3b(keyValueList[i].address);
                        break;
                    }else if(i == keyvalpt){
                        printf("Label %s was undefined",arg0);
                        exit(1);
                    }
                }
            }
            binaryMachCode[10]=biRS[0];
            binaryMachCode[11]=biRS[1];
            binaryMachCode[12]=biRS[2];
            //Add rd
            if(isNumber(arg1)){
                biRT = decToBiUnsign3b(arg1);
            }else{
                for (int i = 0; i <= keyvalpt; i++) {
                    if(!strcmp(arg1, keyValueList[i].key)){
                        biRT = decToBiUnsign3b(keyValueList[i].value);
                        break;
                    }else if(i == keyvalpt){
                        printf("Label %s was undefined",arg1);
                        exit(1);
                    }
                }
            }
            binaryMachCode[13]=biRT[0];
            binaryMachCode[14]=biRT[1];
            binaryMachCode[15]=biRT[2];
            
            free(biRS);
            free(biRT);
        }else if(!strcmp(opcode, "halt")){
            strcpy(binaryOp,"110");
            binaryMachCode[7]=binaryOp[0];
            binaryMachCode[8]=binaryOp[1];
            binaryMachCode[9]=binaryOp[2];
        }else if(!strcmp(opcode, "noop")){
            strcpy(binaryOp,"111");
            binaryMachCode[7]=binaryOp[0];
            binaryMachCode[8]=binaryOp[1];
            binaryMachCode[9]=binaryOp[2];
        }else if(!strcmp(opcode, ".fill")){
            char *biCode;
            int value;
            if(isNumber(arg0)){
                biCode=decToBiSign32b(arg0);
                value = atoi(arg0);
            }else{
                for (int i = 0; i <= keyvalpt; i++) {
                    if(!strcmp(arg0, keyValueList[i].key)){
                        biCode=decToBiSign32b(keyValueList[i].value);
                        value = atoi(keyValueList[i].value);
                        break;
                    }else if(i == keyvalpt){
                        printf("Label %s was undefined",arg0);
                        exit(1);
                    }
                }
            }
            printf("biCode: %s\n",biCode);
            strcpy(binaryMachCode,biCode);
            biToHex4fill(biCode,outFilePtr,outFilePtrSim,linecnt,value);
            free(biCode);
        }
        //char *biResult = decToBiUnsign(arg2);
        // char *biResult = decToBiSign16b(arg2);
        // free(biResult); // Free the allocated memosry
        printf("binaryOp: %s\n",binaryOp);
        printf("address : %d\n",linecnt);
        printf("biMachCode: %s \n",binaryMachCode);
        if(strcmp(opcode, ".fill")) biToHex(binaryMachCode,outFilePtr,outFilePtrSim,linecnt);
        printf("--------------------------------------------\n");
        strcpy(binaryMachCode, "00000000000000000000000000000000");
        linecnt++;
    }
    fclose(outFilePtr);
    exit(0);
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

int biToHex(char bin[],FILE *str,FILE *strSim,int addr){
    // Convert binary to integer using strtol
    unsigned int decimal = strtoll(bin, NULL, 2);
    fprintf(str,"(address %d): %d (hex 0x%X)\n", addr,decimal,decimal);
    fprintf(strSim,"%d\n",decimal);
    printf("Hexadecimal: %X\n", decimal);         //write to text here
    return(1);
}

int biToHex4fill(char bin[],FILE *str,FILE *strSim,int addr,int val){
    // Convert binary to integer using strtol
    unsigned int decimal = strtoll(bin, NULL, 2);
    fprintf(str,"(address %d): %d (hex 0x%X)\n", addr,val,decimal);\
    fprintf(strSim,"%d\n", val);
    printf("Hexadecimal: %X\n", decimal);         //write to text here
    return(1);
}

char* decToBiSign16b(char *string,int linecnt) {
    long int n =atol(string);
    if(n>32768||n<(-32768)){
        printf("Wrong offset input at line %d",linecnt+1);
        exit(1) ;
    }
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
            if(binaryStr[i]=='0') binaryStr[i] = '1';
            else if(binaryStr[i]=='1') binaryStr[i] = '0';
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


char* decToBiSign32b(char *string) {  
    long int n =atol(string);
    // Determine the number of bits dynamically
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
            else if(binaryStr[i]=='1') binaryStr[i] = '0';
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


