#pragma once
#include <stdexcept>
#include <string>

namespace BlackBox {
  class PIIOException : public std::runtime_error {
  public:
    PIIOException(const std::string & what) : runtime_error("PIIO: " + what) { }
  };

  typedef void(*PinCallback)(int, int, unsigned int, void*);

  class PIIO {
  public:
    PIIO() {
    }

    virtual int setMode(unsigned int pin, unsigned int mode) = 0;
    virtual int setPullUpDown(unsigned int pin, unsigned int pud) = 0;
    
    virtual int read(unsigned int pin) = 0;
    virtual int write(unsigned int pin, unsigned int level) = 0;
    
    virtual bool setPinCallBack(unsigned int pin, PinCallback cb, void * userptr) = 0;
    virtual bool setISRCallBack(unsigned int pin,
				unsigned int edge, int timeout, 				  
				PinCallback cb, void * userptr) = 0;

    virtual bool setGlitchFilter(unsigned int pin, unsigned int us_delay) = 0; 

    // PWM functions
    virtual int setPWMRange(unsigned int pin, unsigned int range) = 0;
    virtual int setPWMFrequency(unsigned int pin, unsigned int freq) = 0; 
    virtual bool setPWM(unsigned int pin, unsigned int duty_cycle) = 0;

    // i2c functions
    virtual int openI2C(unsigned int bus, unsigned int addr, unsigned int flags) = 0;
    virtual bool closeI2C(int i2c_handle) = 0;
      
    virtual int readRegI2C(int i2c_handle, unsigned char reg) = 0;
    virtual int writeRegByteI2C(int i2c_handle, unsigned char reg, unsigned int val) = 0;
	
    virtual int readBlockI2C(int i2c_handle, unsigned char reg, int len, char * buf) = 0;
  };
}

