
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
    // Create an array of KeyValuePair structs to store the lable data
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
    //open input file with read mode
    inFilePtr = fopen(inFileString, "r");                       
    if (inFilePtr == NULL) {                                     
        printf("error in opening %s\n", inFileString);
        exit(1);
    }
    //open output file with write mode
    outFilePtr = fopen(outFileString, "w");
    if (outFilePtr == NULL) {
        printf("error in opening %s\n", outFileString);
        exit(1);
    }
    //open output for simulator file with write mode
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
    rewind(inFilePtr); // rewind the input
    // Initiate value of each lable 
    linecnt = 0;// To indicate address
    int keyvalpt = 0;// To indicate current size of keyValue
    while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)){
        if(!strcmp(opcode,".fill")){ // if the opcode was fill then it is the lable 
            strcpy(keyValueList[keyvalpt].type, opcode); // assign opcode (just in case) 
            strcpy(keyValueList[keyvalpt].key, label); // assign label name
            if(isNumber(arg0)){// if the label input was number
                strcpy(keyValueList[keyvalpt].value, arg0) ; // assign value directly
                char str[20] ;
                sprintf(str, "%d", linecnt);
                strcpy(keyValueList[keyvalpt].address, str); // assign the address of the lable
            }else{// if the label input was label (symbolic)
                int i;
                for (i = 0; i < maxPairs; i++) { //find the matching key in key-value pair list
                    if (!strcmp(keyValueList[i].key, arg0)) {
                        strcpy(keyValueList[keyvalpt].value, keyValueList[i].value);
                        char str[20] ;
                        sprintf(str, "%d", linecnt);
                        strcpy(keyValueList[keyvalpt].address, str);
                        break;
                    }
                }
            }
            keyvalpt++;
        }else if(strcmp(label,"")){  // if the label filled but opcode wasn't .fill then it is the instruction lable 
            for (int i = 0; i < keyvalpt; i++) { // find duplicated instruction label
                if(!strcmp(label, keyValueList[i].key)){
                    printf("Duplicated lable at line %d",linecnt+1);
                    exit(1) ;
                }
            }
            // if the instruction label was not duplicated 
            strcpy(keyValueList[keyvalpt].type, opcode);// assign opcode (just in case) 
            strcpy(keyValueList[keyvalpt].key, label);// assign label name
            char str[20] ;
            sprintf(str, "%d", linecnt);
            strcpy(keyValueList[keyvalpt].value, str); // assign value according to the address of that instruction
            strcpy(keyValueList[keyvalpt].address, str);// assign the address of the lable
            keyvalpt++;
        }
        linecnt++;
    }
    // get the output of key-value list assigning
    for (int i = 0; i < keyvalpt; i++) {
        printf(">>> lable : %s ,value : %s ,Addr : %s\n",keyValueList[i].key,keyValueList[i].value,keyValueList[i].address);
    }
    rewind(inFilePtr);// rewind the input

    linecnt = 0;
    // start reading input line by line and turn it into machine code
    while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)) {
        printf("Label: %s, Opcode: %s, Arg0: %s, Arg1: %s, Arg2: %s\n", label, opcode, arg0, arg1, arg2);
        if (!strcmp(opcode, "add")||!strcmp(opcode, "nand")) {// if the instruction was add or nand
            char *biRS,*biRT,*biRD;
            // Add opcode
            if(!strcmp(opcode, "add")) strcpy(binaryOp,"000");
            if(!strcmp(opcode, "nand")) strcpy(binaryOp,"001");
            binaryMachCode[7]=binaryOp[0];
            binaryMachCode[8]=binaryOp[1];
            binaryMachCode[9]=binaryOp[2];
            // Add rs
            if(isNumber(arg0)){// if the arg0 is number then directly assign
                biRS = decToBiUnsign3b(arg0);
            }else{// if it was label then search from the key-value pair list
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
            // Add rt
            if(isNumber(arg1)){// if the arg1 is number then directly assign 
                biRT = decToBiUnsign3b(arg1);
            }else{// if it was label then search from the key-value pair list
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
            // Add rd
            if(isNumber(arg2)){// if the arg2 is number then directly assign
                biRD = decToBiUnsign3b(arg2);
            }else{// if it was label then search from the key-value pair list
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
            // free the memory
            free(biRS);
            free(biRT);
            free(biRD);
        }else if(!strcmp(opcode, "lw")||!strcmp(opcode, "sw")){// if the instruction lw or sw
            char *biRS,*biRT,*biOff;
            // Add opcode
            if(!strcmp(opcode, "lw"))strcpy(binaryOp,"010");
            if(!strcmp(opcode, "sw"))strcpy(binaryOp,"011");
            binaryMachCode[7]=binaryOp[0];
            binaryMachCode[8]=binaryOp[1];
            binaryMachCode[9]=binaryOp[2];
            // Add rs
            if(isNumber(arg0)){// if the arg0 is number then directly assign
                biRS = decToBiUnsign3b(arg0);
            }else{// if it was label then search from the key-value pair list
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
            if(isNumber(arg1)){// if the arg1 is number then directly assign 
                biRT = decToBiUnsign3b(arg1);
            }else{// if it was label then search from the key32value pair list
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
            if(isNumber(arg2)){// if the arg2 is number then directly assign 
                biOff = decToBiSign16b(arg2,linecnt);
            }else{// if it was label then search from the key-value pair list
                for (int i = 0; i <= keyvalpt; i++) {
                    if(!strcmp(arg2, keyValueList[i].key)){
                            if(atoi(keyValueList[i].value)>32767||atoi(keyValueList[i].value)<(-32768)){// check if the input has more than 16 bit or not
                            printf("Wrong offset input at line %d",linecnt+1);
                            exit(1);
                        }
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
            // free the memory
            free(biRS);
            free(biRT);
            free(biOff);
        }else if(!strcmp(opcode, "beq")){// if the instruction was beq
            char *biRS,*biRT,*biOff;
            // Add opcode
            strcpy(binaryOp,"100");
            binaryMachCode[7]=binaryOp[0];
            binaryMachCode[8]=binaryOp[1];
            binaryMachCode[9]=binaryOp[2];
            // Add rs
            if(isNumber(arg0)){// if the arg0 is number then directly assign 
                biRS = decToBiUnsign3b(arg0);
            }else{// if it was label then search from the key-value pair list
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
            // Add rd
            if(isNumber(arg1)){// if the arg1 is number then directly assign 
                biRT = decToBiUnsign3b(arg1);
            }else{// if it was label then search from the key-value pair list
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
            // Add offsetField
            if(isNumber(arg2)){// if the arg2 is number then directly assign
                biOff = decToBiSign16b(arg2,linecnt);
            }else{// if it was label then search from the key-value pair list
                for (int i = 0; i <= keyvalpt; i++) {
                    if(!strcmp(arg2, keyValueList[i].key)){
                        int offseti = (-linecnt)-1+atoi(keyValueList[i].address);
                        char offsetc[32];
                        sprintf(offsetc, "%d", offseti);
                        biOff = decToBiSign16b(offsetc,linecnt);
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
            // free the memory
            free(biRS);
            free(biRT);
            free(biOff);
            
        }else if(!strcmp(opcode, "jalr")){// if the instruction was jalr
            char *biRS,*biRT;
            // Add opcode
            strcpy(binaryOp,"101");
            binaryMachCode[7]=binaryOp[2];
            binaryMachCode[8]=binaryOp[1];
            binaryMachCode[9]=binaryOp[0];
            //Add rs
            if(isNumber(arg0)){// if the arg0 is number then directly assign
                biRS = decToBiUnsign3b(arg0);
            }else{// if it was label then search from the key-value pair list
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
            // Add rd
            if(isNumber(arg1)){// if the arg1 is number then directly assign 
                biRT = decToBiUnsign3b(arg1);
            }else{// if it was label then search from the key-value pair list
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
            // free the memory
            free(biRS);
            free(biRT);
        }else if(!strcmp(opcode, "halt")){// if the instruction was halt
            // Add opcode
            strcpy(binaryOp,"110");
            binaryMachCode[7]=binaryOp[0];
            binaryMachCode[8]=binaryOp[1];
            binaryMachCode[9]=binaryOp[2];
        }else if(!strcmp(opcode, "noop")){// if the instruction was noop
            // Add opcode
            strcpy(binaryOp,"111");
            binaryMachCode[7]=binaryOp[0];
            binaryMachCode[8]=binaryOp[1];
            binaryMachCode[9]=binaryOp[2];
        }else if(!strcmp(opcode, ".fill")){// if the instruction .fill
            char *biCode;
            int value;
            if(isNumber(arg0)){// if the arg0 is number then directly assign
                biCode=decToBiSign32b(arg0);
                value = atoi(arg0);
            }else{// if it was label then search from the key-value pair list
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
            strcpy(binaryMachCode,biCode);
            // print the output to output file
            biToHex4fill(biCode,outFilePtr,outFilePtrSim,linecnt,value);
            // free the memory
            free(biCode);
        }
        // print the output to output file for non .fill instruction
        if(strcmp(opcode, ".fill")) biToHex(binaryMachCode,outFilePtr,outFilePtrSim,linecnt);
        // print to see the outcome of the result
        printf("Address : %d\n",linecnt);
        printf("Binary: %s \n",binaryMachCode);
        printf("--------------------------------------------\n");
        strcpy(binaryMachCode, "00000000000000000000000000000000");
        linecnt++;
    }
    // close the output file
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
int readAndParse(FILE *inFilePtr, char *label, char *opcode, char *arg0,char *arg1, char *arg2)
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

// biToHex convert binary to hexadecimal and print it in output file with a format of "instruction"
int biToHex(char bin[],FILE *str,FILE *strSim,int addr){
    // Convert binary to integer using strtol
    unsigned int decimal = strtoll(bin, NULL, 2);//convert string to long long int
    fprintf(str,"(address %d): %d (hex 0x%X)\n", addr,decimal,decimal);
    fprintf(strSim,"%d\n",decimal);
    printf("Hexadecimal: %X\n", decimal);//write to text here
    return(1);
}
// biToHex4Fill convert binary to hexadecimal and print it in output file with a format of ".fill"
int biToHex4fill(char bin[],FILE *str,FILE *strSim,int addr,int val){
    // Convert binary to integer using strtol
    unsigned int decimal = strtoll(bin, NULL, 2); //convert string to long long int
    fprintf(str,"(address %d): %d (hex 0x%X)\n", addr,val,decimal);\
    fprintf(strSim,"%d\n", val);
    printf("Hexadecimal: %X\n", decimal);//write to text here
    return(1);
}
// decToBiSign16b convert decimal to 16 bit signed binary
char* decToBiSign16b(char *string,int linecnt) {
    long int n =atol(string);// // Use atol to convert string to long int
    if(n>32767||n<(-32768)){// if  the input is more than 16 bit exit(1)
        printf("Wrong offset input at line %d",linecnt+1);
        exit(1) ;
    }

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
    }else{// if the input was positive or 0
    // Convert the decimal number to binary and store it in binaryStr
        for (int i = 16 - 1; i >= 0; i--) {
        binaryStr[i] = (n % 2) + '0'; // Convert remainder to character '0' or '1'
        n = n / 2;
        }
    binaryStr[16] = '\0';
    }
    return binaryStr;
}
// decToBiUnsign3b convert decimal to 16 bit unsigned binary
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
// decToBiSign32b convert decimal to 32 bit signed binary
char* decToBiSign32b(char *string) {  
    long int n =atol(string);// Use atol to convert string to long int
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


