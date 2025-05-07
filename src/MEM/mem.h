#ifndef MEM_h
#define MEM_h
#include <cstdint>
#include <vector>
#include "device.h"

class MEM {

public:
  void register_device(Device& dev);

  uint8_t fetch_byte(uint32_t address);
  uint16_t fetch_half(uint32_t address);
  uint32_t fetch_word(uint32_t address);

  void store_byte(uint32_t address, uint8_t byte);
  void store_half(uint32_t address, uint16_t half);
  void store_word(uint32_t address, uint32_t word);

private:
  std::vector<Device*> registered_devices;
  Device* get_device(uint32_t address);
};

#endif
