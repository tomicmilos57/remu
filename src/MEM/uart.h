#ifndef UART_h
#define UART_h
#include <cstdint>
#include <fstream>
#include <string>
#include "device.h"

class UART : public Device{
  public:
    UART();
    void store_byte(uint32_t address, uint8_t value) override;
    uint8_t fetch_byte(uint32_t address) override;

    void send_char(char c);
  private:
};


#endif
