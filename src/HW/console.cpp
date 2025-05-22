#include "console.h"
#include <iostream>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#define UART_IRQ 10

Console::Console(PLIC& plic, UART& uart) : plic(plic), uart(uart) {}

Console::~Console() {}

void Console::simulate_input(){
  setNonBlockingInput(true);

  char c;
  if (read(STDIN_FILENO, &c, 1) > 0) {
    send_char(c);
  }

  setNonBlockingInput(false);
}

void Console::send_char(char c){
  uart.send_char(c);
  plic.send_interrupt(UART_IRQ);
}

void Console::setNonBlockingInput(bool enable) {
  static termios oldt;
  static bool isEnabled = false;

  if (enable && !isEnabled) {
    termios newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
    isEnabled = true;
  } else if (!enable && isEnabled) {
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, 0);
    isEnabled = false;
  }
}
