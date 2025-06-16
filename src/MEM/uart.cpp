#include "uart.h"
#include "iostream"
#define RHR 0 // receive holding register (for input bytes)
#define THR 0 // transmit holding register (for output bytes)
#define IER 1 // interrupt enable register
#define FCR 2 // FIFO control register
#define ISR 2 // interrupt status register
#define LCR 3 // line control register
#define LSR 5 // line status register

UART::UART(){
  this->size = 6;
  this->memory = new uint8_t[6];
}

void UART::store_byte(uint32_t address, uint8_t value){
  //std::cout << "UART store_byte ADDR: " << address << ", VALUE: " << value << std::endl;
  if (address == RHR) {
    std::cout << value;
  }
   memory[address] = value;
}

void UART::send_char(char c){
  memory[THR] = (uint8_t)c;
  memory[LSR] = 0x01;
}

uint8_t UART::fetch_byte(uint32_t address){
  if (address == LSR) {
    return memory[LSR] = memory[LSR] | (1 << 5);
  }
  else if (address == 0) {
    memory[5] = 0;
    return memory[address];
  }
  else {
    return memory[address];
  }
}
