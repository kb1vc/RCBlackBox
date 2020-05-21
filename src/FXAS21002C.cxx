#include "FXAS21002C.hxx"
#include <pigpio.h>
#include <stdexcept>
#include "PI_IO.hxx"
#include <iostream>
#include <mutex>

namespace BlackBox {

  void dumpBuf(char * buf, int len) {
    for(int i = 0; i < len; i++) {
      std::cerr.width(2);
      std::cerr << std::hex << (((unsigned int) buf[i]) & 0x0ff) << " ";
    }
    std::cerr << "\n";
  }


  void FXAS21002C::fifoIntCallback(int gpio, int level, unsigned int tick, void * obj) {
    FXAS21002C * gyro_p = (FXAS21002C *) obj; 

    gyro_p->serviceFIFO(gpio, level, tick);
  }

  void FXAS21002C::dReadyIntCallback(int gpio, int level, unsigned int tick, void * obj) {
    FXAS21002C * gyro_p = (FXAS21002C *) obj; 

    gyro_p->serviceDReady(gpio, level, tick);
  }
  
  FXAS21002C::FXAS21002C(unsigned char bus, unsigned char addr, 
			 unsigned char int1_pin,
			 Mode mode) : FXBase(bus, addr) {

    sequence_number = 0;
    
    init(mode, int1_pin);
  }

  FXAS21002C::~FXAS21002C() {
    i2cClose(i2c_handle);
  }

  void FXAS21002C::writeByte(unsigned char reg, unsigned char dat) {
    std::cerr << "Writing i2c byte  handle = " << i2c_handle 
	      << "reg = " << std::hex
	      << ((unsigned int) reg) << " " << ((unsigned int) dat) << std::dec << "\n";
    int stat = i2cWriteByteData(i2c_handle, reg, dat); 
    
    if(stat != 0) {
      std::cerr << "Got i2cWriteByteData error: " << stat << "\n";
      throw std::runtime_error("FXAS21002C: Failed to write byte.\n");
    }
  }

  unsigned char FXAS21002C::readByte(unsigned char reg) {
    int ret = i2cReadByteData(i2c_handle, reg); 

    if(ret < 0) {
      std::cerr << "Got i2cReadByteData error: " << ret << "\n";
      throw std::runtime_error("FXAS21002C: Failed to read byte.\n");
    }

    return (unsigned char) (ret & 0xff);
  }

  void FXAS21002C::readBlock(unsigned char reg, int len, char * buf) {
    int l = len;
    char * bp = buf;
    while(l) {
      int tl = (l > 30) ? 30 : l;
      l = l - tl;
      int stat = i2cReadI2CBlockData(i2c_handle, reg, bp, tl); 
      if(stat < 0) {
	std::cerr << "Got i2cReadI2CBlockData error: " << stat << "\n";
	throw std::runtime_error("FXAS21002C: Failed to read block.\n");
      }
      bp += tl;      
    }
  }

  void FXAS21002C::start(CR1_DATA_RATE data_rate) {
    writeByte(CTRL_REG1_RW, data_rate | CR1_ACTIVE);
  }

  
  // return the number of samples read. 
  // rate block must be at least 32 IRates long (192 bytes)
  int FXAS21002C::readFIFO() {
    // look at the fifo status register first. 
    unsigned char fstatus = readByte(F_STATUS_RO);
    int fifo_entries = (fstatus & FST_F_COUNT_M);
    int fifo_bytes = 6 * fifo_entries;
    // now read as many entries as are apparently in the FIFO
    // we're using the fact that reads from the XYZ registers 
    // auto-increment to the next register and wrap back around
    // to X_MSB.
    if(fifo_entries > 0) {
      readBlock(OUT_X_MSB_RO, fifo_bytes, (char*) rate_block); 
    }
    return fifo_entries; 
  }

  int FXAS21002C::readDR(IRates & rates) {
    // return 0 if we got nothing
    // first read the STATUS register. 
    unsigned char stat = readByte(STATUS_RO);
    if(stat & DRS_ZYXDR) {
      // note the 6 -- this is the number of registers 
      // (bytes) required to hold a set of xyz rates.
      readBlock(OUT_X_MSB_RO, 6, (char*) &rates);
    }
    
    return 1; 
  }
  
