#include "mem.h"

MEM::MEM(RAM& ram, GPU& gpu) : ram(ram), gpu(gpu) {}
MEM::MEM(RAM& ram) : ram(ram), gpu(GPU()) {}

Device* MEM::get_device(uint32_t address) {

  if (address >= 0x80000000 && address < 0x90000000)
    return &ram;
  if (address >= 0x20000000 && address < 0x30000000)
    return &gpu;

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
