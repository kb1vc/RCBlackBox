#include "FXOS8700CQ.hxx"
#include <pigpio.h>
#include "PIIO.hxx"
#include <iostream>

namespace BlackBox {
  std::ostream & MXData::print(std::ostream & os) {
    os << "MX " << seq_no << " "
       << ax << ","
       << ay << ","
       << az << ","
       << mx << ","
       << my << ","
       << mz << "\n";
    return os; 
  }

  std::ostream & MXData::printFormat(std::ostream & os) {
    os << "FMT MX sequence_num accel_x accel_y accel_z mag_x mag_y mag_z\n";
    return os; 
  }

  FXOS8700CQ::FXOS8700CQ(PIIO * piio_p, unsigned char bus, unsigned char addr,
			 unsigned char int1_pin, 
			 Mode mode) : piio_p(piio_p) {
    sequence_number = 0;

    piio_p->openI2C(bus, addr, 0);
    
    init(mode, int1_pin);
  }

  FXOS8700CQ::~FXOS8700CQ() { 
    piio_p->closeI2C();
  }


  void FXOS8700CQ::dReadyIntCallback(int gpio, int level, unsigned int tick, void * obj) {
    FXOS8700CQ * accmag_p = (FXOS8700CQ *) obj;
    std::cerr << "[";        
    accmag_p->serviceDReady(gpio, level, tick);
  }

  int FXOS8700CQ::readDR(MXData & mx) {
    ISData raw;
    std::cerr << "{";            
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
    std::cerr << "}";                
    unsigned char stat = piio_p->readRegI2C(M_DR_STATUS_RO);
    if(stat & DRS_ZYXDR) {
      // read the combined mag and acc registers, starting with M_OUT_X_MSB
      //      readBlock(M_OUT_X_MSB_RO, sizeof(ISData), (char*) &raw);
      piio_p->readBlockI2C(OUT_X_MSB_RO, sizeof(ISData), (char*) &raw);      
      return 1; 
    }
    else {
      return 0;
    }
  }

  void FXOS8700CQ::serviceDReady(int gpio, int level, unsigned int tick) {
    // read the rates and store them in the outbound queue.
    ISData raw;
    std::cerr << "%";
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
    std::cerr << "|";    
    piio_p->writeRegByteI2C(CTRL_REG1_RW, 0);

    // enable the accel and mag in hybrid mode
    // sample at 6.25 Hz
    piio_p->writeRegByteI2C(M_CTRL_REG1_RW, MCR1_ACAL_ENA |  MCR1_HMS::BOTH | ((0x7 & MCR1_OSR_M) << MCR1_OSR_S));

    // couple the mag and acc registers in a block,
    // all others are default
    piio_p->writeRegByteI2C(M_CTRL_REG2_RW, MCR2_HYB_AUTO_INC);

    // set acc to 2G full scale, no HPF
    piio_p->writeRegByteI2C(XYZ_DATA_CFG_RW, XYZDC_FS_2G);


    if(mode == DR_INT) {
      // interrupt from data ready...
      piio_p->writeRegByteI2C(CTRL_REG3_RW, CR3_IPOL);
      piio_p->writeRegByteI2C(CTRL_REG4_RW, CR4_INT_EN_DRDY);
      piio_p->writeRegByteI2C(CTRL_REG5_RW, CR5_INT_CFG_DRDY);
      piio_p->setISRCallBack(int1_pin, RISING_EDGE, 0, dReadyIntCallback, this);
    }
  }
  
  int FXOS8700CQ::getMX(int max_samps, MXData * dat_p) {
    int i;
    {
      std::lock_guard<std::mutex> lck(gq_mutex);
      for(i = 0; i < max_samps; i++) {
	if(mx_queue.empty()) {
	  return i;
	}
	else {
	  dat_p[i] = mx_queue.front(); mx_queue.pop();
	}
      }
    }
    return max_samps;
  }
  void FXOS8700CQ::start() {
    // turn on the accelerometer
    std::cerr << "@";
    piio_p->writeRegByteI2C(CTRL_REG1_RW, (CR1_DATA_RATE_6r25 << CR1_DR_S) | CR1_LNOISE | CR1_ACTIVE);
  }
}
