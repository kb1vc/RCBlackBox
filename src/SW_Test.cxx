#include <iostream>
#include <pigpio.h>
#include "PI_IO.hxx"

int main() {
  BlackBox::initPIGPIO();
  
  const int sw_pin = 14;
  const int led_pin = 15;
  gpioSetMode(sw_pin, PI_INPUT);
  gpioSetPullUpDown(sw_pin, PI_PUD_UP);
  gpioSetMode(led_pin, PI_OUTPUT);

  int counter[16];
  int cidx = 0;

  while(1) {
    gpioWrite(led_pin, !gpioRead(sw_pin));
  }
}
