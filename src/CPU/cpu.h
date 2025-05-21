#ifndef CPU_h
#define CPU_h
#include "../MEM/mem.h"
#include "csr.h"
#include <cstdint>
#include <string>
#include <fstream>

class CPU {

public:
  CPU(MEM& memory);
  CPU(MEM& memory, uint32_t pc);
  ~CPU();

  bool execute();
  bool handle_interrupt();
  void info_registers();
  void info_csr_registers();
  void info_pc();
  void info_ir();
  void info_instruction_number();
  void info_unpriv_test(); // EXPECTS MEMORY ON ADDRESS 0x60000000

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
    i_mul, i_mulh, i_mulhsu, i_mulhu, i_div, i_divu, i_rem, i_remu,
    i_fence, i_fence_i, i_sfence_vma, i_wfi,
    i_ecall, i_ebreak, i_mret, i_sret, i_uret,
    i_csrrw, i_csrrs,  i_csrrc,  i_csrrwi, i_csrrsi, i_csrrci,
    i_amoswap_w, i_amoadd_w, i_amoxor_w, i_amoand_w,
    i_amoor_w, i_amomin_w, i_amomax_w, i_amominu_w, i_amomaxu_w,
    i_invalid_instruction
  };

  MEM& memory;
  uint32_t pc = 0x00000000;
  uint32_t ir = 0;
  uint32_t instruction_number = 0;
  uint32_t mode = 0;
  uint32_t trap_cause = 0;

  std::ofstream ofile;

  uint32_t regfile[32] = {};

  CSR csr;

  instruction decode_instruction();
  void execute_instruction(instruction inst);
  void handle_s_interrupt();
  void handle_m_interrupt();
};


#endif
