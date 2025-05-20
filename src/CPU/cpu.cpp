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

void CPU::execute_instruction(instruction inst){

  uint32_t rs1 = (ir >> 15) & 0x1F;
  uint32_t rs2 = (ir >> 20) & 0x1F;
  uint32_t rd = (ir >> 7) & 0x1F;

	uint32_t imm = ir >> 20;
	int32_t imm_se = imm | (( imm & 0x800 )?0xfffff000:0);
	uint32_t imm_store = ( ( ir >> 7 ) & 0x1f ) | ( ( ir & 0xfe000000 ) >> 20 );
	if( imm_store & 0x800 ) imm_store |= 0xfffff000;

	uint32_t imm_branch = ((ir & 0xf00)>>7) | ((ir & 0x7e000000)>>20) | ((ir & 0x80) << 4) | ((ir >> 31)<<12);
	if( imm_branch & 0x1000 ) imm_branch |= 0xffffe000;

	uint32_t csr_sel = ir >> 20;

  switch (inst) {
    case CPU::i_lui:
      {
        regfile[rd] = (ir & 0xfffff000);
        pc += 4;
        break;
      }
    case CPU::i_auipc:
      {
        regfile[rd] = pc + (ir & 0xfffff000);
        pc += 4;
        break;
      }
  
    case CPU::i_jal:
      {
        uint32_t jump = ((ir & 0x80000000)>>11) | ((ir & 0x7fe00000)>>20) | ((ir & 0x00100000)>>9) | ((ir&0x000ff000));
				if( jump & 0x00100000 ) jump |= 0xffe00000; // Sign extension.

        regfile[rd] = pc + 4;
        pc += jump; // -4 MAYBE
        break;
      }
    case CPU::i_jalr:
      {
        uint32_t old_pc = pc;
				pc = ((regfile[rs1] + imm_se) & ~1U);
        regfile[rd] = old_pc + 4;
        break;
      }
  
    case CPU::i_lb:
      {
        regfile[rd] = static_cast<int32_t>(static_cast<int8_t>(memory.fetch_byte(regfile[rs1] + imm_se)));
        pc += 4;
        break;
      }
    case CPU::i_lh:
      {
        regfile[rd] = static_cast<int32_t>(static_cast<int16_t>(memory.fetch_half(regfile[rs1] + imm_se)));
        pc += 4;
        break;
      }
    case CPU::i_lw:
      {
        regfile[rd] = static_cast<int32_t>(memory.fetch_word(regfile[rs1] + imm_se));
        pc += 4;
        break;
      }
    case CPU::i_lbu:
      {
        regfile[rd] = static_cast<int32_t>(memory.fetch_byte(regfile[rs1] + imm_se));
        pc += 4;
        break;
      }
    case CPU::i_lhu:
      {
        regfile[rd] = static_cast<int32_t>(memory.fetch_half(regfile[rs1] + imm_se));
        pc += 4;
        break;
      }
  
    case CPU::i_addi:
      {
        regfile[rd] = regfile[rs1] + imm_se;
        pc += 4;
        break;
      }
    case CPU::i_slti:
      {
        regfile[rd] = static_cast<int32_t>(regfile[rs1]) < static_cast<int32_t>(imm_se);
        pc += 4;
        break;
      }
    case CPU::i_sltiu:
      {
        regfile[rd] = static_cast<uint32_t>(regfile[rs1]) < static_cast<uint32_t>(imm_se);
        pc += 4;
        break;
      }
    case CPU::i_xori:
      {
        regfile[rd] = regfile[rs1] ^ imm_se;
        pc += 4;
        break;
      }
    case CPU::i_ori:
      {
        regfile[rd] = regfile[rs1] | imm_se;
        pc += 4;
        break;
      }
    case CPU::i_andi:
      {
        regfile[rd] = regfile[rs1] & imm_se;
        pc += 4;
        break;
      }
  
    case CPU::i_slli:
      {
        regfile[rd] = regfile[rs1] << (rs2 & 0x1F);
        pc += 4;
        break;
      }
    case CPU::i_srli:
      {
        regfile[rd] = regfile[rs1] >> (rs2 & 0x1F);
        pc += 4;
        break;
      }
    case CPU::i_srai:
      {
        regfile[rd] = static_cast<int32_t>(regfile[rs1]) >> (rs2 & 0x1F);
        pc += 4;
        break;
      }
  
    case CPU::i_add:
      {
        regfile[rd] = regfile[rs1] + regfile[rs2];
        pc += 4;
        break;
      }
    case CPU::i_sub:
      {
        regfile[rd] = regfile[rs1] - regfile[rs2];
        pc += 4;
        break;
      }
    case CPU::i_sll:
      {
        regfile[rd] = regfile[rs1] << (regfile[rs2] & 0x1F);
        pc += 4;
        break;
      }
    case CPU::i_slt:
      {
        regfile[rd] = static_cast<int32_t>(regfile[rs1]) < static_cast<int32_t>(regfile[rs2]);
        pc += 4;
        break;
      }
    case CPU::i_sltu:
      {
        regfile[rd] = static_cast<uint32_t>(regfile[rs1]) < static_cast<uint32_t>(regfile[rs2]);
        pc += 4;
        break;
      }
    case CPU::i_xor:
      {
        regfile[rd] = regfile[rs1] ^ regfile[rs2];
        pc += 4;
        break;
      }
    case CPU::i_srl:
      {
        regfile[rd] = regfile[rs1] >> (regfile[rs2] & 0x1F);
        pc += 4;
        break;
      }
    case CPU::i_sra:
      {
        regfile[rd] = static_cast<int32_t>(regfile[rs1]) >> (regfile[rs2] & 0x1F);
        pc += 4;
        break;
      }
    case CPU::i_or:
      {
        regfile[rd] = regfile[rs1] | regfile[rs2];
        pc += 4;
        break;
      }
    case CPU::i_and:
      {
        regfile[rd] = regfile[rs1] & regfile[rs2];
        pc += 4;
        break;
      }
  
    case CPU::i_beq:
      {
        if(regfile[rs1] == regfile[rs2])
          pc += imm_branch;
        else 
          pc += 4;
        break;
      }
    case CPU::i_bne:
      {
        if(regfile[rs1] != regfile[rs2])
          pc += imm_branch;
        else 
          pc += 4;
        break;
      }
    case CPU::i_blt:
      {
        if(static_cast<int32_t>(regfile[rs1]) < static_cast<int32_t>(regfile[rs2]))
          pc += imm_branch;
        else 
          pc += 4;
        break;
      }
    case CPU::i_bge:
      {
        if(static_cast<int32_t>(regfile[rs1]) >= static_cast<int32_t>(regfile[rs2]))
          pc += imm_branch;
        else 
          pc += 4;
        break;
      }
    case CPU::i_bltu:
      {
        if(regfile[rs1] < regfile[rs2])
          pc += imm_branch;
        else 
          pc += 4;
        break;
      }
    case CPU::i_bgeu:
      {
        if(regfile[rs1] >= regfile[rs2])
          pc += imm_branch;
        else 
          pc += 4;
        break;
      }
  
    case CPU::i_sb:
      {
        memory.store_byte(regfile[rs1] + imm_store, regfile[rs2] & 0xFF);
        pc += 4;
        break;
      }
    case CPU::i_sh:
      {
        memory.store_half(regfile[rs1] + imm_store, regfile[rs2] & 0xFFFF);
        pc += 4;
        break;
      }
    case CPU::i_sw:
      {
        memory.store_word(regfile[rs1] + imm_store, regfile[rs2]);
        pc += 4;
        break;
      }
  
    case CPU::i_mul:
      {
        regfile[rd] = regfile[rs1] * regfile[rs2];
        pc += 4;
        break;
      }
    case CPU::i_mulh:
      {
        regfile[rd] = ((int64_t)((int32_t)regfile[rs1]) * (int64_t)((int32_t)regfile[rs2])) >> 32;
        pc += 4;
        break;
      }
    case CPU::i_mulhsu:
      {
        regfile[rd] = ((int64_t)((int32_t)regfile[rs1]) * (uint64_t)(regfile[rs2])) >> 32;
        pc += 4;
        break;
      }
    case CPU::i_mulhu:
      {
        regfile[rd] = ((uint64_t)regfile[rs1] * (uint64_t)regfile[rs2]) >> 32;
        pc += 4;
        break;
      }
    case CPU::i_div:
      {
        if(regfile[rs2] == 0)
          regfile[rd] = 0xFFFFFFFF;
        else if (static_cast<int32_t>(regfile[rs1]) == INT32_MIN && static_cast<int32_t>(regfile[rs2]) == -1)
          regfile[rd] = INT32_MIN;
        else
          regfile[rd] = static_cast<int32_t>(regfile[rs1]) / static_cast<int32_t>(regfile[rs2]);
        pc += 4;
        break;
      }
    case CPU::i_divu:
      {
        if(regfile[rs2] != 0)
          regfile[rd] = regfile[rs1] / regfile[rs2];
        else
          regfile[rd] = 0xFFFFFFFF;
        pc += 4;
        break;
      }
    case CPU::i_rem:
      {
        if( regfile[rs2] == 0 )
          regfile[rd] = regfile[rs1]; 
        else
          regfile[rd] = ((int32_t)regfile[rs1] == INT32_MIN && (int32_t)regfile[rs2] == -1) ? 0 : ((uint32_t)((int32_t)regfile[rs1] % (int32_t)regfile[rs2]));
        pc += 4;
        break;
      }
    case CPU::i_remu:
      {
        if(regfile[rs2] != 0)
          regfile[rd] = regfile[rs1] % regfile[rs2];
        else
          regfile[rd] = regfile[rs1];
        pc += 4;
        break;
      }

    case CPU::i_fence:
      {
        pc += 4;
        break;
      }
    case CPU::i_fence_i:
      {
        pc += 4;
        break;
      }

    case CPU::i_ecall:
      {
        if (mode == 0) trap_cause = 8;
        else if (mode == 1) trap_cause = 9;
        else{
          printf("ecall from M mode");
          exit(-1);
        }

        pc += 4;
        break;
      }
    case CPU::i_ebreak:
      {
        trap_cause = 3;

        pc += 4;
        break;
      }

    case CPU::i_mret:
      {
        mode = (csr.mstatus >> 11) & 0x3;
        uint32_t new_ret_mstatus = (csr.mstatus & 0xFFFFFFF0) | ((csr.mstatus >> 7) & 0x1) << 3 | (csr.mstatus & 0x7);
        csr.mstatus = new_ret_mstatus;
        pc = csr.mepc;
        break;
      }
    case CPU::i_sret:
      {
        mode = ((csr.sstatus >> 8) & 0x1);
        uint32_t new_ret_sstatus = (csr.sstatus & 0xFFFFFFFC) | ((csr.sstatus >> 5) & 0x1) << 1 | (csr.sstatus & 0x1);     
        csr.sstatus = new_ret_sstatus;
        pc = csr.sepc;
        break;
      }
    case CPU::i_uret:
      {
        ofile << "URET CALLED: Exiting" << std::endl;
        exit(-1);
        break;
      }

    case CPU::i_csrrw:
      {
        uint32_t temp = csr.read(csr_sel);
        csr.set(csr_sel, regfile[rs1]);
        regfile[rd] = temp;

        pc += 4;
        break;
      }
    case CPU::i_csrrs:
      {
        uint32_t temp = csr.read(csr_sel);
        csr.set(csr_sel, temp | regfile[rs1]);
        regfile[rd] = temp;

        pc += 4;
        break;
      }
    case CPU::i_csrrc:
      {
        uint32_t temp = csr.read(csr_sel);
        csr.set(csr_sel, temp & ~regfile[rs1]);
        regfile[rd] = temp;

        pc += 4;
        break;
      }
    case CPU::i_csrrwi:
      {
        uint32_t temp = csr.read(csr_sel);
        csr.set(csr_sel, rs1);
        regfile[rd] = temp;

        pc += 4;
        break;
      }
    case CPU::i_csrrsi:
      {
        uint32_t temp = csr.read(csr_sel);
        csr.set(csr_sel, temp | rs1);
        regfile[rd] = temp;

        pc += 4;
        break;
      }
    case CPU::i_csrrci:
      {
        uint32_t temp = csr.read(csr_sel);
        csr.set(csr_sel, temp & ~rs1);
        regfile[rd] = temp;

        pc += 4;
        break;
      }
  
    case CPU::i_invalid_instruction:
      {

      }
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
  }

  return i_invalid_instruction;
}
