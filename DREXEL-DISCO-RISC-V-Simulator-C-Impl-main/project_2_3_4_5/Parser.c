#include "Parser.h"

// FIXME, implement this function.
// Here shows an example on how to translate "add x10, x10, x25"
void loadInstructions(Instruction_Memory *i_mem, const char *trace)
{
    printf("Loading trace file: %s\n", trace);

    FILE *fd = fopen(trace, "r");
    if (fd == NULL)
    {
        perror("Cannot open trace file. \n");
        exit(EXIT_FAILURE);
    }

    // Iterate all the assembly instructions
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    Addr PC = 0; // program counter points to the zeroth location initially.
    int IMEM_index = 0;
    while ((read = getline(&line, &len, fd)) != -1)
    {
        // Assign program counter
        i_mem->instructions[IMEM_index].addr = PC;

        // Extract operation for R type
        char *raw_instr = strtok(line, " ");
        if (strcmp(raw_instr, "add") == 0 ||
            strcmp(raw_instr, "sub") == 0 ||
            strcmp(raw_instr, "sll") == 0 ||
            strcmp(raw_instr, "srl") == 0 ||
            strcmp(raw_instr, "xor") == 0 ||
            strcmp(raw_instr, "or")  == 0 ||
            strcmp(raw_instr, "and") == 0)
        {
            parseRType(raw_instr, &(i_mem->instructions[IMEM_index]));
            i_mem->last = &(i_mem->instructions[IMEM_index]);
	    
        }
        //calling the I type instruction
        else if(strcmp(raw_instr, "addi") == 0 ||
                strcmp(raw_instr, "ld")   == 0 ||
                strcmp(raw_instr, "slli") == 0)
          {
            parseIType(raw_instr, &(i_mem->instructions[IMEM_index]));
            i_mem->last = &(i_mem->instructions[IMEM_index]);
        }

        //calling the SB type instruction
        else if(strcmp(raw_instr, "bne") == 0){
            parseSBType(raw_instr, &(i_mem->instructions[IMEM_index]));
            i_mem->last = &(i_mem->instructions[IMEM_index]);
        }

        IMEM_index++;
        PC += 4;
    }

    fclose(fd);
}

void parseRType(char *opr, Instruction *instr)
{
    instr->instruction = 0;
    unsigned opcode = 0;
    unsigned funct3 = 0;
    unsigned funct7 = 0;

    if (strcmp(opr, "add") == 0)
    {
        opcode = 51;
        funct3 = 0;
        funct7 = 0;
    }

    else if (strcmp(opr, "sub") == 0)
    {
        opcode = 51;
        funct3 = 0;
        funct7 = 32;
    }

    char *reg = strtok(NULL, ", ");
    unsigned rd = regIndex(reg);

    reg = strtok(NULL, ", ");
    unsigned rs_1 = regIndex(reg);

    reg = strtok(NULL, ", ");
    reg[strlen(reg)-1] = '\0';
    unsigned rs_2 = regIndex(reg);

    // Contruct instruction
    instr->instruction |= opcode;
    instr->instruction |= (rd << 7);
    instr->instruction |= (funct3 << (7 + 5));
    instr->instruction |= (rs_1 << (7 + 5 + 3));
    instr->instruction |= (rs_2 << (7 + 5 + 3 + 5));
    instr->instruction |= (funct7 << (7 + 5 + 3 + 5 + 5));
}

void parseIType(char *opr, Instruction *instr)
{
    //intializing the variables
    instr->instruction = 0;
    unsigned opcode = 0;
    unsigned funct3 = 0;
    unsigned funct6 = 0;
    unsigned funct7 = 0;

    //setting the standard code for addi
    if (strcmp(opr, "addi") == 0)
    {
        opcode = 19;
        funct3 = 0;

        //finding the rd 
        char *reg = strtok(NULL, ", ");
        unsigned rd = regIndex(reg);

        //finding the rs1
        reg = strtok(NULL, ", ");
        unsigned rs_1 = regIndex(reg);

        
        //finding the immediate
        reg = strtok(NULL, ", ");
        //reg[strlen(reg)-1] = '\0';
        unsigned int imme = atoi(reg);

        // Contruct instruction
        //each instruction is offset by it's length to full construct it
        instr->instruction |= opcode; // no offset
        instr->instruction |= (rd << 7); // offset by 7
        instr->instruction |= (funct3 << (12)); // offset by 12
        instr->instruction |= (rs_1 << (15)); //offset by 15
        instr->instruction |= (imme << (20)); // offset by 20
    }
    
    else if(strcmp(opr, "ld") == 0){
        opcode = 3;
        funct3 = 3;

        char *imme_reg;

        //creating a copy of the token and a copied tokenizer
        char copy_instruct[100];
        char *copied_token;

        //finding the rd 
        char *reg = strtok(NULL, ", ");
        unsigned rd = regIndex(reg);

        //moving the token forward
        reg = strtok(NULL, ", ");

        //copying the token
        sprintf(copy_instruct, "%s", reg);

        //tokenizing the copied instruction to get the immediate
        copied_token = strtok(copy_instruct, "(");
        unsigned imme = atoi(copied_token);

        //tokenizing the copied instruction to get the register 
        copied_token = strtok(NULL, ")");
        unsigned rs_1 = regIndex(copied_token);

        // Contruct instruction
        //each instruction is offset by it's length to full construct it
        instr->instruction |= opcode; // no offset
        instr->instruction |= (rd << 7); // offset by 7
        instr->instruction |= (funct3 << (12)); // offset by 12
        instr->instruction |= (rs_1 << (15)); //offset by 15
        instr->instruction |= (imme << (20)); // offset by 20
    }

    //setting the standard code for slli
    else if (strcmp(opr, "slli") == 0)
    {
        opcode = 19;
        funct3 = 1;
        funct6 = 0;

        //finding the rd 
        char *reg = strtok(NULL, ", ");
        unsigned rd = regIndex(reg);

        //finding the rs1
        reg = strtok(NULL, ", ");
        unsigned rs_1 = regIndex(reg);

        
        //finding the immediate
        reg = strtok(NULL, ", ");
        unsigned int imme = atoi(reg);

        // Contruct instruction
        //each instruction is offset by it's length to full construct it
        instr->instruction |= opcode; // no offset
        instr->instruction |= (rd << 7); // offset by 7
        instr->instruction |= (funct3 << (12)); // offset by 12
        instr->instruction |= (rs_1 << (15)); //offset by 15
        instr->instruction |= (imme << (20)); // offset by 20
        instr->instruction |= (funct6 << (6)); //offset by 26

    }
}

