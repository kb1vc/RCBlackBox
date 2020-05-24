#include <iostream>
#include <pigpio.h>
#include "PI_IO.hxx"

int main() {
  BlackBox::initPIGPIO();

  gpioSetMode(23, PI_INPUT);
  gpioSetPullUpDown(23, PI_PUD_UP);

  int i = 0;
  std::cerr << "First read of 23\n";
  while(!gpioRead(23)) { i++; }

  std::cerr << "Pin 23 was low for " << i << " iterations.  It is now high\n";

}
