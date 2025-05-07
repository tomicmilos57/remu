#ifndef CPU_h
#define CPU_h
#include "../MEM/mem.h"
#include <cstdint>

class CPU {

public:
  CPU(MEM& memory);
  CPU(MEM& memory, uint32_t pc);

  bool execute();
  void info_registers();
  void info_pc();
  void info_ir();
  void info_instruction_number();

private:
  enum instruction {
    i_lui, i_auipc,
    i_jal, i_jalr,
    i_lb, i_lh, i_lw, i_lbu, i_lhu,
    i_addi, i_slti, i_sltiu, i_xori, i_ori, i_andi,
    i_slli, i_srli, i_srai,
    i_add, i_sub, i_sll, i_slt, i_sltu, i_xor, i_srl, i_sra, i_or, i_and,
    i_beq, i_bne, i_blt, i_bge, i_bltu, i_bgeu,
    i_sb, i_sh, i_sw,
    i_fence, i_fence_i, i_ecall, i_ebreak,
    i_invalid_instruction
  };

  MEM& memory;
  uint32_t pc = 0x00000000;
  uint32_t ir = 0;
  uint32_t instruction_number = 0;

  uint32_t regfile[32] = {};

  instruction decode_instruction();
  void execute_instruction(instruction inst);
};

#endif
