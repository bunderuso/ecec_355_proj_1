#include "Core.h"
#include "Parser.h"
#include <string.h>
#include <math.h>

Core *initCore(Instruction_Memory *i_mem)
{
    Core *core = (Core *)malloc(sizeof(Core));

    core->clk = 0;

    core->PC = 0;

    core->instr_mem = i_mem;

    core->tick = tickFunc;

    // initialize data mem 
    
    for(int j = 0 ; j < sizeof(core->data_mem)/sizeof(core->data_mem[0]) ; j++){ 
        core->data_mem[j] = 0;
    }

    //this is 16
    core->data_mem[0]= 16; //00010000
    core->data_mem[1]= 0; //00000000 
    core->data_mem[2]= 0; //00000000 
    core->data_mem[3]= 0; //00000000 
    core->data_mem[4]= 0; //00000000 
    core->data_mem[5]= 0; //00000000 
    core->data_mem[6]= 0; //00000000 
    core->data_mem[7]= 0; //00000000 

    //this is 128
    core->data_mem[8]= 128; //00000000 
    core->data_mem[9]= 0; //00000000 
    core->data_mem[10]= 0; //00000000 
    core->data_mem[11]= 0; //00000000 
    core->data_mem[12]= 0; //00000000 
    core->data_mem[13]= 0; //00000000 
    core->data_mem[14]= 0; //00000000 
    core->data_mem[15]= 0; //00000000 
    
    //this is 8
    core->data_mem[16]= 8; //00000000 
    core->data_mem[17]= 0; //00000000 
    core->data_mem[18]= 0; //00000000 
    core->data_mem[19]= 0; //00000000 
    core->data_mem[20]= 0; //00000000 
    core->data_mem[21]= 0; //00000000 
    core->data_mem[22]= 0; //00000000 
    core->data_mem[23]= 0; //00000000 

    //this is 4
    core->data_mem[24]=4; //00000100
    core->data_mem[25]=0; //00000000 
    core->data_mem[26]=0; //00000000 
    core->data_mem[27]=0; //00000000 
    core->data_mem[28]=0; //00000000 
    core->data_mem[29]=0; //00000000 
    core->data_mem[30]=0; //00000000 
    core->data_mem[31]=0; //00000000 
    
    // initialize register file

    for(int i = 0 ; i < sizeof(core->reg_file)/sizeof(core->reg_file[0]) ; i++){ 
        core->reg_file[i] = 0;
    }

    core->reg_file[22] = 1;
    
    // test configs
    /*
    core->reg_file[25] = 4;
    core->reg_file[10] = 4;
    core->reg_file[22] = 1;

    */
    return core;
}

