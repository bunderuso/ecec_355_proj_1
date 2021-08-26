#include "Core.h"
#include "Parser.h"
#include <string.h>
#include <math.h>

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "set.h" //includes a SET, which contains a linked list head and the size of the linked list

/**********************************************************************************************************************/
typedef struct node node;
typedef struct set_t Set;

struct node {
  int fetch[32]; //either "static int fetch[32];" or "int *fetch;"
  int opcode; //value based on certain bits of int fetch, which is assigned during the decode stage based on opcode
  int execResult; 
  int memAddr;
  int memVal;
  int rd; 
  int funct3; 
  int rs1; 
  int rs2; 
  int funct7; 
  int imm11_0; 
  int imm11_5; 
  int imm12_10_5; 
  int imm31_12; 
  int imm20_10_1_11_19_12;
  int imm4_0; 
  int imm4_1_11; 
  int imm11_0_sign_extended; 
	struct node *next ;
} ;

struct set_t {
	struct node *head ;
	size_t n ; 
} ;

Set* set()
{
	Set *rv = malloc(sizeof(Set)) ;
	rv->n = 0 ; 
	//rv->head->next = NULL;
	return rv ;
}

size_t setSize( const Set* s ) 
{
	size_t length = 0 ; 
	
	for (const node *current = s->head ; current !=NULL ; current = current->next){
		length++;
	}
  
	return length ;
}
/*
bool setFind( const Set* s, const char* x ) //TODO: determine if this method is still needed and update this method for the new data items inside each node
{
	for (const node *current = s->head ; current !=NULL ; current = current->next){ 
		if ( strcmp(current->data , x) == 0)
			return true; //if true doesn't work, use 1
	}
	return false; //may use 0 instead
}
*/

bool setInsert( Set* s, int *fetch )
{
	node *newNode;
	newNode = malloc(sizeof(node));

  for(int i = 0; i<=32; i++){
      newNode->fetch[i] = fetch[i];
  }  

	newNode->next = s->head; 
	s->head = newNode ; 
	s->n = s->n + 1; 
	return true;
}

void setKill( Set* s ) 
{
	node *current = s->head;
	node *next = NULL;

	while (current != NULL){
		next = current->next;
    free(current->fetch);
    free(&current->opcode);
    free(&current->execResult);
    free(&current->rd);
    free(&current->rs1);
    free(&current->rs2);
    free(&current->funct3);
    free(&current->funct7);
    free(&current->imm11_0);
    free(&current->imm11_5);
    free(&current->imm12_10_5);
    free(&current->imm31_12);
    free(&current->imm20_10_1_11_19_12);
    free(&current->imm4_0);
    free(&current->imm4_1_11);
    free(&current->imm11_0_sign_extended);
		free(current);

		current = next;
	}
	//s->head = NULL ;  // MEMORY LEAK!
	free(s);
}
/*********************************************************************************************************************/

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
    
    // initialize register file

    for(int i = 0 ; i < sizeof(core->reg_file)/sizeof(core->reg_file[0]) ; i++){ 
        core->reg_file[i] = 0;
    }
    

    //core->data_mem[5]= -63; // 40->48 for -63
    //core->data_mem[5]= 193; //oops, too big
    //printf("This is the memboi: %d\n", core->data_mem[5] );
    //core->data_mem[6] = 63; // 48->56 for 63
    //core->data_mem[6] = 193; 
    // test configurations can be found below - please only uncomment 1 test configuration at a time in order to obtain "expected" output

    //-63 to 2's compliment: 1111111111111111111111111111111111111111111111111111111111000001

    core->reg_file[1] = 0;
    core->reg_file[2] = 10;
    core->reg_file[3] = -15;
    core->reg_file[4] = 20;
    core->reg_file[5] = 30;
    core->reg_file[6] = -35;

    core->data_mem[5]= -63;
    core->data_mem[6]= 63;


    /*
    //this is arr[2] = 8
    core->data_mem[16]= 8; //00000000 
    core->data_mem[17]= 0; //00000000 
    core->data_mem[18]= 0; //00000000 
    core->data_mem[19]= 0; //00000000 
    core->data_mem[20]= 0; //00000000 
    core->data_mem[21]= 0; //00000000 
    core->data_mem[22]= 0; //00000000 
    core->data_mem[23]= 0; //00000000 

    //this is arr[3] = 4
    core->data_mem[24]=4; //00000100
    core->data_mem[25]=0; //00000000 
    core->data_mem[26]=0; //00000000 
    core->data_mem[27]=0; //00000000 
    core->data_mem[28]=0; //00000000 
    core->data_mem[29]=0; //00000000 
    core->data_mem[30]=0; //00000000 
    core->data_mem[31]=0; //00000000 
    */
    return core;
}

