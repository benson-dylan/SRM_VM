#include <stdio.h>
#include <stdlib.h>
#include "instruction.h"
#include "machine_types.h"
#include "utilities.h"
#include "regname.h"
#include "bof.h"
#include "machine.h"

#define MEMORY_SIZE_IN_BYTES (65536 - BYTES_PER_WORD)
#define HI 0
#define LO 1

static union mem_u {
    byte_type bytes[MEMORY_SIZE_IN_BYTES];
    word_type words[MEMORY_SIZE_IN_BYTES / BYTES_PER_WORD];
    bin_instr_t instrs[MEMORY_SIZE_IN_BYTES / BYTES_PER_WORD];
} memory;

int PC;
int GPR[NUM_REGISTERS];
int HI_LO[2];

void initialize_registers(BOFHeader bin_header)
{
    PC = bin_header.text_start_address;
    GPR[GP] = bin_header.data_start_address;
    GPR[FP] = bin_header.stack_bottom_addr;
    GPR[SP] = bin_header.stack_bottom_addr; 
}

void load_words(BOFFILE bf, word_type start_address, word_type read_length)
{
    for (int i = start_address; i < start_address + (read_length / BYTES_PER_WORD); i++)
        memory.words[i] =  bof_read_word(bf);
}

void print_GPR(int * GPR)
{
    printf("PC: %d\n", PC);

    for (int i = 0; i < 32; i++)
    {
        if (i % 6 == 0 && i != 0)
            printf("\n");
        printf("GPR[%s]: %d   ", regname_get(i), GPR[i]);
    }

    printf("\n");
}

int main(int argc, char *argv[])  
{   
    bin_instr_t IR;
    bool JUMP = false;

    BOFFILE bf;
    if (argc == 2)
        bf = bof_read_open(argv[1]);
    else 
        bf = bof_read_open(argv[2]);

    // Initialize header values
    BOFHeader bfh = bof_read_header(bf);
    initialize_registers(bfh);

    // Validate header values
    if (PC % 4 != 0 || PC > GPR[GP])
        bail_with_error("Invalid PC start");
    if (GPR[GP] % 4 != 0)
        bail_with_error("Invalid data start");
    if (GPR[FP] % 4 != 0 || GPR[FP] < GPR[GP])
        bail_with_error("Invalid stack bottom");

    load_words(bf, PC, bfh.text_length); // Load instructions
    load_words(bf, GPR[GP], bfh.data_length); // Load data

    while (true)
    {
        print_GPR(GPR);
        JUMP = false;

        // Load instruction from memory
        IR = memory.instrs[PC / BYTES_PER_WORD];

        if (instruction_type(IR) == reg_instr_type)
        {
            // printf("Register Operation\n");

            reg_instr_t ri = IR.reg;
            switch (ri.func)
            {
                case ADD_F:
                    add_op(ri, GPR);
                    break;
                case SUB_F:
                    sub_op(ri, GPR);
                    break;
                case MUL_F:
                    mul_op(ri, GPR, HI_LO);
                    break;
                case DIV_F:
                    div_op(ri, GPR, HI_LO);
                    break;
                case MFHI_F:
                    mfhi_op(ri, GPR, HI_LO);
                    break;
                case MFLO_F:
                    mflo_op(ri, GPR, HI_LO);
                    break;
                case AND_F:
                    and_op(ri, GPR);
                    break;
                case BOR_F:
                    bor_op(ri, GPR);
                    break;
                case NOR_F:
                    nor_op(ri, GPR);
                    break;
                case XOR_F:
                    xor_op(ri, GPR);
                    break;
                case SLL_F:
                    sll_op(ri, GPR);
                    break;
                case SRL_F:
                    srl_op(ri, GPR);
                    break;
                case JR_F:
                    PC = jr_op(ri, GPR);
                    break;
                case SYSCALL_F:
                    break;
            }
        }

        else if (instruction_type(IR) == immed_instr_type)
        {
            // printf("Immediate Operation\n");

            immed_instr_t ii = IR.immed;
            switch (ii.op)
            {
                case ADDI_O:
                    addi_op(ii, GPR);
                    break;
                case ANDI_O:
                    andi_op(ii, GPR);
                    break;
                case BORI_O:
                    bori_op(ii, GPR);
                    break;
                case XORI_O:
                    xori_op(ii, GPR);
                    break;
                case BEQ_O:
                    if (GPR[ii.rs] == GPR[ii.rt])
                    {    
                        PC += branch_op(ii, GPR);
                        JUMP = true;
                    }
                    break;
                case BGEZ_O:
                    if (GPR[ii.rs] >= 0)
                    {
                        PC += branch_op(ii, GPR);
                        JUMP = true;
                    }
                    break;
                case BGTZ_O:
                    if (GPR[ii.rs] > 0)
                    {
                        PC += branch_op(ii, GPR);
                        JUMP = true;
                    }
                    break;
                case BLEZ_O:
                    if (GPR[ii.rs] <= 0)
                    {
                        PC += branch_op(ii, GPR);
                        JUMP = true;
                    }
                    break;
                case BLTZ_O:
                    if (GPR[ii.rs] < 0)
                    {
                        PC += branch_op(ii, GPR);
                        JUMP = true;
                    }
                    break;
                case BNE_O:
                    if (GPR[ii.rs] != GPR[ii.rt])
                    {
                        PC += branch_op(ii, GPR);
                        JUMP = true;
                    }
                    break;
                case LBU_O:
                    lbu_op(ii, GPR, memory.words);
                    break;
                case LW_O:
                    lw_op(ii, GPR, memory.words);
                    break;
                case SB_O:
                    sb_op(ii, GPR, memory.words);
                    break;
                case SW_O:
                    sw_op(ii, GPR, memory.words);
                    break;
            }
        }

        else if (instruction_type(IR) == jump_instr_type)
        {
            // printf("Jump Operation\n");

            jump_instr_t ji = IR.jump;
            switch (ji.op)
            {
                case JMP_O:
                    PC = jmp_op(ji, PC);
                    JUMP = true;
                    break;
                case JAL_O:
                    PC = jal_op(ji, PC, GPR);
                    JUMP = true;
                    break;
            }
        }

        else if (instruction_type(IR) == syscall_instr_type)
        {
            // printf("Syscall Operation\n");

            syscall_instr_t si = IR.syscall;
            switch (si.code)
            {
                case exit_sc:
                    return 0;
                    break;
                case print_str_sc:
                    break;
                case print_char_sc:
                    break;
                case read_char_sc:
                    break;
                case start_tracing_sc:
                    break;
                case stop_tracing_sc:
                    break;
            }
        }

        if (!JUMP)
            PC += 4;
    }    
}