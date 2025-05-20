#include "cpu.h"
#include <iostream>
#include <iomanip>

CPU::CPU(MEM& memory) : memory(memory) {
  ofile.open("log.txt");
}
CPU::CPU(MEM& memory, uint32_t pc) : memory(memory), pc(pc) {
  ofile.open("log.txt");
}
CPU::~CPU(){
  ofile.close();
}

bool CPU::execute(){
  this->ir = memory.fetch_word(this->pc);
  instruction_number++;

  CPU::instruction inst = decode_instruction();
  regfile[0] = 0;
  execute_instruction(inst);
  regfile[0] = 0;
 
  return true;
}

void CPU::info_registers(){

    ofile << "PC: 0x"
              << std::hex << std::setw(8) << std::setfill('0') << pc << std::endl;

    ofile << "IR: 0x"
              << std::hex << std::setw(8) << std::setfill('0') << ir << std::endl;

    for (int i = 0; i < 32; ++i) {
    ofile << "x" << std::setw(2) << std::setfill('0') << i << ": 0x"
      << std::hex << std::setw(8)<< regfile[i] << std::dec << std::endl;
  }
}

void CPU::info_csr_registers(){
  ofile << "CSR Registers:\n";

  auto print_csr = [](const std::string& name, uint32_t value, std::ofstream& ofile) {
    ofile << std::left << std::setw(12) << std::setfill(' ') << name << std::setfill('0') << std::right << ": 0x"
      << std::hex << std::setw(8) << value << std::dec << std::endl;
  };

  print_csr("mhartid",    csr.mhartid, ofile);
  print_csr("mstatus",    csr.mstatus, ofile);
  print_csr("mepc",       csr.mepc, ofile);
  print_csr("sstatus",    csr.sstatus, ofile);
  print_csr("sip",        csr.sip, ofile);
  print_csr("sie",        csr.sie, ofile);
  print_csr("mie",        csr.mie, ofile);
  print_csr("sepc",       csr.sepc, ofile);
  print_csr("medeleg",    csr.medeleg, ofile);
  print_csr("mideleg",    csr.mideleg, ofile);
  print_csr("stvec",      csr.stvec, ofile);
  print_csr("mtvec",      csr.mtvec, ofile);
  print_csr("satp",       csr.satp, ofile);
  print_csr("scause",     csr.scause, ofile);
  print_csr("mcause",     csr.mcause, ofile);
  print_csr("stval",      csr.stval, ofile);
  print_csr("mcounter",   csr.mcounter, ofile);
  print_csr("time",       csr.time, ofile);
  print_csr("sscratch",   csr.sscratch, ofile);
  print_csr("mscratch",   csr.mscratch, ofile);
}

void CPU::info_pc(){
  ofile << "PC: 0x" << std::hex << std::setw(8) << std::setfill('0') << pc << std::endl;
}
void CPU::info_ir(){
  ofile << "IR: 0x" << std::hex << std::setw(8) << std::setfill('0') << ir << std::endl;
}

void CPU::info_instruction_number(){
  ofile << std::dec << "instruction number: " << instruction_number << std::endl;
}

void CPU::info_unpriv_test(){
  const char* test_names[] = {
    "add", "addi", "and", "andi", "auipc", "beq", "bge", "bgeu", "blt", "bltu",
    "bne", "div", "divu", "j", "jal", "jalr", "lb", "lbu", "lh", "lhu", "lui",
    "lw", "mul", "mulh", "mulhsu", "mulhu", "or", "ori", "rem", "remu", "sb",
    "sh", "simple", "sll", "slli", "slt", "slti", "sra", "srai", "srl", "srli",
    "sub", "sw", "xor", "xori", "csrr", "csrw", "csrs", "csrrw", "csrrs",
    "mret", "sret", "ecall", "ebreak", "fence", "fence.i", "wfi", "sfence.vma",
    "illegal", "nop", "custom0", "custom1", "custom2", "custom3", "custom4"
  };

  for (int i = 0; i <= 44; ++i) {
    uint32_t result = memory.fetch_word(0x60000000 + i);

    std::string passed = "FAILED ";
    if(result & 0x1)
      passed = "PASSED ";

    ofile << "TEST " << std::dec << i  << " " << test_names[i] << " " << passed << ": 0x" << std::hex << std::setw(8) << std::setfill('0') << memory.fetch_word(0x60000000 + i) << std::endl;

  }
}

bool CPU::handle_interrupt(){

  if(trap_cause == 0) return false;
  // -------------------
  // ECALL Exception
  // -------------------
  else if (trap_cause == 8 || trap_cause == 9) {
  // -------------------
    if (csr.medeleg & (1 << 11)) {
      handle_s_interrupt();
    } else {
      handle_m_interrupt();
    }
  }

  // -------------------
  // EBREAK Exception
  // -------------------
  else if (trap_cause == 3) {
    if (csr.medeleg & (1 << 3)) {
      handle_s_interrupt();
    } else {
      handle_m_interrupt();
    }
  }

  // -------------------
  // Timer Interrupt
  // -------------------
  else if (trap_cause == 0x80000001) {
    if (csr.mideleg & (1 << 7)) {
      if ((csr.sie & (1 << 7)) && (csr.sstatus & (1 << 1))) {
        handle_s_interrupt();
      }
    } else {
      if ((csr.mie & (1 << 7)) && (csr.mstatus & (1 << 3))) {
        handle_m_interrupt();
      }
    }
  }

  // -------------------
  // Unsupported Interrupt
  // -------------------
  else {
    printf("Unsupported Interrupt\n");
    exit(-1);
  }

  return true;
}

