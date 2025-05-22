#ifndef CONSOLE_H
#define CONSOLE_H
#include "../MEM/plic.h"
#include "../MEM/uart.h"
#include <cstdint>

class Console {
public:
  Console(PLIC& plic, UART& uart);
  ~Console();
  void simulate_input();

private:
  PLIC& plic;
  UART& uart;

  void setNonBlockingInput(bool enable);
  void send_char(char c);
};

#endif