// FIXME, implement this function
bool tickFunc(Core *core)
{
    Signal select = 1;
    int sign_extended;
    // Steps may include
    // (Step 1) Reading instruction from instruction memory
    
    unsigned instruction = core->instr_mem->instructions[core->PC / 4].instruction;
    
    // (Step 2) ...
    // the instructions are coming in as decimal numbers

    //STEP 2.1: converting the decimal values to binary
    int *imme_binary;
    //checking if instruction is a negative number 
        if (instruction < 0) {
            imme_binary = signedBinary(instruction);
        }
        else {
            imme_binary = binary(instruction);
        }
    
    //STEP 2.2
    //sending the first seven bits (opcode) to the control unit 

    char deci_1[9] = "\0";
    char a_1[2] = "1";
    char a_0[2] = "0";
    for(int j=6; j>=0; j--){ // loop runs 7 times for 7 bits
        if(imme_binary[j] == 0){
            strcat(deci_1, a_0);
        }
        else{
            strcat(deci_1, a_1);
        }
    }
    int opcode_binary = atoi(deci_1);
    int deci_opcode = bin_to_dec(opcode_binary); 

    //intializing the control signal lines 
    ControlSignals *signals = (ControlSignals *)malloc(sizeof(ControlSignals));

    //calling the control unit function 
    ControlUnit(deci_opcode, signals);

    //STEP 2.3
    //Calling the ALU control unit 
    //to do this we need to find the Funct3 and Funct7 in decimal 
    
    // code block below finds the Funct 3 in decimal 
    char deci_2[9] = "\0";
    for(int j=14; j>=12; j--){ // loop runs 3 times for 3 bits
        if(imme_binary[j] == 0){
            strcat(deci_2, a_0);
        }
        else{
            strcat(deci_2, a_1);
        }
    }
    int funct3_binary = atoi(deci_2);
    int deci_funct3 = bin_to_dec(funct3_binary);

    // code block below finds the Funct 7 in decimal 
    char deci_3[12] = "\0";
    for(int j=31; j>=25; j--){
        if(imme_binary[j] == 0){
            strcat(deci_3, a_0);
        }
        else{
            strcat(deci_3, a_1);
        }
    }
    int funct7_binary = atoi(deci_3);
    int deci_funct7 = bin_to_dec(funct7_binary);

    //getting register 1 in decimal form 
    char deci_reg1[12] = "\0";
    for(int j=19; j>=15; j--){
        if(imme_binary[j] == 0){
            strcat(deci_reg1, a_0);
        }
        else{
            strcat(deci_reg1, a_1);
        }
    }
    int reg1_binary = atoi(deci_reg1);
    int deci_reg1_comp = bin_to_dec(reg1_binary);
    int reg1 = core->reg_file[deci_reg1_comp];

    //calling the ALU control unit function 
    int64_t alu_signal; //TODO: this is supposed to be of the typedeffed "Signal" type
    alu_signal = ALUControlUnit(signals->ALUOp, deci_funct7, deci_funct3);

    //STEP 2.4
    //calling the immediate generator function 
    
    if (deci_opcode == 19 || deci_opcode == 3){
      // code block below finds the Immediate in decimal for i-types

      //getting rd in decimal form
      char deci_rd[12] = "\0";
      for(int j=11; j>=7; j--){
          if(imme_binary[j] == 0){
              strcat(deci_rd, a_0);
          }
          else{
              strcat(deci_rd, a_1);
          }
      }
      int rd_binary = atoi(deci_rd);
      int deci_rd_comp = bin_to_dec(rd_binary);
      int rd = deci_rd_comp;

      int deci_imm[12];
      int counter = 11;
      for(int j=31; j>=20; j--){
          if(imme_binary[j] == 0){
              deci_imm[counter] = 0;
          }
          else{
              deci_imm[counter] = 1;
          }
          counter--;
      }
      
      //int immediate_binary = atoi(deci_imm);
      //deci_immediate = bin_to_dec(immediate_binary);
      sign_extended = ImmeGen(deci_imm);
    }
    // immediate generation for bne instruction
    else if (deci_opcode == 99){
      // code block below finds the Immediate in decimal for bne
      //getting register 2 in decimal form
      char deci_reg2[16] = "\0";
      for(int j=24; j>=20; j--){
          if(imme_binary[j] == 0){
              strcat(deci_reg2, a_0);
          }
          else{
              strcat(deci_reg2, a_1);
          }
      }
      int reg2_binary = atoi(deci_reg2);
      int deci_reg2_comp = bin_to_dec(reg2_binary);
      int reg2 = core->reg_file[deci_reg2_comp];

      int deci_imm[12];

      //checking the 11th bit of immediate
      if(imme_binary[31] == 0){
          deci_imm[11] = 0;
      }
      else{
          deci_imm[11] = 1;
      }
      
      //checking the 10th bit of immediate
      if(imme_binary[7] == 0){
          deci_imm[10] = 0;
      }
      else{
          deci_imm[10] = 1;
      }
      
      //checking the 9th bit of immediate
      if(imme_binary[30] == 0){
          deci_imm[9] = 0;
      }
      else{
          deci_imm[9] = 1;
      }
      
      //checking the 8th bit of immediate
      if(imme_binary[29] == 0){
          deci_imm[8] = 0;
      }
      else{
          deci_imm[8] = 1;
      }
      
      //checking the 7th bit of immediate
      if(imme_binary[28] == 0){
          deci_imm[7] = 0;
      }
      else{
          deci_imm[7] = 1;
      }
      
      //checking the 6th bit of immediate
      if(imme_binary[27] == 0){
          deci_imm[6] = 0;
      }
      else{
          deci_imm[6] = 1;
      }
      
      //checking the 5th bit of immediate
      if(imme_binary[26] == 0){
          deci_imm[5] = 0;
      }
      else{
          deci_imm[5] = 1;
      }
      
      //checking the 4th bit of immediate
      if(imme_binary[25] == 0){
          deci_imm[4] = 0;
      }
      else{
          deci_imm[4] = 1;
      }
      
      //checking the 3th bit of immediate
      if(imme_binary[11] == 0){
          deci_imm[3] = 0;
      }
      else{
          deci_imm[3] = 1;
      }
      
      //checking the 2th bit of immediate
      if(imme_binary[10] == 0){
          deci_imm[2] = 0;
      }
      else{
          deci_imm[2] = 1;
      }
      
      //checking the 1th bit of immediate
      if(imme_binary[9] == 0){
          deci_imm[1] = 0;
      }
      else{
          deci_imm[1] = 1;
      }
      
      //checking the 0th bit of immediate
      if(imme_binary[8] == 0){
          deci_imm[0] = 0;
      }
      else{
          deci_imm[0] = 1;
      }
      //int immediate_binary = atoi(deci_imm);
      //deci_immediate = bin_to_dec(immediate_binary);
      sign_extended = ImmeGen(deci_imm);

      if (reg1 != reg2){
        select = 0;
      }
    }

    //STEP 2.5
    //calling the ALU function
    //we need to find the register 1 and register 2 in the case of a non-immediate function 
    
    //Calling Add in the ALU
    if(deci_opcode == 51){
        //getting register 2 in decimal form
        char deci_reg2[12] = "\0";
        for(int j=24; j>=20; j--){
            if(imme_binary[j] == 0){
                strcat(deci_reg2, a_0);
            }
            else{
                strcat(deci_reg2, a_1);
            }
        }
        int reg2_binary = atoi(deci_reg2);
        int deci_reg2_comp = bin_to_dec(reg2_binary);
        int reg2 = core->reg_file[deci_reg2_comp];

        //getting rd in decimal form
        char deci_rd[12] = "\0";
        for(int j=11; j>=7; j--){
            if(imme_binary[j] == 0){
                strcat(deci_rd, a_0);
            }
            else{
                strcat(deci_rd, a_1);
            }
        }
        int rd_binary = atoi(deci_rd);
        int deci_rd_comp = bin_to_dec(rd_binary);
        int rd = deci_rd_comp;

        //calling the ALU function 
        int64_t *ALU_result = malloc(sizeof(int64_t));; //long int *ALU_result = 0;
        int64_t *zero = malloc(sizeof(int64_t));; //long int *zero = 0;
        ALU(reg1, reg2, alu_signal, ALU_result, zero); 

        core->reg_file[rd] = *ALU_result;
        
    }

    //For ADDI
    else if(deci_opcode == 19 && deci_funct3 == 0){
        //getting rd in decimal form
        char deci_rd[12] = "\0";
        for(int j=11; j>=7; j--){
            if(imme_binary[j] == 0){
                strcat(deci_rd, a_0);
            }
            else{
                strcat(deci_rd, a_1);
            }
        }
        int rd_binary = atoi(deci_rd);
        int deci_rd_comp = bin_to_dec(rd_binary);
        int rd = deci_rd_comp;
        
        //adding the immediate with reg1 
        int temp = 0;
        temp = sign_extended + reg1;

        core->reg_file[rd] = temp;
    }

    // for slli
    else if(deci_opcode == 19 && deci_funct3 == 1){
      //getting rd in decimal form
        char deci_rd[12] = "\0";
        for(int j=11; j>=7; j--){
            if(imme_binary[j] == 0){
                strcat(deci_rd, a_0);
            }
            else{
                strcat(deci_rd, a_1);
            }
        }
        int rd_binary = atoi(deci_rd);
        int deci_rd_comp = bin_to_dec(rd_binary);
        int rd = deci_rd_comp;

        int temp = 0;
        temp = reg1 << sign_extended;
        core->reg_file[rd] = temp;
    }

    // for LD
    else if(deci_opcode == 3 && deci_funct3 == 3){
      //getting rd in decimal form
        char deci_rd[12] = "\0";
        for(int j=11; j>=7; j--){
            if(imme_binary[j] == 0){
                strcat(deci_rd, a_0);
            }
            else{
                strcat(deci_rd, a_1);
            }
        }
        int rd_binary = atoi(deci_rd);
        int deci_rd_comp = bin_to_dec(rd_binary);
        int rd = deci_rd_comp;
        
        //calling the ALU function 
        int64_t *ALU_result = malloc(sizeof(int64_t));; //long int *ALU_result = 0;
        int64_t *zero = malloc(sizeof(int64_t));; //long int *zero = 0;
        ALU(reg1, sign_extended, alu_signal, ALU_result, zero); 

        //grabbing the data stored at the address
        int mem_value = 0;
        mem_value = core->data_mem[*ALU_result];

        //storing the memory value into 9 
        core->reg_file[9] = mem_value;
    }

    // (Step N) Increment PC. FIXME, is it correct to always increment PC by 4?!

    if (select == 1){
      core->PC += 4;
    }
    else{
      core->PC += sign_extended * 4;
    }
    //core->PC += 4;
    
    ++core->clk;
    // Are we reaching the final instruction?
    if (core->PC > core->instr_mem->last->addr)
    {
        return false;
    }

    return true;  
}

