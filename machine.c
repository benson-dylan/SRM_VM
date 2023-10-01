#include "machine.h"
#include "instruction.h"
#include "machine_types.h"

#define HI 0
#define LO 1
#define R_A0 4
#define R_V0 2

// Register Operations

void add_op(reg_instr_t ri, int *GPR)
{
    GPR[ri.rd] = GPR[ri.rs] + GPR[ri.rt];
}

void sub_op(reg_instr_t ri, int *GPR)
{
    GPR[ri.rd] = GPR[ri.rs] - GPR[ri.rt];
}

void mul_op(reg_instr_t ri, int *GPR, int *HI_LO)
{
    uint64_t result = GPR[ri.rt] *GPR[ri.rs];

    HI_LO[HI] = (int) (result >> 32);
    HI_LO[LO] = (int) result;
}

void div_op(reg_instr_t ri, int *GPR, int *HI_LO)
{
    HI_LO[HI] = GPR[ri.rs] % GPR[ri.rt];
    HI_LO[LO] = GPR[ri.rs] / GPR[ri.rt];
}

void mfhi_op(reg_instr_t ri, int *GPR, int *HI_LO)
{
    GPR[ri.rd] = HI_LO[HI];
}

void mflo_op(reg_instr_t ri, int *GPR, int *HI_LO)
{
    GPR[ri.rd] = HI_LO[LO];
}

void and_op(reg_instr_t ri, int *GPR)
{
    GPR[ri.rd] = GPR[ri.rs] & GPR[ri.rt];
}

void bor_op(reg_instr_t ri, int *GPR)
{
    GPR[ri.rd] = GPR[ri.rs] | GPR[ri.rt];
}

void nor_op(reg_instr_t ri, int *GPR)
{
    GPR[ri.rd] = ~(GPR[ri.rs] | GPR[ri.rt]);
}

void xor_op(reg_instr_t ri, int *GPR)
{
    GPR[ri.rd] = GPR[ri.rs] ^ GPR[ri.rt];
}

void sll_op(reg_instr_t ri, int *GPR)
{
    GPR[ri.rd] = GPR[ri.rt] << ri.shift;
}

void srl_op(reg_instr_t ri, int *GPR)
{
    GPR[ri.rd] = GPR[ri.rt] >> ri.shift;
}

int jr_op(reg_instr_t ri, int *GPR)
{
    return GPR[ri.rs];
}

// Immediate Operations

void addi_op(immed_instr_t ii, int *GPR)
{
    GPR[ii.rt] = GPR[ii.rs] + machine_types_sgnExt(ii.immed);
}

void andi_op(immed_instr_t ii, int *GPR)
{
    GPR[ii.rt] = GPR[ii.rs] & machine_types_zeroExt(ii.immed);
}

void bori_op(immed_instr_t ii, int *GPR)
{
    GPR[ii.rt] = GPR[ii.rs] | machine_types_zeroExt(ii.immed);
}

void xori_op(immed_instr_t ii, int *GPR)
{
    GPR[ii.rt] = GPR[ii.rs] ^ machine_types_zeroExt(ii.immed);
}

int branch_op(immed_instr_t ii, int *GPR)
{
    return machine_types_formOffset(ii.immed) + 4;
}

void lbu_op(immed_instr_t ii, int *GPR, byte_type *memory)
{
    GPR[ii.rt] = machine_types_zeroExt(memory[GPR[ii.rs] + machine_types_formOffset(ii.immed)]);
}

void lw_op(immed_instr_t ii, int *GPR, word_type *memory)
{
    GPR[ii.rt] = memory[(GPR[ii.rs] + machine_types_formOffset(ii.immed)) / BYTES_PER_WORD];
}

void sb_op(immed_instr_t ii, int *GPR, byte_type *memory)
{
    memory[GPR[ii.rs] + machine_types_formOffset(ii.immed)] = (byte_type)(0xFF & GPR[ii.rt]);
}

void sw_op(immed_instr_t ii, int *GPR, word_type *memory)
{
    memory[(GPR[ii.rs] + machine_types_formOffset(ii.immed)) / BYTES_PER_WORD] = GPR[ii.rt];
}

// Jump Operations

int jmp_op(jump_instr_t ji, int PC)
{
    int jumpAddr = machine_types_formAddress(PC, ji.addr);
    return jumpAddr;
}

int jal_op(jump_instr_t ji, int PC, int *GPR)
{
    int jumpAddr = machine_types_formAddress(PC, ji.addr);
    GPR[31] = PC + 4;
    return jumpAddr;
}

// System Calls

void pstr_call(int *GPR, word_type *memory) 
{
    char *str = (char *)&memory[GPR[R_A0]];
    int res = printf("%s", str);
    GPR[R_V0] = res;
}

void pch_call(int *GPR) 
{
    GPR[R_V0] = fputc(GPR[R_A0], stdout);
}

void rch_call(int *GPR) 
{
    GPR[R_V0] = getc(stdin);
}

