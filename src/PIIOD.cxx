#include <pigpiod_if2.h>
#include "PIIOD.hxx"
#include <iostream>
#include <sys/time.h>
#include <stdexcept>


namespace BlackBox {

  bool PIIOD::initPIGPIO() {
    pigpio_server = pigpio_start(NULL, NULL);
    return true; 
  }

  bool PIIOD::closePIGPIO() {
    pigpio_stop(pigpio_server);
    return true; 
  }

  // most of these are wrappers for the PIGPIO calls, but they
  // give us the ability to target both direct and daemon mediated
  // access to the GPIOs. 
  int PIIOD::setMode(unsigned int pin, unsigned int mode) {
    return set_mode(pigpio_server, pin, mode); 
  }

  int PIIOD::setPullUpDown(unsigned int pin, unsigned int pud) {
    return set_pull_up_down(pigpio_server, pin, pud);
  }
    
  int PIIOD::read(unsigned int pin) {
    return gpio_read(pigpio_server, pin);
  }
  
  int PIIOD::write(unsigned int pin, unsigned int level) {
    return gpio_write(pigpio_server, pin, level);
  }
    
  int PIIOD::setupCallback(unsigned int pin, unsigned int edge, 
			   PinCallbackRec * cbr) {
    return callback_ex(pigpio_server, pin, edge, PIIOD::handleCallback, cbr);

  }

  void PIIOD::handleCallback(int server, unsigned int gpio, unsigned int level, 
			     unsigned int tick, void * userdata) {
    auto cbr = (PinCallbackRec *) userdata;

    cbr->cb(gpio, level, tick, cbr->userptr);
  }

  bool PIIOD::setPinCallBack(unsigned int pin, 
			       PinCallback cb, void * userptr) {
    return setupCallback(pin, EITHER_EDGE, new PinCallbackRec(cb, userptr));
    
    int stat = callback_ex(pigpio_server, pin, EITHER_EDGE, (CBFuncEx_t) cb, userptr);
    return stat == 0;
  }

  bool PIIOD::setISRCallBack(unsigned int pin, 
			       unsigned int edge, int timeout, 
			       PinCallback cb, void * userptr) {
    int stat = callback_ex(pigpio_server, pin, edge, (CBFuncEx_t) cb, userptr);
    return stat == 0;
  }

  bool PIIOD::setGlitchFilter(unsigned int pin, unsigned int us_delay) {
    return set_glitch_filter(pigpio_server, pin, us_delay) == 0;
  }

  // i2c functions
  bool PIIOD::openI2C(unsigned int bus, unsigned int addr, unsigned int flags) {
    i2c_handle = i2c_open(pigpio_server, bus, addr, flags);
    return i2c_handle >= 0;
  }

  bool PIIOD::closeI2C() {
    if(i2c_handle >= 0) {
      return i2c_close(pigpio_server, i2c_handle) == 0;
    }
    return false; 
  }
      
  int PIIOD::readRegI2C(unsigned char reg) {
    return i2c_read_byte_data(pigpio_server, i2c_handle, reg);
  }
  
  int PIIOD::writeRegByteI2C(unsigned char reg, unsigned int val) {
    return i2c_write_byte_data(pigpio_server, i2c_handle, reg, val);
  }
	
  int PIIOD::readBlockI2C(unsigned char reg, int len, char * buf) {
    return i2c_read_i2c_block_data(pigpio_server, i2c_handle, reg, buf, len);
  }


  int PIIOD::setPWMRange(unsigned int pin, unsigned int range) {
    return set_PWM_range(pigpio_server, pin, range);
  }
  
  int PIIOD::setPWMFrequency(unsigned int pin, unsigned int freq) {
    return set_PWM_frequency(pigpio_server, pin, freq);
  }
  
  bool PIIOD::setPWM(unsigned int pin, unsigned int duty_cycle) {
    return set_PWM_dutycycle(pigpio_server, pin, duty_cycle) == 0;
  }
  
}
