#include "MEM/mem.h"
#include "CPU/cpu.h"
#include <iostream>

int main(){

  RAM ram(8*1024*1024, "misc/01_fib.bin");
  GPU gpu;
  MEM mem_map(ram, gpu);
  CPU cpu(mem_map);

  int count = 0;
  while (count < 150) {
    cpu.execute();
    std::cout << "Instruction number: ";
    //cpu.info_pc();
    cpu.info_instruction_number();
    //cpu.info_ir();
    cpu.info_registers();
    count++;
  }

  return 0;
}
