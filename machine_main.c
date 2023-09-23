#include <stdio.h>
#include "instruction.h"
#include "machine_types.h"
#include "utilities.h"
#include "regname.h"
#include "bof.h"

#define MEMORY_SIZE_IN_BYTES (65536 - BYTES_PER_WORD)

int PC;
int GPR[NUM_REGISTERS];
bin_instr_t IR[MEMORY_SIZE_IN_BYTES / BYTES_PER_WORD];

void initialize_registers(BOFHeader bin_header)
{
    PC = bin_header.text_start_address;
    GPR[GP] = bin_header.data_start_address;
    GPR[FP] = bin_header.stack_bottom_addr;
    GPR[SP] = bin_header.stack_bottom_addr;
}

void fill_instruction_reg(BOFFILE bf, BOFHeader bin_header)
{
    for (int i = PC; i < PC + (bin_header.text_length / BYTES_PER_WORD); i++)
        IR[i] = instruction_read(bf);
}

int main(int argc, char *argv[])  
{
    BOFFILE bf;
    if (argc == 2)
        bf = bof_read_open(argv[1]);
    else 
        bf = bof_read_open(argv[2]);

    BOFHeader bfh = bof_read_header(bf);
    initialize_registers(bfh);

    if (PC % 4 != 0 || PC > GPR[GP])
        bail_with_error("Invalid PC start");
    if (GPR[GP] % 4 != 0)
        bail_with_error("Invalid data start");
    if (GPR[FP] % 4 != 0 || GPR[FP] < GPR[GP])
        bail_with_error("Invalid stack bottom");

    // Read in instructions from BOF to IR
    fill_instruction_reg(bf, bfh);
    
}