#include "device.h"
#include <iostream>

RAM::RAM(int size, std::string filename){

  //Allocate RAM memory
  this->memory = new uint8_t[size];
  this->size = size;
  this->base = 0x80000000;

  //Open bin file
  std::ifstream file(filename, std::ios::binary);
  if (!file) throw std::runtime_error("Cannot open file.");

  //Get size of bin file
  file.seekg(0, std::ios::end);
  std::streamsize filesize = file.tellg();
  file.seekg(0, std::ios::beg);

  //Read bin file inside memory
  if (!file.read(reinterpret_cast<char*>(memory), filesize)) throw std::runtime_error("Error reading file.");

}

GPU::GPU(){
  int size = 16 * 1024;

  //Allocate GPU memory
  this->memory = new uint8_t[size];
  this->size = size;
  this->base = 0x20000000;
}

GPU::GPU(std::string filename){
  int size = 16 * 1024;

  //Allocate GPU memory
  this->memory = new uint8_t[size];
  this->size = size;
  this->base = 0x20000000;

  //Open bin file
  std::ifstream file(filename, std::ios::binary);
  if (!file) throw std::runtime_error("Cannot open file.");

  //Get size of bin file
  file.seekg(0, std::ios::end);
  std::streamsize filesize = file.tellg();
  file.seekg(0, std::ios::beg);

  //Read bin file inside memory
  if (!file.read(reinterpret_cast<char*>(memory), filesize)) throw std::runtime_error("Error reading file.");

}

Device::~Device(){
  delete memory;
}

uint8_t Device::fetch_byte(uint32_t address){
  return memory[address - base];
}

uint16_t Device::fetch_half(uint32_t address){
  return (memory[address - base] | (memory[address - base + 1] << 8));
}

uint32_t Device::fetch_word(uint32_t address){
  return (memory[address - base] |
      (memory[address - base + 1] << 8) |
      (memory[address - base + 2] << 16) |
      (memory[address - base + 3] << 24));
}


void Device::store_byte(uint32_t address, uint8_t byte){
  memory[address - base] = byte;
}

void Device::store_half(uint32_t address, uint16_t half){
  memory[address - base] = half & 0xFF;
  memory[address - base + 1] = (half >> 8) & 0xFF;
}

void Device::store_word(uint32_t address, uint32_t word){
  memory[address - base] = word & 0xFF;
  memory[address - base + 1] = (word >> 8) & 0xFF;
  memory[address - base + 2] = (word >> 16) & 0xFF;
  memory[address - base + 3] = (word >> 24) & 0xFF;
}
