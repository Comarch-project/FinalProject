
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAXLINELENGTH 1000

int readAndParse(FILE *, char *, char *, char *, char *, char *);
int isNumber(char *);

int main(int argc, char *argv[]) //argv = argument vector, argc = argument count 
{
    char *inFileString, *outFileString;
    FILE *inFilePtr, *outFilePtr;
    char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH],
            arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];
    char binaryOp[4];
    if (argc != 3) { 
        printf("error: usage: %s <assembly-code-file> <machine-code-file>\n",
            argv[0]);
        exit(1);
    }else{
        printf("can access to file \n");
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

    while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)) {

        //printf("Label: %s, Opcode: %s, Arg0: %s, Arg1: %s, Arg2: %s\n", label, opcode, arg0, arg1, arg2);
        if (!strcmp(opcode, "add")) {
            strcpy(binaryOp,"000");
            
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
        }else if(!strcmp(opcode, "halt")){
            strcpy(binaryOp,"110");
        }else if(!strcmp(opcode, "noop")){
            strcpy(binaryOp,"111");
        }
        printf(binaryOp);
        printf("\n");
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
    // Print the decimal number in hexadecimal format
    printf("Hexadecimal: %X\n", decimal);
}