int opcode_parse(int * imme_binary){
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
  //printf("This is the deciopcde: %d", deci_opcode);
  return deci_opcode;
} 

// code block below finds the Funct 3 in decimal
int funct3Parse(int instr[]){ 
  char deci_2[9] = "\0";
  char a_1[2] = "1";
  char a_0[2] = "0";
  for(int j=14; j>=12; j--){ // loop runs 3 times for 3 bits
      if(instr[j] == 0){
          strcat(deci_2, a_0);
      }
      else{
          strcat(deci_2, a_1);
      }
  }
  int funct3_binary = atoi(deci_2);
  int deci_funct3 = bin_to_dec(funct3_binary);
  return deci_funct3;
}

// code block below finds the Funct 7 in decimal 
int funct7Parse(int instr[]){
  char deci_3[12] = "\0";
  char a_1[2] = "1";
  char a_0[2] = "0";
  for(int j=31; j>=25; j--){
      if(instr[j] == 0){
          strcat(deci_3, a_0);
      }
      else{
          strcat(deci_3, a_1);
      }
  }
  int funct7_binary = atoi(deci_3);
  int deci_funct7 = bin_to_dec(funct7_binary);
  return deci_funct7;
}

//getting register 1 in decimal form 
int rs1Parse(int instr[], Core *core){
  char deci_reg1[12] = "\0";
  char a_1[2] = "1";
  char a_0[2] = "0";
  for(int j=19; j>=15; j--){
      if(instr[j] == 0){
          strcat(deci_reg1, a_0);
      }
      else{
          strcat(deci_reg1, a_1);
      }
  }
  int reg1_binary = atoi(deci_reg1);
  int deci_reg1_comp = bin_to_dec(reg1_binary);
  int reg1 = core->reg_file[deci_reg1_comp];
  return deci_reg1_comp;
}

//getting register 2 in decimal form
int rs2Parse(int instr[]){
    char deci_reg2[12] = "\0";
    char a_1[2] = "1";
    char a_0[2] = "0";
    for(int j=24; j>=20; j--){
        if(instr[j] == 0){
            strcat(deci_reg2, a_0);
        }
        else{
            strcat(deci_reg2, a_1);
        }
    }
    int reg2_binary = atoi(deci_reg2);
    int deci_reg2_comp = bin_to_dec(reg2_binary);
    return deci_reg2_comp;
}

//getting rd in decimal form
int rdParse(int instr[]){
  char deci_rd[12] = "\0";
  char a_1[2] = "1";
  char a_0[2] = "0";
  for(int j=11; j>=7; j--){
      if(instr[j] == 0){
          strcat(deci_rd, a_0);
      }
      else{
          strcat(deci_rd, a_1);
      }
  }
  int rd_binary = atoi(deci_rd);
  int deci_rd_comp = bin_to_dec(rd_binary);
  return deci_rd_comp;
}