// FIXME (1). Control Unit. Refer to Figure 4.18.
void ControlUnit(Signal input,
                 ControlSignals *signals)
{
    // For R-type
    if (input == 51)
    {
        signals->ALUSrc = 0;
        signals->MemtoReg = 0;
        signals->RegWrite = 1;
        signals->MemRead = 0;
        signals->MemWrite = 0;
        signals->Branch = 0;
        signals->ALUOp = 2;
    }

    //For ld 
    else if(input == 3){
        signals->ALUSrc = 1;
        signals->MemtoReg = 1;
        signals->RegWrite = 1;
        signals->MemRead = 1;
        signals->MemWrite = 0;
        signals->Branch = 0;
        signals->ALUOp = 0;
    }

    //For sd 
    else if(input == 35){
        signals->ALUSrc = 1;
        //signals->MemtoReg = 1;
        signals->RegWrite = 0;
        signals->MemRead = 0;
        signals->MemWrite = 1;
        signals->Branch = 0;
        signals->ALUOp = 0;
    }

    //For beq
    else if(input == 99){
        signals->ALUSrc = 0;
        //signals->MemtoReg = 1;
        signals->RegWrite = 0;
        signals->MemRead = 0;
        signals->MemWrite = 0;
        signals->Branch = 1;
        signals->ALUOp = 1;
    }
}

