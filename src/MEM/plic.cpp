#include "plic.h"

PLIC::PLIC(CPU& cpu) : cpu(cpu){
  //this->size = 2101252;
  this->size = 2102000; //Simulates all PLIC addresses without acctually alocating them
}

void PLIC::send_interrupt(uint32_t irq){
  claim = irq;
  cpu.external_interrupt();
}

void PLIC::store_byte(uint32_t address, uint8_t value){

}

void PLIC::store_half(uint32_t address, uint16_t value){

}

void PLIC::store_word(uint32_t address, uint32_t value){

}

uint8_t PLIC::fetch_byte(uint32_t address) {
  if (address == 2101252) {
    return 10;
  }
  return 0;
}

uint16_t PLIC::fetch_half(uint32_t address) {
  if (address == 2101252) {
    return 10;
  }
  return 0;
}

uint32_t PLIC::fetch_word(uint32_t address) {
  if (address == 2101252) {
    return 10;
  }
  return 0;
}