//if (deci_opcode == 19 || deci_opcode == 3)
int *Itype_immediate(int instrFetch[]){
  // code block below finds the Immediate in decimal for i-types
  static int deci_imm[12];
  int counter = 11;
  for(int j=31; j>=20; j--){
      if(instrFetch[j] == 0){
          deci_imm[counter] = 0;
      }
      else{
          deci_imm[counter] = 1;
      }
      counter--;
  }
  return deci_imm;
}

// immediate generation for bne instruction
//else if (deci_opcode == 99)
/*
//function commented out as not strictly required for project 4
int bne(){
  // code block below finds the Immediate in decimal for bne
  //getting register 2 in decimal form
  char deci_reg2[16] = "\0";
  char a_1[2] = "1";
  char a_0[2] = "0";
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
  return sign_extended;
}
*/
//STEP 2.5
//calling the ALU function

//Calling ALU for R type instructions
void RType(struct node * curInstr, Core *core, ControlSignals *signals){
    int reg2 = curInstr->rs2;
    int rd = curInstr->rd;
    int reg1 = curInstr->rs1;

    //calling the ALU function
    int64_t alu_signal; //TODO: this is supposed to be of the typedeffed "Signal" type
    alu_signal = ALUControlUnit(signals->ALUOp, curInstr->funct7, curInstr->funct3);
    int64_t *ALU_result = malloc(sizeof(int64_t));; //long int *ALU_result = 0;
    int64_t *zero = malloc(sizeof(int64_t));; //long int *zero = 0;
    ALU(core->reg_file[reg1], core->reg_file[reg2], alu_signal, ALU_result, zero); 

    //core->reg_file[rd] = *ALU_result;  // save this line for next stage?
    curInstr->execResult = *ALU_result; 
}

//For ADDI
//else if(deci_opcode == 19 && deci_funct3 == 0)
void addi(struct node * curInstr, Core *core){
    //adding the immediate with reg1 
    int rd = curInstr->rd;
    int reg1 = curInstr->rs1;
    int immediate = curInstr->imm11_0;
    int temp = 0;
    temp = immediate + reg1;
    core->reg_file[rd] = temp;
}

// for slli
//else if(deci_opcode == 19 && deci_funct3 == 1)
void slli(struct node * curInstr, Core *core){
    int rd = curInstr->rd;
    int reg1 = curInstr->rs1;
    int sign_extended = curInstr->imm11_0;
    int temp = 0;
    temp = reg1 << sign_extended;
    core->reg_file[rd] = temp;
}

// for LD
void ldExec(struct node * curInstr, Core *core, ControlSignals *signals){

    int immediate = curInstr->imm11_0_sign_extended;
    int address_offset = immediate/8;
    int rd = curInstr->rd;
    int reg1 = curInstr->rs1;
    //calling the ALU function 
    int64_t alu_signal;
    alu_signal = ALUControlUnit(signals->ALUOp, curInstr->funct7, curInstr->funct3);
    int64_t *ALU_result = malloc(sizeof(int64_t));; 
    int64_t *zero = malloc(sizeof(int64_t));; 
    ALU(core->reg_file[reg1], address_offset, alu_signal, ALU_result, zero); 

    curInstr->memAddr = *ALU_result;
}

void ldMem(struct node * curInstr, Core *core, ControlSignals *signals){
    //==========================================================
    //Start of MEM Stage by obtaining the memory adress

    //grabbing the data stored at the address
    int mem_value = 0;
    mem_value = core->data_mem[curInstr->memAddr];

    //convert mem value to binary 
    
    //printf("This is memvalue: %d\n", mem_value);
    int * data;
    data = binary(mem_value);
    /*
    for(int g = 31; g >= 0; g--){
        printf("%d", data[g]);
      }
      printf("\n");
    */
    if(data[7] == 1){

      int mem_final = mem_value - 256;
      curInstr->execResult = mem_final;
    }
    else{
      //printf("This is memvalue: %d\n", mem_value);
      int mem_final = mem_value;
      curInstr->execResult = mem_final;
    }
    //printf("This is the curInstr->memAddr: %d\n", curInstr->memAddr);
}
    //==========================================================
