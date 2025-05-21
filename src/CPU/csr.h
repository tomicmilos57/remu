#ifndef CSR_h
#define CSR_h
#include <cstdint>
#include "../MEM/mem.h"

class CSR{
  public:
    CSR(MEM& memory) : memory(memory) {}
    void set(uint32_t sel, uint32_t value);
    uint32_t read(uint32_t sel) const;

    uint32_t mhartid = 0;
    uint32_t mstatus = 0;
    uint32_t mepc = 0;
    uint32_t sstatus = 0;
    uint32_t sip = 0;
    uint32_t sie = 0;
    uint32_t mie = 0;
    uint32_t sepc = 0;
    uint32_t medeleg = 0;
    uint32_t mideleg = 0;
    uint32_t stvec = 0;
    uint32_t mtvec = 0;
    uint32_t satp = 0;
    uint32_t scause = 0;
    uint32_t mcause = 0;
    uint32_t stval = 0;
    uint32_t mcounter = 0;
    uint32_t time = 0;
    uint32_t sscratch = 0;
    uint32_t mscratch = 0;
  private:
    MEM& memory;
};

#endif
