#ifndef PLIC_h
#define PLIC_h
#include <cstdint>
#include <fstream>
#include <string>
#include "device.h"
#include "../CPU/cpu.h"

class PLIC : public Device{
  public:
    PLIC(CPU& cpu);
    void store_byte(uint32_t address, uint8_t value) override;
    uint8_t fetch_byte(uint32_t address) override;

    void store_half(uint32_t address, uint16_t value) override;
    uint16_t fetch_half(uint32_t address) override;

    void store_word(uint32_t address, uint32_t value) override;
    uint32_t fetch_word(uint32_t address) override;

    void send_interrupt(uint32_t irq);
  private:
    CPU& cpu;
    uint32_t claim;
};

#endif
