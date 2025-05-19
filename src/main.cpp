#include "MEM/mem.h"
#include "CPU/cpu.h"
#include <iostream>

int main(){

  RAM ram_memory(8*1024*1024, "misc/kernel.bin");
  GPU gpu_memory;
  MEM mem_map;
  mem_map.register_device(ram_memory, 0x80000000);
  mem_map.register_device(gpu_memory, 0x20000000);

  RAM test_memory(1024);
  mem_map.register_device(test_memory, 0x60000000);

  CPU cpu(mem_map, 0x80000000);

  int count = 0;
  while (count < 100) {
    cpu.execute();
    cpu.handle_interrupt();
    cpu.info_instruction_number();
    //cpu.info_pc();
    //cpu.info_ir();
    cpu.info_registers();
    cpu.info_csr_registers();
    count++;
  }
  //cpu.info_unpriv_test();

  return 0;
}
