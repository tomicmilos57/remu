#ifndef SD_CARD_H
#define SD_CARD_H
#include "../MEM/mem.h"
#include <cstdint>

class SD_CARD : public Device{
  public:
    SD_CARD();
    ~SD_CARD();
    void simulate_sd_card();

  private:
    uint8_t *disk_memory;
    uint32_t fetch_local_word(uint32_t address);
};

#endif
