#pragma once
#include "PIIO.hxx"

namespace BlackBox {

  class PIIOD : public PIIO {
  public:
    PIIOD() : PIIO() {
      initPIGPIO();
    }

    ~PIIOD() {
      closeI2C();
      closePIGPIO();
    }
    
    int setMode(unsigned int pin, unsigned int mode);
    int setPullUpDown(unsigned int pin, unsigned int pud);
    
    int read(unsigned int pin);
    int write(unsigned int pin, unsigned int level);
    
    bool setPinCallBack(unsigned int pin, PinCallback cb, void * userptr);
    bool setISRCallBack(unsigned int pin,
			unsigned int edge, int timeout, 			  
			PinCallback cb, void * userptr);

    bool setGlitchFilter(unsigned int pin, unsigned int us_delay); 

    // PWM functions
    int setPWMRange(unsigned int pin, unsigned int range);
    int setPWMFrequency(unsigned int pin, unsigned int freq);
    bool setPWM(unsigned int pin, unsigned int duty_cycle);

    // i2c functions
    bool openI2C(unsigned int bus, unsigned int addr, unsigned int flags);
    bool closeI2C();
      
    int readRegI2C(unsigned char reg);
    int writeRegByteI2C(unsigned char reg, unsigned int val);
	
    int readBlockI2C(unsigned char reg, int len, char * buf);

  protected:
    /**
     * @brief Initialize PIGPIO system, but do it only once. 
     * 
     * @return true if all was well, false otherwise. 
     */
    bool initPIGPIO();
    
    bool closePIGPIO();

    int i2c_handle; 
    
    int pigpio_server;

    class PinCallbackRec {
    public:
      PinCallbackRec(PinCallback cb, void * userptr) : cb(cb), userptr(userptr) { }
      PinCallback cb;
      void * userptr;
    };

    int setupCallback(unsigned int pin, unsigned int edge, PinCallbackRec * cbr);
    static void handleCallback(int server, unsigned int gpio, 
			       unsigned int level, unsigned int tick, void * userdata);
    
  };
}


