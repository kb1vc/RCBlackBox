#include "FXOS8700CQ.hxx"
#include <pigpio.h>
#include "PI_IO.hxx"
#include <iostream>

namespace BlackBox {
  FXOS8700CQ::FXOS8700CQ(unsigned char bus, unsigned char addr,
			 unsigned char int1_pin, 
			 Mode mode) : FXBase(bus, addr) {
    sequence_number = 0;

    init(mode, int1_pin);
  }

  FXOS8700CQ::~FXOS8700CQ() { 
    i2cClose(i2c_handle);
  }


  void FXOS8700CQ::dReadyIntCallback(int gpio, int level, unsigned int tick, void * obj) {
    FXOS8700CQ * accmag_p = (FXOS8700CQ *) obj; 
    accmag_p->serviceDReady(gpio, level, tick);
  }

  int FXOS8700CQ::readDR(MXData & mx) {
    ISData raw;
    if(readDR(raw)) {
      IS2MXData(mx, raw);
      mx.seq_no = sequence_number ++; 
      return 1; 
    }
    else return 0;
  }
  int FXOS8700CQ::readDR(ISData & raw) {
    // return 0 if we got nothing
    // first read the STATUS register. 
    unsigned char stat = readByte(M_DR_STATUS_RO);
    if(stat & DRS_ZYXDR) {
      // read the combined mag and acc registers, starting with M_OUT_X_MSB
      //      readBlock(M_OUT_X_MSB_RO, sizeof(ISData), (char*) &raw);
      readBlock(OUT_X_MSB_RO, sizeof(ISData), (char*) &raw);      
      std::cerr << "# " << std::hex
		<< raw.mx << " "
		<< raw.my << " "
		<< raw.mz << " "	
		<< raw.ax << " "
		<< raw.ay << " "
		<< raw.az << "\n";
      return 1; 
    }
    else {
      return 0;
    }
  }

  void FXOS8700CQ::serviceDReady(int gpio, int level, unsigned int tick) {
    // read the rates and store them in the outbound queue.
    ISData raw; 
    if(readDR(raw)) {
      // lock the queue
      std::lock_guard<std::mutex> lck(gq_mutex);
      MXData v;
      IS2MXData(v, raw);
      v.seq_no = sequence_number++; 
      if(mx_queue.size() < MX_QUEUE_MAXLEN) {
	mx_queue.push(v);
      }
    }
  }
  

  void FXOS8700CQ::init(Mode mode, unsigned char int1_pin) {
    // put the device to sleep
    writeByte(CTRL_REG1_RW, 0);

    // enable the accel and mag in hybrid mode
    // sample at 6.25 Hz
    writeByte(M_CTRL_REG1_RW, MCR1_HMS::BOTH | ((0x7 & MCR1_OSR_M) << MCR1_OSR_S));

    // couple the mag and acc registers in a block,
    // all others are default
    writeByte(M_CTRL_REG2_RW, MCR2_HYB_AUTO_INC);

    // set acc to 2G full scale, no HPF
    writeByte(XYZ_DATA_CFG_RW, XYZDC_FS_2G);


    if(mode == DR_INT) {
      // interrupt from data ready... 
      writeByte(CTRL_REG4_RW, CR4_INT_EN_DRDY);
      writeByte(CTRL_REG5_RW, CR5_INT_CFG_DRDY); // send int to INT1.
      gpioSetISRFuncEx(int1_pin, RISING_EDGE, 0, dReadyIntCallback, this);
    }
  }
  
  int FXOS8700CQ::getMX(int max_samps, MXData * dat_p) {
    int i; 
    for(i = 0; i < max_samps; i++) {
      if(mx_queue.empty()) return i;
      dat_p[i] = mx_queue.front(); mx_queue.pop();
    }
    return max_samps;
  }
  void FXOS8700CQ::start() {
    // turn on the accelerometer
    writeByte(CTRL_REG1_RW, (CR1_DATA_RATE_6r25 << CR1_DR_S) | CR1_LNOISE | CR1_ACTIVE);
  }
}
