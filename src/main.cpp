#include "MEM/mem.h"
#include "MEM/uart.h"
#include "MEM/plic.h"
#include "MEM/sd_card.h"
#include "CPU/cpu.h"
#include "HW/console.h"
#include <iostream>

int main(){

  RAM ram_memory(8*1024*1024, "misc/kernel.bin");
  GPU gpu_memory;
  MEM mem_map;
  mem_map.register_device(ram_memory, 0x80000000);
  mem_map.register_device(gpu_memory, 0x20000000);

  RAM test_memory(1024);
  mem_map.register_device(test_memory, 0x60000000);

  UART uart;
  mem_map.register_device(uart, 0x10000000);

  SD_CARD sd_card;
  mem_map.register_device(sd_card, 0x10001000);

  CPU cpu(mem_map, 0x80000000);

  PLIC plic(cpu);
  mem_map.register_device(plic, 0x0c000000);

  Console console(plic, uart);

  int count = 0;
  while (true) {
    if(count >= 10000) {
      console.simulate_input();
      sd_card.simulate_sd_card();
      count = 0;
    }
    cpu.execute();
    cpu.handle_interrupt();
    //cpu.info_pc();
    //cpu.info_ir();
    //cpu.info_instruction_number();
    //cpu.info_registers();
    //cpu.info_csr_registers();
    count++;
  }
  //cpu.info_unpriv_test();

  return 0;
}
