#include "MEM/mem.h"
#include "CPU/cpu.h"
#include <iostream>

int main(){

  RAM ram_memory(8*1024*1024, "misc/test.bin", 0x00000000);
  GPU gpu_memory(0x20000000);
  MEM mem_map;
  mem_map.register_device(ram_memory);
  mem_map.register_device(gpu_memory);

  RAM test_memory(1024, 0x60000000);
  mem_map.register_device(test_memory);

  CPU cpu(mem_map, 0x00000000);

  int count = 0;
  while (count < 16200) {
    cpu.execute();
    //std::cout << "Instruction number: ";
    //cpu.info_pc();
    //cpu.info_instruction_number();
    //cpu.info_ir();
    //cpu.info_registers();
    count++;
  }
  cpu.info_unpriv_test();

  return 0;
}
