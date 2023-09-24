#include "machine.h"
#include "instruction.h"
#include "machine_types.h"

#define HI 0
#define LO 1

// Register Operations

void add_op(reg_instr_t ri, int * GPR)
{
    GPR[ri.rd] = GPR[ri.rs] + GPR[ri.rt];
}

void sub_op(reg_instr_t ri, int * GPR)
{
    GPR[ri.rd] = GPR[ri.rs] - GPR[ri.rt];
}

void mul_op(reg_instr_t ri, int * GPR, int * HI_LO)
{
    uint64_t result = GPR[ri.rt] * GPR[ri.rs];

    HI_LO[HI] = (int) (result >> 32);
    HI_LO[LO] = (int) result;
}

void div_op(reg_instr_t ri, int * GPR, int * HI_LO)
{
    HI_LO[HI] = GPR[ri.rs] % GPR[ri.rt];
    HI_LO[LO] = GPR[ri.rs] / GPR[ri.rt];
}

void mfhi_op(reg_instr_t ri, int * GPR, int * HI_LO)
{
    GPR[ri.rd] = HI_LO[HI];
}

void mflo_op(reg_instr_t ri, int * GPR, int * HI_LO)
{
    GPR[ri.rd] = HI_LO[LO];
}

void and_op(reg_instr_t ri, int * GPR)
{
    GPR[ri.rd] = GPR[ri.rs] & GPR[ri.rt];
}

void bor_op(reg_instr_t ri, int * GPR)
{
    GPR[ri.rd] = GPR[ri.rs] | GPR[ri.rt];
}

void nor_op(reg_instr_t ri, int * GPR)
{
    GPR[ri.rd] = ~(GPR[ri.rs] | GPR[ri.rt]);
}

void xor_op(reg_instr_t ri, int * GPR)
{
    GPR[ri.rd] = GPR[ri.rs] ^ GPR[ri.rt];
}

void sll_op(reg_instr_t ri, int * GPR)
{
    GPR[ri.rd] = GPR[ri.rt] << ri.shift;
}

void srl_op(reg_instr_t ri, int * GPR)
{
    GPR[ri.rd] = GPR[ri.rt] >> ri.shift;
}

int jr_op(reg_instr_t ri, int * GPR)
{
    return GPR[ri.rs];
}

// Immediate Operations

void addi_op(immed_instr_t ii, int * GPR)
{
    GPR[ii.rt] = GPR[ii.rs] + machine_types_sgnExt(ii.immed);
}

// Jump Operations

int jmp_op(jump_instr_t ji, int PC)
{
    return machine_types_formAddress(PC, ji.addr);
}

int jal_op(jump_instr_t ji, int PC, int * GPR)
{
    GPR[31] = PC;
    return machine_types_formAddress(PC, ji.addr);
}