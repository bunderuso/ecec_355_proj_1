#ifndef __CORE_H__
#define __CORE_H__

#include "Instruction_Memory.h"

#include "set.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

#define BOOL bool

typedef uint8_t Byte;
typedef int64_t Signal;
typedef int64_t Register;

struct Core;
typedef struct Core Core;
typedef struct Core
{
    Tick clk; // Keep track of core clock
    Addr PC; // Keep track of program counter

    // What else you need? Data memory? Register file?
    Instruction_Memory *instr_mem;
   
    Byte data_mem[1024]; // data memory

    Register reg_file[32]; // register file.

    bool (*tick)(Core *core);
}Core;

Core *initCore(Instruction_Memory *i_mem);
bool tickFunc(Core *core);

// FIXME. Implement the following functions in Core.c
// FIXME (1). Control Unit.
typedef struct ControlSignals
{
    Signal Branch;
    Signal MemRead;
    Signal MemtoReg;
    Signal ALUOp;
    Signal MemWrite;
    Signal ALUSrc;
    Signal RegWrite;
}ControlSignals;
void ControlUnit(Signal input,
                 ControlSignals *signals);

// FIXME (2). ALU Control Unit.
Signal ALUControlUnit(Signal ALUOp,
                      Signal Funct7,
                      Signal Funct3);

// FIXME (3). Imme. Generator
Signal ImmeGen(int *binary_imm);

// FIXME (4). ALU
void ALU(Signal input_0,
         Signal input_1,
         Signal ALU_ctrl_signal,
         Signal *ALU_result,
         Signal *zero);

// (4). MUX
Signal MUX(Signal sel,
           Signal input_0,
           Signal input_1);

// (5). Add
Signal Add(Signal input_0,
           Signal input_1);

// (6). ShiftLeft1
Signal ShiftLeft1(Signal input);

//to decimal function 
int bin_to_dec(int n);

int opcode_parse( int * imme_binary);
int rdParse(int instr[]);
int funct3Parse(int instr[]);
int rs1Parse(int instr[], Core* core);
int *Itype_immediate(int instrFetch[]);
int rs2Parse(int instr[]);
int funct7Parse(int instr[]);
void RType(struct node * curInstr, Core *core, ControlSignals *signals);
void ldExec(struct node * curInstr, Core *core, ControlSignals *signals);
void ldMem(struct node * curInstr, Core *core, ControlSignals *signals);
void WB(struct node * curInstr, Core *core, ControlSignals *signals);


#endif