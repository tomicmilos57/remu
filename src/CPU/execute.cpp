#include "cpu.h"
#include <iostream>
#include <iomanip>

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
    case CPU::i_sfence_vma:
      {
        pc += 4;
        break;
      }
    case CPU::i_wfi:
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
        ofile << "SRET CALLED: Exiting" << std::endl;
        info_registers();
        info_csr_registers();
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
  
    case CPU::i_amoswap_w:
      {
        uint32_t address = regfile[rs1];
        uint32_t value2 = regfile[rs2];
        uint32_t value_from_mem = memory.fetch_word(address);
        regfile[rd] = value_from_mem;
        memory.store_word(address, value2);

        pc += 4;
        break;
      }
    case CPU::i_amoadd_w:
      {
        uint32_t address = regfile[rs1];
        uint32_t value2 = regfile[rs2];
        uint32_t value_from_mem = memory.fetch_word(address);
        regfile[rd] = value_from_mem;
        memory.store_word(address, value_from_mem + value2);

        pc += 4;
        break;
      }
    case CPU::i_amoxor_w:
      {
        uint32_t address = regfile[rs1];
        uint32_t value2 = regfile[rs2];
        uint32_t value_from_mem = memory.fetch_word(address);
        regfile[rd] = value_from_mem;
        memory.store_word(address, value_from_mem ^ value2);

        pc += 4;
        break;
      }
    case CPU::i_amoand_w:
      {
        uint32_t address = regfile[rs1];
        uint32_t value2 = regfile[rs2];
        uint32_t value_from_mem = memory.fetch_word(address);
        regfile[rd] = value_from_mem;
        memory.store_word(address, value_from_mem & value2);

        pc += 4;
        break;
      }
    case CPU::i_amoor_w:
      {
        uint32_t address = regfile[rs1];
        uint32_t value2 = regfile[rs2];
        uint32_t value_from_mem = memory.fetch_word(address);
        regfile[rd] = value_from_mem;
        memory.store_word(address, value_from_mem | value2);

        pc += 4;
        break;
      }
    case CPU::i_amomin_w:
      {
        uint32_t address = regfile[rs1];
        uint32_t value2 = regfile[rs2];
        uint32_t value_from_mem = memory.fetch_word(address);
        regfile[rd] = value_from_mem;
        memory.store_word(address, (int32_t)value_from_mem < (int32_t)value2 ? value_from_mem : value2);

        pc += 4;
        break;
      } 
    case CPU::i_amomax_w:
      {
        uint32_t address = regfile[rs1];
        uint32_t value2 = regfile[rs2];
        uint32_t value_from_mem = memory.fetch_word(address);
        regfile[rd] = value_from_mem;
        memory.store_word(address, (int32_t)value_from_mem > (int32_t)value2 ? value_from_mem : value2);

        pc += 4;
        break;
      }
    case CPU::i_amominu_w:
      {
        uint32_t address = regfile[rs1];
        uint32_t value2 = regfile[rs2];
        uint32_t value_from_mem = memory.fetch_word(address);
        regfile[rd] = value_from_mem;
        memory.store_word(address, value_from_mem < value2 ? value_from_mem : value2);

        pc += 4;
        break;
      }
    case CPU::i_amomaxu_w:
      {
        uint32_t address = regfile[rs1];
        uint32_t value2 = regfile[rs2];
        uint32_t value_from_mem = memory.fetch_word(address);
        regfile[rd] = value_from_mem;
        memory.store_word(address, value_from_mem > value2 ? value_from_mem : value2);

        pc += 4;
        break;
      }

    case CPU::i_invalid_instruction:
      {

      }
  }
}