void CPU::handle_s_interrupt(){
  uint32_t new_sstatus =
    (csr.sstatus & 0xFFFFFE00)
    | ((mode & 0x1) << 8)
    | (csr.sstatus & 0x000000C0)
    | ((csr.sstatus & (1 << 1)) << 6)
    | ((csr.sstatus & 0x0000001C) << 1)
    | (0 << 2)
    | (csr.sstatus & 0x1);

  csr.sstatus = new_sstatus;
  csr.sepc = pc;
  csr.scause = trap_cause;
  pc = csr.stvec;
}

void CPU::handle_m_interrupt(){
  uint32_t new_mstatus =
      (csr.mstatus & 0xFFFFE000)
    | ((mode & 0x3) << 11)
    | (csr.mstatus & 0x00000700)
    | ((csr.mstatus & (1 << 3)) << 8)
    | ((csr.mstatus & 0x00000070) << 1)
    | (0 << 4)
    | (csr.mstatus & 0x7);

  csr.sstatus = new_mstatus;
  csr.mepc = pc;
  csr.mcause = trap_cause;
  pc = csr.mtvec;
}

CPU::instruction CPU::decode_instruction() {
  uint32_t opcode = ir & 0x7F;
  uint32_t funct3 = (ir >> 12) & 0x7;
  uint32_t funct7 = (ir >> 25) & 0x7F;

  switch (opcode) {
    case 0b0110111: return i_lui;
    case 0b0010111: return i_auipc;
    case 0b1101111: return i_jal;
    case 0b1100111: return i_jalr;

    case 0b0000011:
      switch (funct3) {
        case 0b000: return i_lb;
        case 0b001: return i_lh;
        case 0b010: return i_lw;
        case 0b100: return i_lbu;
        case 0b101: return i_lhu;
      }
      break;

    case 0b0010011:
      switch (funct3) {
        case 0b000: return i_addi;
        case 0b010: return i_slti;
        case 0b011: return i_sltiu;
        case 0b100: return i_xori;
        case 0b110: return i_ori;
        case 0b111: return i_andi;
        case 0b001: return i_slli;
        case 0b101:
          if (funct7 == 0b0000000) return i_srli;
          else if (funct7 == 0b0100000) return i_srai;
      }
      break;

    case 0b0110011:
      switch (funct3) {
        case 0b000:
          if (funct7 == 0b0000000) return i_add;
          else if (funct7 == 0b0100000) return i_sub;
          else if (funct7 == 0b0000001) return i_mul;
          break;
        case 0b001:
          if (funct7 == 0b0000000) return i_sll;
          else if (funct7 == 0b0000001) return i_mulh;
          break;
        case 0b010:
          if (funct7 == 0b0000000) return i_slt;
          else if (funct7 == 0b0000001) return i_mulhsu;
          break;
        case 0b011:
          if (funct7 == 0b0000000) return i_sltu;
          else if (funct7 == 0b0000001) return i_mulhu;
          break;
        case 0b100:
          if (funct7 == 0b0000000) return i_xor;
          else if (funct7 == 0b0000001) return i_div;
          break;
        case 0b101:
          if (funct7 == 0b0000000) return i_srl;
          else if (funct7 == 0b0100000) return i_sra;
          else if (funct7 == 0b0000001) return i_divu;
          break;
        case 0b110:
          if (funct7 == 0b0000000) return i_or;
          else if (funct7 == 0b0000001) return i_rem;
          break;
        case 0b111:
          if (funct7 == 0b0000000) return i_and;
          else if (funct7 == 0b0000001) return i_remu;
          break;
      }
      break;

    case 0b1100011: // Branch
      switch (funct3) {
        case 0b000: return i_beq;
        case 0b001: return i_bne;
        case 0b100: return i_blt;
        case 0b101: return i_bge;
        case 0b110: return i_bltu;
        case 0b111: return i_bgeu;
      }
      break;

    case 0b0100011: // Store
      switch (funct3) {
        case 0b000: return i_sb;
        case 0b001: return i_sh;
        case 0b010: return i_sw;
      }
      break;

    case 0b0001111: // Fence
      switch (funct3) {
        case 0b000: return i_fence;
        case 0b001: return i_fence_i;
      }
      break;

    case 0b1110011: { // SYSTEM
    
      if (ir == 0b00000000000000000000000001110011) return i_ecall;
      else if (ir == 0b00000000000100000000000001110011) return i_ebreak;
      else if (ir == 0b00110000001000000000000001110011) return i_mret;
      else if (ir == 0b00010000001000000000000001110011) return i_sret;
      else if (ir == 0b00000000001000000000000001110011) return i_uret;
    
      switch (funct3) {
        case 0b001: return i_csrrw;
        case 0b010: return i_csrrs;
        case 0b011: return i_csrrc;
        case 0b101: return i_csrrwi;
        case 0b110: return i_csrrsi;
        case 0b111: return i_csrrci;
      }
      break;
    }
    case 0b0101111: //Atomic instructions
      if (funct3 == 0b010) {
        uint32_t funct5 = (funct7 >> 2) & 0x1F;
        switch (funct5) {
//          case 0b00010: return i_lr_w;
//          case 0b00011: return i_sc_w;
          case 0b00001: return i_amoswap_w;
          case 0b00000: return i_amoadd_w;
          case 0b00100: return i_amoxor_w;
          case 0b01100: return i_amoand_w;
          case 0b01000: return i_amoor_w;
          case 0b10000: return i_amomin_w;
          case 0b10100: return i_amomax_w;
          case 0b11000: return i_amominu_w;
          case 0b11100: return i_amomaxu_w;
        }
      }
      break;
  }
  printf("CPU: INVALID INSTRUCTION\n");
  exit(-1);
  return i_invalid_instruction;
}
