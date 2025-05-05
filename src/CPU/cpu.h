#ifndef CPU_h
#define CPU_h
#include "../MEM/mem.h"
#include <cstdint>

class CPU {

public:
  CPU(MEM memory);
  bool execute();

private:
  MEM& memory;
  uint32_t pc;
  uint32_t ir;

  uint32_t regfile[32];
};

#endif
