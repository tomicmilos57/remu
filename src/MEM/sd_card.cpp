#include "sd_card.h"
#include <iostream>

#define SD_CARD_BUFFER 0x00000000UL
#define SD_CARD_ADDRESS 0x00000200UL
#define SD_CARD_OPERATION 0x00000204UL
#define BSIZE 512

SD_CARD::SD_CARD(){
  this->size = BSIZE + 4 + 1; //(1024B)Buffer + (4B)Address + (1B)OP
  this->memory = new uint8_t[size];

  //Open bin file
  std::ifstream file("misc/fs.img", std::ios::binary);
  if (!file) throw std::runtime_error("Cannot open file.");

  //Get size of bin file
  file.seekg(0, std::ios::end);
  size_t filesize = file.tellg();
  file.seekg(0, std::ios::beg);

  disk_memory = new uint8_t[filesize];

  //Read bin file inside memory
  if (!file.read(reinterpret_cast<char*>(disk_memory), filesize)) throw std::runtime_error("Error reading file.");
}

SD_CARD::~SD_CARD(){
  delete memory;
  delete disk_memory;
}

void SD_CARD::simulate_sd_card(){
  uint8_t request = memory[SD_CARD_OPERATION];
  if(request == 0x01){ //READ
    uint32_t blockno = fetch_local_word(SD_CARD_ADDRESS);
    uint32_t address = blockno;

    for (int i = 0; i < BSIZE; i++) {
      memory[i] = disk_memory[address+i];
    }

    memory[SD_CARD_OPERATION] = 0x00;
  }
  else if (request == 0x02) { //WRITE
    uint32_t blockno = fetch_local_word(SD_CARD_ADDRESS);
    uint32_t address = blockno;

    for (int i = 0; i < BSIZE; i++) {
      disk_memory[address+i] = memory[i];
    }

    memory[SD_CARD_OPERATION] = 0x00;
  }
}

uint32_t SD_CARD::fetch_local_word(uint32_t address){
  return (memory[address] |
      (memory[address + 1] << 8) |
      (memory[address + 2] << 16) |
      (memory[address + 3] << 24));
}
