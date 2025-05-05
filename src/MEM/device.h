#ifndef DEVICE_h
#define DEVICE_h
#include <cstdint>
#include <fstream>
#include <string>

struct Device {
  public:
    virtual uint8_t  fetch_byte(uint32_t address);
    virtual uint16_t fetch_half(uint32_t address);
    virtual uint32_t fetch_word(uint32_t address);

    virtual void store_byte(uint32_t address, uint8_t value);
    virtual void store_half(uint32_t address, uint16_t value);
    virtual void store_word(uint32_t address, uint32_t value);

    virtual ~Device();
  protected:
    uint8_t *memory;
    int size;
    uint32_t base;
};

class RAM : public Device{
  public:
    RAM(int size, std::string filename);

  private:
};


class GPU : public Device{
  public:
    GPU();
    GPU(std::string filename);

  private:
};









#endif
