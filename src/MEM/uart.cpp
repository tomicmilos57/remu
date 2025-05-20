#include "uart.h"
#include "iostream"

UART::UART(){
  this->size = 6;
  this->memory = new uint8_t[6];
}

void UART::store_byte(uint32_t address, uint8_t value){
  //std::cout << "UART store_byte ADDR: " << address << ", VALUE: " << value << std::endl;
  if (address == 0) {
    std::cout << value;
  }
  else {
      memory[address] = value;
  }
}

uint8_t UART::fetch_byte(uint32_t address){
  if (address == 5) {
    return 0xFF;
  }
  else {
    return memory[address];
  }
}
