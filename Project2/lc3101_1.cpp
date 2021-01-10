/*
 * Mason Ballard
 * written in C++
 * CDA 3101 Project 2
 * Fall 2020, Tyson
 */

#include <iostream>
#include <stdio.h>
using namespace std;

#define NUMMEMORY 65536 /* maximum number of words in memory */
#define NUMREGS 8 /* number of machine registers */
#define MAXLINELENGTH 1000

typedef struct stateStruct {
    int pc;
    int mem[NUMMEMORY];
    int reg[NUMREGS];
    int numMemory;
} stateType;

void printState(stateType *);
int convertNum(int num);

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
        if (sscanf(line, "%d", state.mem+state.numMemory) != 1) {
            printf("error in reading address %d\n", state.numMemory);
            exit(1);
        }
        printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
    }
    cout << state.numMemory << endl;

    /* initialize all registers and pc to 0 */
    state.pc = 0;
    for (int i = 0; i < 8; i++)
    {
        state.reg[i] = 0;
    }

    /* print state once before execution, after every instruction, and once at end */
    printState(&state);

    int opcode, regA, regB, dest_off, instr = 0;
    opcode = (state.mem[state.pc] & 0x1C00000) >> 22;
    while (opcode != 6){
        if (opcode == 7){      // noop
            instr++;
            state.pc++;
        }
        else if (opcode == 0){      // add
            regA = (state.mem[state.pc] & 0x380000) >> 19;
            regB = (state.mem[state.pc] & 0x70000) >> 16;
            dest_off = (state.mem[state.pc] & 0x7);
            if (dest_off == 0){
                exit(1);
            }
            state.reg[dest_off] = state.reg[regA] + state.reg[regB];
            instr++;
            state.pc++;
        }
        else if (opcode == 1){      // nand
            regA = (state.mem[state.pc] & 0x380000) >> 19;
            regB = (state.mem[state.pc] & 0x70000) >> 16;
            dest_off = (state.mem[state.pc] & 0x7);
            if (dest_off == 0){
                exit(1);
            }
            state.reg[dest_off] = ~(state.reg[regA] & state.reg[regB]);
            instr++;
            state.pc++;
        }
        else if (opcode == 2){      // lw
            regA = (state.mem[state.pc] & 0x380000) >> 19;
            regB = (state.mem[state.pc] & 0x70000) >> 16;
            if (regB == 0){
                exit(1);
            }
            dest_off = convertNum(state.mem[state.pc] & 0xffff);
            if ((state.reg[regA] + dest_off) > state.numMemory | (state.reg[regA] + dest_off) < 0){
                exit(1);
            }
            state.reg[regB] = state.mem[state.reg[regA] + dest_off];
            instr++;
            state.pc++;
        }
        else if (opcode == 3){      // sw
            regA = (state.mem[state.pc] & 0x380000) >> 19;
            regB = (state.mem[state.pc] & 0x70000) >> 16;
            if (regB == 0){
                exit(1);
            }
            dest_off = convertNum(state.mem[state.pc] & 0xffff);
            if ((state.reg[regA] + dest_off) > state.numMemory | (state.reg[regA] + dest_off) < 0){
                exit(1);
            }
            state.reg[state.reg[regA] + dest_off] = state.reg[regB];
            instr++;
            state.pc++;
        }
        else if (opcode == 4){      // beq
            regA = (state.mem[state.pc] & 0x380000) >> 19;
            regB = (state.mem[state.pc] & 0x70000) >> 16;
            dest_off = convertNum(state.mem[state.pc] & 0xffff);
            if ((state.pc+1+dest_off) > state.numMemory | (state.pc+1+dest_off) < 0){
                exit(1);
            }
            if (state.reg[regA] == state.reg[regB])
                state.pc = state.pc+1+dest_off;
            else
                state.pc++;
            instr++;
        }
        else if (opcode == 5){      // jalr
            regA = (state.mem[state.pc] & 0x380000) >> 19;
            regB = (state.mem[state.pc] & 0x70000) >> 16;
            state.reg[regB] = state.pc + 1;
            instr++;
            state.pc = state.reg[regA];
        }
        printState(&state);
        opcode = (state.mem[state.pc] & 0x1C00000) >> 22;
    }

    state.pc++;
    cout << "machine halted" << endl;
    cout << "total of " << instr+1 << " instructions executed" << endl;
    cout << "final state of the machine: " << endl;
    printState(&state);

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

int convertNum(int num)
{
    /* convert a 16-bit number into a 32-bit Sun integer */
    if (num & (1<<15) ) {
        num -= (1<<16);
    }
    return(num);
}