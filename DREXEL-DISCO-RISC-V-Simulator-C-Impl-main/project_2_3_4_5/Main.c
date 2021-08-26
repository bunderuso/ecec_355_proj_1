#include <stdio.h>
#include <math.h>
#include "Core.h"
#include "Parser.h"


int main(int argc, const char *argv[])
{	
    if (argc != 2)
    {
        printf("Usage: %s %s\n", argv[0], "<trace-file>");

        return 0;
    }

    /* Task One */
    // TODO, (1) parse and translate all the assembly instructions into binary format;
    // (2) store the translated binary instructions into instruction memory.
    Instruction_Memory instr_mem;
    instr_mem.last = NULL;
    loadInstructions(&instr_mem, argv[1]);

    unsigned PC = 0;
    while (1)
    {
        Instruction *instr = &(instr_mem.instructions[PC / 4]);
        printf("\nInstruction at PC: %u\n", PC);
        unsigned mask = (1 << 31);
        for (int i = 31; i >= 0; i--)
        {
            if (instr->instruction & mask) { printf("1 ");}
            else { printf("0 "); }

            mask >>= 1;
        }
        printf("\n");
        if (instr == instr_mem.last) { break; }
        PC += 4;
    }

    /* Task Two */
    // TODO, implement Core.{h,c}
    
    Core *core = initCore(&instr_mem);
    
    /* Task Three - Simulation */
    while (core->tick(core));
    
    printf("\nSimulation is finished.\n");
    printf("Value at x10: %ld\n", core->reg_file[10]);
    printf("Value at x11: %ld\n", core->reg_file[11]);
    printf("Value at x12: %ld\n", core->reg_file[12]);
    printf("Value at x13: %ld\n", core->reg_file[13]);
    printf("Value at x14: %ld\n", core->reg_file[14]);
    printf("Value at x5: %ld\n", core->reg_file[5]);
    
    int data_5;
    int signed_data5;
    int dec;

    for (int j = 47; j >= 40; j--){
      data_5 += core->data_mem[j];
    }
    
    for (int j = 7; j>= 0; j--){
      if (j > 0){
        data_5 -= core->data_mem[j+40] * pow(2,j);
      }
      else{
        data_5 -= core->data_mem[40];
      }
      
    }

    //signed_data5 = signedBinary(data_5);
    //dec = bin_to_dec(data_5);
    //printf("63 in signedBinary: %d\n", signedBinary(63))
    

    //printf("Value at data mem 5: %d\n", data_5);

    //printf("Value at data mem 5: %d\n", core->data_mem[40]);
    //printf("Value at x6: %ld\n", core->reg_file[6]);
    //printf("Value at data mem 6: %d\n", core->data_mem[48]);

    free(core);    
}