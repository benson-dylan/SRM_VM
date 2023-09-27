#ifndef _MACHINE_H
#define _MACHINE_H
#include "instruction.h"

// Register Functions
void add_op(reg_instr_t ri, int *GPR);

void sub_op(reg_instr_t ri, int *GPR);

void mul_op(reg_instr_t ri, int *GPR, int *HI_LO);

void div_op(reg_instr_t ri, int *GPR, int *HI_LO);

void mfhi_op(reg_instr_t ri, int *GPR, int *HI_LO);

void mflo_op(reg_instr_t ri, int *GPR, int *HI_LO);

void and_op(reg_instr_t ri, int *GPR);

void bor_op(reg_instr_t ri, int *GPR);

void nor_op(reg_instr_t ri, int *GPR);

void xor_op(reg_instr_t ri, int *GPR);

void sll_op(reg_instr_t ri, int *GPR);

void srl_op(reg_instr_t ri, int *GPR);

int jr_op(reg_instr_t ri, int *GPR);

//Immediate Functions
void addi_op(immed_instr_t ii, int *GPR);

void andi_op(immed_instr_t ii, int *GPR);

void bori_op(immed_instr_t ii, int *GPR);

void xori_op(immed_instr_t ii, int *GPR);

int branch_op(immed_instr_t ii, int *GPR);

void lbu_op(immed_instr_t ii, int *GPR, word_type *memory);

void lw_op(immed_instr_t ii, int *GPR, word_type *memory);

void sb_op(immed_instr_t ii, int *GPR, word_type *memory);

void sw_op(immed_instr_t ii, int *GPR, word_type *memory);

// Jump Operations

int jmp_op(jump_instr_t ji, int PC);

int jal_op(jump_instr_t ji, int PC, int *GPR);

// System Calls 

void exit_call();

void pstr_call(int* GPR, word_type *memory);

void pch_call(int* GPR, word_type *memory);

void rch_call(int *GPR, word_type *memory);

// missing stra and notr calls 

#endif
