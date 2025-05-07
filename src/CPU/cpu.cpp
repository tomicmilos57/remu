#include "cpu.h"
#include <iostream>
#include <iomanip>

CPU::CPU(MEM& memory) : memory(memory) {}
CPU::CPU(MEM& memory, uint32_t pc) : memory(memory), pc(pc) {}


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

    std::cout << "PC: 0x"
              << std::hex << std::setw(8) << std::setfill('0') << pc << std::endl;

    std::cout << "IR: 0x"
              << std::hex << std::setw(8) << std::setfill('0') << ir << std::endl;

    for (int i = 0; i < 32; ++i) {
    std::cout << "x" << std::setw(2) << std::setfill('0') << i << ": 0x"
      << std::hex << std::setw(8) << std::setfill('0') << regfile[i]
      << std::dec << std::endl;
  }
}

void CPU::info_pc(){
  std::cout << "PC: 0x" << std::hex << std::setw(8) << std::setfill('0') << pc << std::endl;
}
void CPU::info_ir(){
  std::cout << "IR: 0x" << std::hex << std::setw(8) << std::setfill('0') << ir << std::endl;
}

void CPU::info_instruction_number(){
  std::cout << std::dec << "instruction number: " << instruction_number << std::endl;
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
        regfile[rd] = pc + 4;
				pc = ((regfile[rs1] + imm_se) & ~1);
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
        regfile[rd] = regfile[rs1] << rs2 & 0x1F;
        pc += 4;
        break;
      }
    case CPU::i_srli:
      {
        regfile[rd] = regfile[rs1] >> rs2 & 0x1F;
        pc += 4;
        break;
      }
    case CPU::i_srai:
      {
        regfile[rd] = static_cast<int32_t>(regfile[rs1]) >> rs2 & 0x1F;
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
        regfile[rd] = regfile[rs1] << regfile[rs2] & 0x1F;
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
        regfile[rd] = regfile[rs1] >> regfile[rs2] & 0x1F;
        pc += 4;
        break;
      }
    case CPU::i_sra:
      {
        regfile[rd] = static_cast<int32_t>(regfile[rs1]) >> regfile[rs2] & 0x1F;
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
  
    case CPU::i_fence:
      {

      }
    case CPU::i_fence_i:
      {

      }
    case CPU::i_ecall:
      {

      }
    case CPU::i_ebreak:
      {

      }
  
    case CPU::i_invalid_instruction:
      {

      }
  }
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
        case 0b000: return (funct7 == 0b0000000) ? i_add : i_sub;
        case 0b001: return i_sll;
        case 0b010: return i_slt;
        case 0b011: return i_sltu;
        case 0b100: return i_xor;
        case 0b101: return (funct7 == 0b0000000) ? i_srl : i_sra;
        case 0b110: return i_or;
        case 0b111: return i_and;
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
        case 0b001: return i_fence;
      }
      break;

    case 0b1110011: // System
      if ((ir >> 20) == 0) return i_ecall;
      else if ((ir >> 20) == 1) return i_ebreak;
      break;
  }

  return i_invalid_instruction;
}
