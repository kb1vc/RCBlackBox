#include <pigpio.h>
#include "PI_IO.hxx"

static bool pigpio_has_been_initialized = false;

bool BlackBox::initPIGPIO() {
  if(pigpio_has_been_initialized) return true;

  // two peoples separated by a common language. 
  int stat = gpioInitialise();

  return pigpio_has_been_initialized = (stat > 0);
}

bool BlackBox::closePIGPIO() {
  if(pigpio_has_been_initialized) gpioTerminate();
  pigpio_has_been_initialized = false; 
  return true; 
}
