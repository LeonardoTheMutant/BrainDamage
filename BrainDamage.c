// BrainDamage
// A BrainF*ck interpreter in C
// Code by LeonardoTheMutant

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

FILE *inputFile;
uint16_t bytesRead;
char bufferRead[UINT16_MAX];
char program[UINT16_MAX];   //Program Memory, this stores the BF code
uint8_t memory[INT16_MAX];  //Memory, also known as "cells"
uint16_t loopPoints[UINT8_MAX]; //Loop entry points
uint16_t progSize;
uint16_t progPointer;       //Program Pointer, points to what instruction is being executed right now
uint16_t memPointer;        //Memory Pointer, points to the currently selected memory cell
uint8_t loopPointer;        //Loop Pointer, points to the loop currently being executed (for nested loops)
uint8_t loopsIgnore;
uint8_t flags;              //Binary flags: 0x1 - Debug mode;

int main(int argc, char **argv ) {
    if (argc < 2) {
        printf("BrainDamage - A BrainF*ck interpreter\n\n%s [inputFile] -d\n\t -d - Enable the Debug Mode", argv[0]);
        return 0;
    }

    //Check the Command Line arguments
    for (uint8_t x = 1; x < argc; x++) {
        if (!strcmp(argv[x], "-d")) {
            flags |= 0x1; //Debug Mode is enabled
        } else {
            inputFile = fopen(argv[x], "r");
            if (!inputFile) {
                fprintf(stderr, "Error opening the input file (%s)", argv[x]);
                return 1;
            }
        }
    }

    //Copying the program into the memory and counting its size
    progSize = 0;
    while(!feof(inputFile)) {
        bytesRead = fread(bufferRead, 1, UINT16_MAX, inputFile);
        progSize += bytesRead;
		memcpy(program + progSize - bytesRead, bufferRead, bytesRead);
    }
    fclose(inputFile);

    //Interpreter
    progPointer = 0;
    memPointer = 0;
    loopPointer = 0;
    loopsIgnore = 0;
    while (progPointer < progSize) {
        if (flags & 0x1) printf("%d %d %c\n", memPointer, memory[memPointer], program[progPointer]); //Debug
        switch (program[progPointer]) {
            case '+':
                memory[memPointer]++;
                break;
            case '-':
                memory[memPointer]--;
                break;
            case '.':
                putchar(memory[memPointer]);
                break;
            case ',':
                memory[memPointer] = getchar();
                break;
            case '>':
                memPointer++; //memPointer is an Unsigned 16-bit integer, it will overflow to 0 after 65535
                break;
            case '<':
                memPointer--; //memPointer is an Unsigned 16-bit integer, it will underflow to 65535 after 0
                break;
            case '[':
                switch(memory[memPointer]) { //replaced the IF instruction here for speed
                    case 0: //The memory cell value is 0, skip the loop
                        loopsIgnore = 1;
                        while(loopsIgnore) {
                            progPointer++;

                            switch(program[progPointer]) {
                                case ']':
                                    loopsIgnore--;
                                    break;
                                case '[':
                                    loopsIgnore++;
                                    break;
                            }

                            if (progPointer == UINT16_MAX) {
                                fprintf(stderr, "Runtime error: ']' not found to end the loop\n");
                                return 2;
                            }
                        }
                        break;
                    default: //The memory cell is not 0, go into the loop
                        loopPoints[loopPointer] = progPointer;
                        loopPointer++;
                        break;
                }
                break;
            case ']':
                switch (loopPointer) { //replaced the IF instruction here for speed
                    case 0 :
                        fprintf(stderr, "Runtime error: Found ']', but not '[' to start the loop\n");
                        return 2;
                    default:
                        switch (memory[memPointer]) { //replaced the IF instruction here for speed
                            case 0: //The memory cell value is 0, leave the loop
                                loopPointer--;
                                break;
                            default: //The memory cell value is not 0, jump to the beginning of the loop
                                progPointer = loopPoints[loopPointer - 1];
                                break;
                        }
                }

                break;

            //The following instructions are not a part of the official BrainF*ck standard
            case '#':
                printf("%d", memory[memPointer]);
                break;
        }
        progPointer++;
    }
}