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

void MEM::set_satp(uint32_t satp){
  this->satp = satp;
}

uint32_t MEM::resolve_address(uint32_t vaddr){
  //std::cout << "vaddr: " <<  std::hex << vaddr << std::endl;
  uint32_t vpn1 = (vaddr >> 22) & 0x3FF;
  uint32_t vpn0 = (vaddr >> 12) & 0x3FF;
  uint32_t offset = vaddr & 0xFFF;

  uint32_t root_ppn = satp & 0x003FFFFF;
  uint32_t l1_addr = (root_ppn << 12) + vpn1 * 4;
  //std::cout << "lvl1 addr: " <<  std::hex << l1_addr << std::endl;

  auto m1 = get_device(l1_addr);
  uint32_t l1_pte = m1->dev->fetch_word(l1_addr - m1->base);
  if ((l1_pte & 0x1) == 0) {
    printf("Invalid level-1 PTE\n");
    exit(-1);
  }

  uint32_t l1_ppn = (l1_pte >> 10) & 0xFFFFF;
  uint32_t l2_addr = (l1_ppn << 12) + vpn0 * 4;
  //std::cout << "lvl2 addr: " << std::hex << l2_addr << std::endl;

  auto m2 = get_device(l2_addr);
  uint32_t l2_pte = m2->dev->fetch_word(l2_addr - m2->base);
  if ((l2_pte & 0x1) == 0 || ((l2_pte >> 1) & 0x7) == 0) {
    printf("Invalid or non-leaf level-2 PTE\n");
    exit(-1);
  }

  uint32_t page_ppn = (l2_pte >> 10) & 0xFFFFF;
  //std::cout << "Returning addr: " <<  std::hex << (page_ppn << 12) + offset << std::endl;
  return (page_ppn << 12) + offset;
  /*uint32_t vpn1 = (vaddr >> 22) & 0x3ff;
  uint32_t vpn0 = (vaddr >> 12) & 0x3ff;
  uint32_t offset = vaddr & 0xFFF;

  uint32_t root_ppn = satp & 0x003FFFFF;
  uint32_t addr1 = (root_ppn << 12) + vpn1 * 4;

  auto module1 = get_device(addr1);
  uint32_t level1 = module1->dev->fetch_word(addr1 - module1->base);

  uint32_t l1_ppn = (level1 >> 10) & 0xFFFFF;
  uint32_t addr2 = (l1_ppn << 12) + vpn0 * 4;

  auto module2 = get_device(addr2);
  uint32_t level2 = module2->dev->fetch_word(addr2 - module2->base);

  return (level2 & 0xFFFFFC00) + offset;*/
}

uint8_t MEM::fetch_byte(uint32_t address) {
  if(satp)
    address = resolve_address(address);
  if (auto module = get_device(address))
    return module->dev->fetch_byte(address - module->base);
  return 0;
}

uint16_t MEM::fetch_half(uint32_t address) {
  if(satp)
    address = resolve_address(address);
  if (auto module = get_device(address))
    return module->dev->fetch_half(address - module->base);
  return 0;
}

uint32_t MEM::fetch_word(uint32_t address) {
  if(satp)
    address = resolve_address(address);
  if (auto module = get_device(address))
    return module->dev->fetch_word(address - module->base);
  return 0;
}

void MEM::store_byte(uint32_t address, uint8_t value) {
  if(satp)
    address = resolve_address(address);
  if (auto module = get_device(address))
    module->dev->store_byte(address - module->base, value);
}

void MEM::store_half(uint32_t address, uint16_t value) {
  if(satp)
    address = resolve_address(address);
  if (auto module = get_device(address))
    module->dev->store_half(address - module->base, value);
}

void MEM::store_word(uint32_t address, uint32_t value) {
  if(satp)
    address = resolve_address(address);
  if (auto module = get_device(address))
    module->dev->store_word(address - module->base, value);
}
