#ifndef MEM_h
#define MEM_h
#include <cstdint>
#include <vector>
#include "device.h"

class MEM {

  public:
    void register_device(Device& dev, uint32_t base);

    uint8_t fetch_byte(uint32_t address);
    uint16_t fetch_half(uint32_t address);
    uint32_t fetch_word(uint32_t address);

    void store_byte(uint32_t address, uint8_t byte);
    void store_half(uint32_t address, uint16_t half);
    void store_word(uint32_t address, uint32_t word);

    ~MEM();
  private:
    class Module{
      public:
        Module(Device* dev, uint32_t base) : dev(dev), base(base) {}
        Device* dev;
        uint32_t base = 0;
    };
    std::vector<Module*> registered_devices;
    Module* get_device(uint32_t address);
};

#endif