void WB(struct node * curInstr, Core *core, ControlSignals *signals){
    int rd = curInstr->rd;
    core->reg_file[rd] = curInstr->execResult;
}

bool tickFunc(Core *core)
{  
    Signal select = 1;
    int sign_extended;

    int pipe_cycle = 1;
    ControlSignals *signals = (ControlSignals *)malloc(sizeof(ControlSignals));

    Set *pipeInstrs = set();

    //figuring out how many clock cycles will be in the pipeline 
    int total_cycles = 0;
    
    total_cycles = 5 + ((core->instr_mem->last->addr / 4) - 1); //TODO: evaulate if core->instr_mem->last->addr should return either 20 or 19(that'd be a problem)

    struct node* curDecode = NULL; //the current node to be decoded
    struct node* curExecute = NULL; //the current node to be executed
    int curOpCode;
    int* deci_imm;

    char deci_1[9] = "\0";
    char a_1[2] = "1";
    char a_0[2] = "0";
    
    unsigned instruction = 0;

    // creating pipeline while-loop
    while(pipe_cycle <= 10){
      // Steps may include
      // (Step 1) Reading instruction from instruction memory

      // Are we reaching the final instruction?
      //if (core->PC >! core->instr_mem->last->addr)
      //{
        instruction = core->instr_mem->instructions[core->PC / 4].instruction;
        //printf("This is the instruction %d:\n", instruction);
        // increment the PC
        if (select == 1){
        core->PC += 4;
        }
        else{
          core->PC += curExecute->imm11_0_sign_extended * 4;
        }
        ++core->clk;
      //}
      
      // (Step 2) ...
      // the instructions are coming in as decimal numbers
      
      //STEP 2.1: converting the decimal values to binary
      int *imme_binary; // full 32-bit instruction
      //checking if instruction is a negative number 
          if (instruction < 0) {
              imme_binary = signedBinary(instruction);
          }
          else {
              imme_binary = binary(instruction);
          }
      
      //Here is where the binary form of instruction is stored in the pipeline linked list 

      // FETCH
      if(pipe_cycle >= 1 && pipe_cycle <= 5){ //TODO: make the int values of this if-statment variable dependentso they work for any number of instructions
        setInsert(pipeInstrs, imme_binary);
      }

      // DECODE 
      if(pipe_cycle >=2 && pipe_cycle <=6){//TODO: make the int values of this if-statment variable dependent so they work for any number of instructions

        if ( pipe_cycle <= 5 ){
          curDecode = pipeInstrs->head->next;
        }
        else{
          curDecode = pipeInstrs->head;
        }

        curDecode->opcode = opcode_parse(curDecode->fetch);
        
        //printf("THis is the curdecode opcode: %d\n", curDecode->opcode);
        
        //filtering for I-type Instructions
        if (curDecode->opcode == 3 || curDecode->opcode == 15 || curDecode->opcode == 19 || curDecode->opcode == 115){
          curDecode->rd = rdParse(curDecode->fetch);
          curDecode->funct3 = funct3Parse(curDecode->fetch);
          curDecode->rs1 = rs1Parse(curDecode->fetch, core);
        } 
        //filtering for R-type Instructions
        if (curDecode->opcode == 51 || curDecode->opcode == 59){
          curDecode->rd = rdParse(curDecode->fetch);
          curDecode->funct3 = funct3Parse(curDecode->fetch);
          curDecode->rs1 = rs1Parse(curDecode->fetch, core);
          curDecode->rs2 = rs2Parse(curDecode->fetch);
          curDecode->funct7 = funct7Parse(curDecode->fetch);
        }
      }

      // EXECUTION 
      if(pipe_cycle >=3 && pipe_cycle <=7){//TODO: make the int values of this if-statment variable dependent so they work for any number of instructions

        //calling the control unit function 
        int deci_opcode = curDecode->opcode;
        ControlUnit(deci_opcode, signals);

        if ( pipe_cycle <= 5 ){
          curExecute = pipeInstrs->head->next->next;
        }
        if ( pipe_cycle == 5 + 1 ) { //catches Cycle 7
          curExecute = pipeInstrs->head->next;
        }
        if ( pipe_cycle == 5 + 2 ) { //catches Cycle 8
          curExecute = pipeInstrs->head;
        }

        curOpCode = curExecute->opcode;
        
        if (curOpCode == 51){
          RType(curExecute, core, signals);
        }
        // check for i type
        else if (curOpCode == 19 || curOpCode == 3){
          int *binary_immediate;
          binary_immediate = Itype_immediate(curExecute->fetch);
          
          //copying this to an array
          int deci_imm[12];
          for(int j = 11; j>=0; j--){
            deci_imm[j] = binary_immediate[j];
          }
          
          sign_extended = ImmeGen(deci_imm);
          curExecute->imm11_0 = *binary_immediate;
          curExecute->imm11_0_sign_extended = sign_extended;

          // ld function call
          if (curExecute->funct3 == 3){
            ldExec(curExecute, core, signals);
          }
        }
      }

      // MEMORY
      if(pipe_cycle >=4 && pipe_cycle <=8){//TODO: make the int values of this if-statment variable dependentso they work for any number of instructions

        //calling the control unit function 
        int deci_opcode = curDecode->opcode;
        ControlUnit(deci_opcode, signals);

        //catchs cycles 4 and 5
        if ( pipe_cycle <= 5 ){
          curExecute = pipeInstrs->head->next->next->next;
        }
        if ( pipe_cycle == 5 + 1 ) { //catches Cycle 6
          curExecute = pipeInstrs->head->next->next;
        }
        if ( pipe_cycle == 5 + 2 ) { //catches Cycle 7
          curExecute = pipeInstrs->head->next;
        }
        if ( pipe_cycle == 5 + 3 ) { //catches Cycle 8
          curExecute = pipeInstrs->head;
        }

        curOpCode = curExecute->opcode;
        //printf("This is the opcode: %d\n", curOpCode);
        
        // check for i type
        if (curOpCode == 19 || curOpCode == 3){
          
          // ld function call
          if (curExecute->funct3 == 3){
            ldMem(curExecute, core, signals);
          }
        }
      }

      // WRITE BACK
      if(pipe_cycle >=5 && pipe_cycle <=9){//TODO: make the int values of this if-statment variable dependentso they work for any number of instructions

        int deci_opcode = curDecode->opcode;
        ControlUnit(deci_opcode, signals);

        //catchs cycle 5
        if ( pipe_cycle == 5 ){
          curExecute = pipeInstrs->head->next->next->next->next;
        }
        if ( pipe_cycle == 5 + 1 ) { //catches Cycle 6
          curExecute = pipeInstrs->head->next->next->next;
        }
        if ( pipe_cycle == 5 + 2 ) { //catches Cycle 7
          curExecute = pipeInstrs->head->next->next;
        }
        if ( pipe_cycle == 5 + 3 ) { //catches Cycle 8
          curExecute = pipeInstrs->head->next;
        }
        if ( pipe_cycle == 5 + 4 ) { //catches Cycle 9
          curExecute = pipeInstrs->head;
        }

        curOpCode = curExecute->opcode;
        //printf("This is the opcode: %d\n", curOpCode);  
        WB(curExecute, core, signals);
          
      }

      pipe_cycle++;
      if (pipe_cycle == 10){
        return false;
      }
    }
 
    //STEP 2.2
    //sending the first seven bits (opcode) to the control unit 

    // (Step N) Increment PC.
    // TODO: we're updating the PC in the main while loop,
    // so the below code probably needs to be removed
    /*
    if (select == 1){
      core->PC += 4;
    }
    else{
      core->PC += sign_extended * 4;
    }
    ++core->clk;
    */
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
    char deci_chars[66] = "\0";
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