// FIXME (2). ALU Control Unit. Refer to Figure 4.12.
// should be gucci gang
// TODO: TEST MESA
Signal ALUControlUnit(Signal ALUOp,
                      Signal Funct7,
                      Signal Funct3)
{
    // For add
    if (ALUOp == 2 && Funct7 == 0 && Funct3 == 0)
    {
        //telling the ALU to add things
        return 2;
    }
    // For ld and sd 
    else if(ALUOp == 0){
        //telling the ALU to add things
        return 2;
    } 

    // For sub 
    else if(ALUOp == 2 && Funct7 == 32 && Funct3 == 0){
        //telling the ALU to sub things
        return 6;
    } 

    // For and 
    else if(ALUOp == 2 && Funct7 == 0 && Funct3 == 7){
        //telling the ALU to sub things
        return 0;
    } 

    // For or 
    else if(ALUOp == 2 && Funct7 == 0 && Funct3 == 6){
        //telling the ALU to or things
        return 1;
    } 

    // For beq 
    else if(ALUOp == 1){
        //telling the ALU to add things
        return 6;
    } 
}

// FIXME (3). Imme. Generator
//input is only ever filled up to the 32nd bit.
Signal ImmeGen(int *binary_imm) 
{ 
    int deci_input[64];
    char deci_chars[66];
    int temp = 0;
    int counter = 52;
    // sign extend by adding 0s for positives, 1s for negatives
    if (binary_imm[11] == 1){
      //adding the ones for padding
      for (int i = 0; i < 64; i++){
        deci_input[i] = 1;
      }

      //adding the OG values
      for(int k=11; k>=0; k--){
        deci_input[counter] = binary_imm[k];
        counter++;
      }

      //flipping the bits of the binary number 
      for(int i = 63; i >= 0; i--){
          if (deci_input[i] == 1){
              deci_input[i] = 0;
          }
          else{
              deci_input[i] = 1;
          }
      }

      //adding the one to the number
      for(int h = 63; h >= 0; h--){
          if (deci_input[h] == 1){
              deci_input[h] = 0;
          }
          else{
              deci_input[h] = 1;
              break;
          }
      }

      //adding this to a string 
      for(int u = 0; u < 64; u++){
          if(deci_input[u] == 0){
              strcat(deci_chars, "0");
          }
          else{
              strcat(deci_chars, "1");
          }
      }

        //converting it to decimal
        int temp = atoi(deci_chars);
        Signal input_immediate = bin_to_dec(temp);
        return input_immediate * -1;
      
    }

    
    else{
      // pad with 0s
      for (int i = 0; i < 64; i++){
        deci_input[i] = 0;
      }
      
      //adding the OG values
      for(int k=11; k>=0; k--){
        deci_input[counter] = binary_imm[k];
        counter++;
      }

      //adding this to a string 
      for(int u = 0; u < 64; u++){
          if(deci_input[u] == 0){
              strcat(deci_chars, "0");
          }
          else{
              strcat(deci_chars, "1");
          }
      }

        //converting it to decimal
        int temp = atoi(deci_chars);
        Signal input_immediate = bin_to_dec(temp);
        return input_immediate;
    }
}

