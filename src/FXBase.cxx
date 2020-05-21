#include <pigpio.h>
#include <stdexcept>
#include "PI_IO.hxx"
#include <iostream>
#include <mutex>
#include "FXBase.hxx"

namespace BlackBox {
  FXBase::FXBase(unsigned char bus, unsigned char addr) {
    if(!initPIGPIO()) {
      throw std::runtime_error("Failed to initialize PIGPIO.");
    }
    
    i2c_handle = i2cOpen(bus, addr, 0);
    std::cerr << "I2C open returns handle = " << i2c_handle << "\n";
    if(i2c_handle < 0) {
      std::cerr << "Got i2cOpen error: " << i2c_handle << "\n";
      throw std::runtime_error("FXAS21002C: Failed to open i2c device .\n");
    }
  }
  void FXBase::writeByte(unsigned char reg, unsigned char dat) {
    std::cerr << "Writing i2c byte  handle = " << i2c_handle 
	      << "reg = " << std::hex
	      << ((unsigned int) reg) << " " << ((unsigned int) dat) << std::dec << "\n";
    int stat = i2cWriteByteData(i2c_handle, reg, dat); 
    
    if(stat != 0) {
      std::cerr << "Got i2cWriteByteData error: " << stat << "\n";
      throw std::runtime_error("FXBase: Failed to write byte.\n");
    }
  }

  unsigned char FXBase::readByte(unsigned char reg) {
    int ret = i2cReadByteData(i2c_handle, reg); 

    if(ret < 0) {
      std::cerr << "Got i2cReadByteData error: " << ret << "\n";
      throw std::runtime_error("FXBase: Failed to read byte.\n");
    }

    return (unsigned char) (ret & 0xff);
  }

  void FXBase::readBlock(unsigned char reg, int len, char * buf) {
    int l = len;
    char * bp = buf;
    while(l) {
      int tl = (l > 30) ? 30 : l;
      l = l - tl;
      int stat = i2cReadI2CBlockData(i2c_handle, reg, bp, tl); 
      if(stat < 0) {
	std::cerr << "Got i2cReadI2CBlockData error: " << stat << "\n";
	throw std::runtime_error("FXBase: Failed to read block.\n");
      }
      bp += tl;      
    }
  }
}
