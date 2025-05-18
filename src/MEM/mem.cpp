#include "mem.h"
#include <iostream>
#include <iomanip>


void MEM::register_device(Device& dev, uint32_t base){
  registered_devices.push_back(new Module(&dev, base));
}

MEM::~MEM(){
  for(Module* module : registered_devices) {
    delete module;
  }
}

MEM::Module* MEM::get_device(uint32_t address) {

  for(Module* module : registered_devices) {
    if (address >= module->base && address < module->base + module->dev->get_size())
      return module;
  }

  std::cout << "MEMORY ACCESS FAULT" << std::endl;
  std::cout << "Address: 0x" << std::hex << std::setw(8) << std::setfill('0') << address <<  std::endl;
  exit(404);
  return nullptr;
}

uint8_t MEM::fetch_byte(uint32_t address) {
  if (auto module = get_device(address))
    return module->dev->fetch_byte(address - module->base);
  return 0;
}

uint16_t MEM::fetch_half(uint32_t address) {
  if (auto module = get_device(address))
    return module->dev->fetch_half(address - module->base);
  return 0;
}

uint32_t MEM::fetch_word(uint32_t address) {
  if (auto module = get_device(address))
    return module->dev->fetch_word(address - module->base);
  return 0;
}

void MEM::store_byte(uint32_t address, uint8_t value) {
  if (auto module = get_device(address))
    module->dev->store_byte(address - module->base, value);
}

void MEM::store_half(uint32_t address, uint16_t value) {
  if (auto module = get_device(address))
    module->dev->store_half(address - module->base, value);
}

void MEM::store_word(uint32_t address, uint32_t value) {
  if (auto module = get_device(address))
    module->dev->store_word(address - module->base, value);
}