// FIXME (4). ALU
void ALU(Signal input_0, 
         Signal input_1,
         Signal ALU_ctrl_signal,
         Signal *ALU_result,
         Signal *zero)
{
    // For addition
    if (ALU_ctrl_signal == 2)
    {
        *ALU_result = (input_0 + input_1);
        if (*ALU_result == 0) { *zero = 1; } else { *zero = 0; }
    }

    // For subtraction
    else if (ALU_ctrl_signal == 6)
    {
        *ALU_result = (input_0 - input_1);
        if (*ALU_result == 0) { *zero = 1; } else { *zero = 0; }
    }

    // For or (assuming bitwise)
    else if (ALU_ctrl_signal == 1)
    {
        *ALU_result = (input_0 | input_1);
        if (*ALU_result == 0) { *zero = 1; } else { *zero = 0; }
    }

    // For and (assuming bitwise)
    else if (ALU_ctrl_signal == 0)
    {
        *ALU_result = (input_0 & input_1);
        if (*ALU_result == 0) { *zero = 1; } else { *zero = 0; }
    }
}

// (4). MUX
Signal MUX(Signal sel,
           Signal input_0,
           Signal input_1)
{
    if (sel == 0) { return input_0; } else { return input_1; }
}

// (5). Add
Signal Add(Signal input_0,
           Signal input_1)
{
    return (input_0 + input_1);
}

// (6). ShiftLeft1
Signal ShiftLeft1(Signal input)
{
    return input << 1;
}

int bin_to_dec(int n) { 
//ANS: Function takes a binary value and converts it to decimal. Decimal is easier to compare than binary numbers stored in an array
    int dec = 0, i = 0, rem;
    while (n != 0) {
        rem = n % 10;
        n /= 10;
        dec += rem * pow(2, i);
        ++i;
    }
    return dec;
}