void parseSBType(char *opr, Instruction *instr){
    //intializing the variables
    instr->instruction = 0;
    unsigned opcode = 0;
    unsigned funct3 = 0;
    unsigned funct7 = 0;
    int *imme_binary;

    if (strcmp(opr, "bne") == 0){
        opcode = 99;
        funct3 = 1;

        //finding the rs1 
        char *reg = strtok(NULL, ", ");
        unsigned rs_1 = regIndex(reg);

        //finding the rs2
        reg = strtok(NULL, ", ");
        unsigned rs_2 = regIndex(reg);

        //finding the immediate
        reg = strtok(NULL, ", ");
        int imme = atoi(reg);

        //checking if imme is a negative number 
        if (imme < 0) {
            imme = imme * -1;
            imme_binary = signedBinary(imme);
        }
        else {
            imme_binary = binary(imme);
        }

        
      
        // Contruct instruction
        //each instruction is offset by it's length to full construct it
        instr->instruction |= opcode; // no offset

        instr->instruction |= (imme_binary[10] << 7); // offset by 7
        instr->instruction |= (imme_binary[0] << 8); // offset by 8 
        instr->instruction |= (imme_binary[1] << 9); // offset by 9
        instr->instruction |= (imme_binary[2] << 10); // offset by 10
        instr->instruction |= (imme_binary[3] << 11); // offset by 11

        instr->instruction |= (funct3 << (12)); // offset by 12
        instr->instruction |= (rs_1 << (15)); //offset by 15
        instr->instruction |= (rs_2 << (20)); // offset by 20

        instr->instruction |= (imme_binary[11] << (25)); //offset by 25
        instr->instruction |= (imme_binary[9] << (26)); //offset by 26
        instr->instruction |= (imme_binary[8] << (27)); //offset by 27
        instr->instruction |= (imme_binary[7] << (28)); //offset by 28
        instr->instruction |= (imme_binary[6] << (29)); //offset by 29
        instr->instruction |= (imme_binary[5] << (30)); //offset by 30
        instr->instruction |= (imme_binary[4] << (31)); //offset by 31    
    }
}

int regIndex(char *reg)
{
    unsigned i = 0;
    for (i; i < NUM_OF_REGS; i++)
    {
        if (strcmp(REGISTER_NAME[i], reg) == 0)
        {
            break;
        }
    }

    return i;
}

//creating the signed binary function
int *signedBinary(int n){
    int c, k;

    //defining the array
    static int binary[32];

    //creating the binary number
    for (c = 31; c >= 0; c--)
    {
        k = n >> c;

        if (k & 1){
        //printf("1");
        binary[c] = 1;
        }
        else{
        //printf("0");
        binary[c] = 0;
        }
    }

    //flipping the bits of the binary number 
    for(int i = 31; i >= 0; i--){
        if (binary[i] == 1){
            binary[i] = 0;
        }
        else{
            binary[i] = 1;
        }
    }

    //adding the one to the number
    for(int h = 0; h < 12; h++){
        if (binary[h] == 1){
            binary[h] = 0;
        }
        else{
            binary[h] = 1;
            break;
        }
    }

    return binary;
}

//creating the non negative binary
int *binary(int n){
    int c, k;

    //defining the array
    static int binary[32];

    //creating the binary number
    for (c = 31; c >= 0; c--)
    {
        k = n >> c;

        if (k & 1){
        //printf("1");
        binary[c] = 1;
        }
        else{
        //printf("0");
        binary[c] = 0;
        }
    }

    return binary;
}