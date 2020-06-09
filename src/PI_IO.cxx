#include <pigpio.h>
#include "PI_IO.hxx"
#include <iostream>
#include <sys/time.h>
#include <stdexcept>

static bool pigpio_has_been_initialized = false;
namespace BlackBox {
  bool initPIGPIO() {
    if(pigpio_has_been_initialized) return true;

    // two peoples separated by a common language. 
    int stat = gpioInitialise();
    
    std::cerr << "PIGPIO initialized returns " << stat << "\n";

    // setup debug
    //    gpioCfgInternals(984762879, 7);
    
    return pigpio_has_been_initialized = (stat > 0);
  }

  bool closePIGPIO() {
    if(pigpio_has_been_initialized) gpioTerminate();
    pigpio_has_been_initialized = false; 
    return true; 
  }

}
