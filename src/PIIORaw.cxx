#include <pigpio.h>
#include "PIIORaw.hxx"
#include <iostream>
#include <sys/time.h>
#include <stdexcept>


namespace BlackBox {

  bool PIIORaw::pigpio_has_been_initialized = false;

  bool PIIORaw::initPIGPIO() {
    std::cerr << "initPIGPIO\n";
    if(pigpio_has_been_initialized) return true;
    
    // disable the network interfaces
    gpioCfgInterfaces(PI_DISABLE_SOCK_IF);
    std::cerr << "\tInitializing PIGPIO\n";
    // two peoples separated by a common language. 
    int stat = gpioInitialise();
    
    if(stat == PI_INIT_FAILED) {
      throw PIIOException("Could not initialize raw pigpio.");
    }

    return pigpio_has_been_initialized = (stat > 0);
  }

  bool PIIORaw::closePIGPIO() {
    if(pigpio_has_been_initialized) gpioTerminate();
    pigpio_has_been_initialized = false; 
    return true; 
  }

  // most of these are wrappers for the PIGPIO calls, but they
  // give us the ability to target both direct and daemon mediated
  // access to the GPIOs. 
  int PIIORaw::setMode(unsigned int pin, unsigned int mode) {
    return gpioSetMode(pin, mode); 
  }

  int PIIORaw::setPullUpDown(unsigned int pin, unsigned int pud) {
    return gpioSetPullUpDown(pin, pud);
  }
    
  int PIIORaw::read(unsigned int pin) {
    return gpioRead(pin);
  }
  
  int PIIORaw::write(unsigned int pin, unsigned int level) {
    return gpioWrite(pin, level);
  }
    
  bool PIIORaw::setPinCallBack(unsigned int pin, 
			       PinCallback cb, void * userptr) {
    int stat = gpioSetAlertFuncEx(pin, cb, userptr);
    return stat == 0;
  }

  bool PIIORaw::setISRCallBack(unsigned int pin, 
			       unsigned int edge, int timeout, 
			       PinCallback cb, void * userptr) {
    int stat = gpioSetISRFuncEx(pin, edge, timeout, cb, userptr);
    return stat == 0;
  }

  bool PIIORaw::setGlitchFilter(unsigned int pin, unsigned int us_delay) {
    return gpioGlitchFilter(pin, us_delay) == 0;
  }

  // i2c functions
  int PIIORaw::openI2C(unsigned int bus, unsigned int addr, unsigned int flags) {
    int i2c_handle = i2cOpen(bus, addr, flags);
    std::cerr << "OpenI2C returns " << i2c_handle << "\n";
    return i2c_handle;
  }

  bool PIIORaw::closeI2C(int i2c_handle) {
    if(i2c_handle >= 0) {
      return i2cClose(i2c_handle) == 0;
    }
    return false; 
  }
      
  int PIIORaw::readRegI2C(int i2c_handle, unsigned char reg) {
    return i2cReadByteData(i2c_handle, reg);
  }
  
  int PIIORaw::writeRegByteI2C(int i2c_handle, unsigned char reg, unsigned int val) {
    return i2cWriteByteData(i2c_handle, reg, val);
  }
	
  int PIIORaw::readBlockI2C(int i2c_handle, unsigned char reg, int len, char * buf) {
    return i2cReadI2CBlockData(i2c_handle, reg, buf, len);
  }


  int PIIORaw::setPWMRange(unsigned int pin, unsigned int range) {
    return gpioSetPWMrange(pin, range);
  }
  
  int PIIORaw::setPWMFrequency(unsigned int pin, unsigned int freq) {
    return gpioSetPWMfrequency(pin, freq);
  }
  
  bool PIIORaw::setPWM(unsigned int pin, unsigned int duty_cycle) {
    return gpioPWM(pin, duty_cycle) == 0;
  }
  
}