  void FXAS21002C::init(Mode mode, unsigned char int1_pin) {

    // put the device in standby mode
    std::cerr << "Need to put device in standby mode\n";
    // put in standby.
    // since ACTIVE and READY are both 0, the unit will be in standby
    writeByte(CTRL_REG1_RW, 0);
    
    // setup the interrupts and CR 2. 
    unsigned char creg2 = 0;    
    if(mode == FIFO) {
      // enable the fifo
      // and set the watermark at 16 -- once we have this many samples, we
      // will get an interrupt on pin 1.
      writeByte(F_SETUP_RW, FS_MODE_STOP | (FS_WMRK_M & 16));
      // LPF at 8 Hz, limit range to about 3 RPS
      writeByte(CTRL_REG0_RW, CR0_LPF_M | CR0_HPF_3 | CR0_HPF_EN | CR0_RANGE_1000);
      
      // FIFO interrupt on int1 pin
      // active high, totem pole output.
      writeByte(CTRL_REG2_RW, 
		CR2_INT_CFG_FIFO_I1 | CR2_INT_EN_FIFO | CR2_IPOL_HI);

      // wrap to one to allow fast dump of the FIFO
      writeByte(CTRL_REG3_RW, CR3_WRAPTOONE); 

      // connect the ISR
      int stat = gpioSetISRFuncEx(int1_pin, RISING_EDGE, 0, fifoIntCallback, this);
    }
    else {  // mode must be DR_INT or DR_POLL
      // disable the fifo
      writeByte(F_SETUP_RW, 0);
      // LPF at 8 Hz, limit range to about 3 RPM
      writeByte(CTRL_REG0_RW, CR0_LPF_H | CR0_RANGE_1000);

      // data ready interrupt on int1 pin
      // active high, totem pole output.
      if(mode == DR_POLL) {
	writeByte(CTRL_REG2_RW, 
			 CR2_INT_CFG_DRDY_I1 | CR2_INT_EN_DRDY | CR2_IPOL_HI);
      }
      else {
	writeByte(CTRL_REG2_RW, 0);
      }

      // wrap to zero so we can read the status register and all the data
      // at once. 
      writeByte(CTRL_REG3_RW, 0);

      int stat = gpioSetISRFuncEx(int1_pin, RISING_EDGE, 0, dReadyIntCallback, this);      
    }

    // disable rate threshold    
    writeByte(RT_CFG_RW, 0);

  }

  void FXAS21002C::serviceFIFO(int gpio, int level, unsigned int tick) {
    // read the rates and store them in the outbound rate queue. 
    int num_samps = readFIFO();
    // anything to push?
    if((num_samps > 0) && (gyro_rates.size() < GYRO_RATE_QUEUE_MAXLEN)) {
      // lock the queue
      std::lock_guard<std::mutex> lck(gq_mutex);
      for(int i = 0; i < num_samps; i++) {
	Rates v;
	v.x = rate_block[i].x;
	v.y = rate_block[i].y;
	v.z = rate_block[i].z;
	v.seq_no = sequence_number++;
	gyro_rates.push(v);
      }
    }
  }

  void FXAS21002C::serviceDReady(int gpio, int level, unsigned int tick) {
    // read the rates and store them in the outbound rate queue. 
    if(readDR(rate_block[0])) {
      // lock the queue
      std::lock_guard<std::mutex> lck(gq_mutex);
      Rates v;
      v.x = rate_block[0].x;
      v.y = rate_block[0].y;
      v.z = rate_block[0].z;
      v.seq_no = sequence_number++; 
      if(gyro_rates.size() < GYRO_RATE_QUEUE_MAXLEN) {
	gyro_rates.push(v);
      }
    }
  }

  int FXAS21002C::getRates(int max_samples, Rates * samps) {
    int rv = 0;
    {
      std::lock_guard<std::mutex> lck(gq_mutex);
      if(!gyro_rates.empty()) {
	for(int i = 0; i < gyro_rates.size(); i++) {
	  samps[i] = gyro_rates.front();
	  gyro_rates.pop();
	  rv++;
	}
      }
    }
    return rv;
  }
}
