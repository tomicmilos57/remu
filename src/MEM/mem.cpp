#include "mem.h"
#include <iostream>
#include <iomanip>

MEM::MEM(RAM& ram, GPU& gpu) : ram(ram), gpu(gpu) {}

Device* MEM::get_device(uint32_t address) {

  if (address >= 0x00000000 && address < 0x90000000)
    return &ram;
  if (address >= 0x20000000 && address < 0x30000000)
    return &gpu;

  std::cout << "MEMORY ACCESS FAULT" << std::endl;
  std::cout << "Address: 0x" << std::hex << std::setw(8) << std::setfill('0') << address <<  std::endl;
  exit(404);
  return nullptr;
}

uint8_t MEM::fetch_byte(uint32_t address) {
  if (auto dev = get_device(address))
    return dev->fetch_byte(address);
  return 0;
}

uint16_t MEM::fetch_half(uint32_t address) {
  if (auto dev = get_device(address))
    return dev->fetch_half(address);
  return 0;
}

uint32_t MEM::fetch_word(uint32_t address) {
  if (auto dev = get_device(address))
    return dev->fetch_word(address);
  return 0;
}

void MEM::store_byte(uint32_t address, uint8_t value) {
  if (auto dev = get_device(address))
    dev->store_byte(address, value);
}

void MEM::store_half(uint32_t address, uint16_t value) {
  if (auto dev = get_device(address))
    dev->store_half(address, value);
}

void MEM::store_word(uint32_t address, uint32_t value) {
  if (auto dev = get_device(address))
    dev->store_word(address, value);
}
