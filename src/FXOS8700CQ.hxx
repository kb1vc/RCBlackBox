#pragma once

#include <mutex>
#include <queue>
#include <iostream>

namespace BlackBox {
  class PIIO; 
  
  // this is the byte-order corrected signed
  // measured data, magnetometer, and accelerometer
  struct MXData {
    short ax, ay, az;    
    short mx, my, mz;
    int seq_no;

    static std::ostream & printFormat(std::ostream & os);
    
    std::ostream & print(std::ostream & os);
  };
  
  class FXOS8700CQ {
  public:
    enum Mode { DR_INT, DR_POLL };    
    
    /**
     *
     * @param bus which i2c bus is the gyro on?  (1)
     * @param addr i2c address for the gyro
     * @param int1_pin first of two int pins from the chip
     * @param int2_pin second of two int pins from the chip
     */
    FXOS8700CQ(PIIO * piio_p, unsigned char bus, unsigned char addr,
	       unsigned char int1_pin, 
	       Mode mode);
    
    ~FXOS8700CQ();

    int getMX(int max_samps, MXData * dat_p);
    
    void stop() { }
    
  protected:

    PIIO * piio_p; // pointer to gpio interface. 
    int i2c_handle;
    
#pragma pack(push, 1)
    // want this in a contiguous 12 byte block.
    // This is the structure that mimics the layout of the
    // registers starting with M_OUT_X_MSB down to CMP_Z_LSB
    // This allows us to read simultaneous accellerometer and
    // magnetometer values. 
    struct ISData {
      unsigned short ax, ay, az;      
      unsigned short mx, my, mz;
    };
#pragma pack(pop)

    void IS2MXData(MXData & to, const ISData & from) {
      to.mx = magSwap(from.mx);
      to.my = magSwap(from.my);
      to.mz = magSwap(from.mz);      
      to.ax = accSwap(from.ax);
      to.ay = accSwap(from.ay);
      to.az = accSwap(from.az);      
    }

    short accSwap(unsigned short in) {
      // acceleration values are 14 bits, but MSB first.  Swap the bytes
      unsigned short ret = magSwap(in) >> 2;
      if(ret & 0x2000) {
	ret |= 0xc000; // sign extend. 
      }
      return ret;
    }
    
    short magSwap(unsigned short in) {
      // mag values are 16 bits, but MSB first.  Swap the bytes
      unsigned short ret;
      ret = (in << 8) | (in >> 8);
      return ((short) ret);
    }
    
    std::queue<MXData> mx_queue;
    unsigned short sequence_number;
    static const int MX_QUEUE_MAXLEN = 1024;
    std::mutex gq_mutex;

    
    // register definitions, and bit fields.
    // register definitions, and bit fields.
    static const unsigned char STATUS_RO = 0x0;
    static const unsigned char M_DR_STATUS_RO = 0x32;    
    // data register mode
    static const unsigned char DRS_ZYXOW = 0x80;
    static const unsigned char DRS_ZOW = 0x40;
    static const unsigned char DRS_YOW = 0x20;
    static const unsigned char DRS_XOW = 0x10;        
    static const unsigned char DRS_ZYXDR = 0x8;
    static const unsigned char DRS_ZDR = 0x4;
    static const unsigned char DRS_YDR = 0x2;
    static const unsigned char DRS_XDR = 0x1;
    // fifo mode
    static const unsigned char FST_F_OVF = 0x80;
    static const unsigned char FST_F_WMKF = 0x40;
    static const unsigned char FST_F_COUNT_M = 0x3f;

    static const unsigned char OUT_X_MSB_RO = 0x1;
    static const unsigned char OUT_X_LSB_RO = 0x2;    
    static const unsigned char OUT_Y_MSB_RO = 0x3;
    static const unsigned char OUT_Y_LSB_RO = 0x4;    
    static const unsigned char OUT_Z_MSB_RO = 0x5;
    static const unsigned char OUT_Z_LSB_RO = 0x6;    

    static const unsigned char F_SETUP_RW = 0x9;
    enum FS_MODE {
		  FIFO_TRIGGER = 0xc0,
		  FIFO_STOP_ON_OVF = 0x80,
		  FIFO_CIRC = 0x40,
		  FIFO_DIS = 0x0
    };
    static const unsigned char FS_WMRK_M = 0x3f;
    static const unsigned char FS_WMRK_S = 0;

    static const unsigned char TRIG_CFG_RW = 0xa;
    static const unsigned char TC_TRANS = 0x20;
    static const unsigned char TC_LNDPRT = 0x10;
    static const unsigned char TC_PULSE = 0x8;
    static const unsigned char TC_FFMT = 0x4;
    static const unsigned char TC_A_VECM = 0x2;
    
    static const unsigned char SYSMOD_RO = 0xb;
    static const unsigned char SM_FGERR = 0x80;
    static const unsigned char SM_FGT_M = 0x3f;
    static const unsigned char SM_FGT_S = 2;
    static const unsigned char SM_SYSMOD_M = 0x3;
    static const unsigned char SM_SYSMOD_S = 0;
    
    static const unsigned char INT_SOURCE_RO = 0xc;
    static const unsigned char IS_ASLP = 0x80;
    static const unsigned char IS_FIFO = 0x40;
    static const unsigned char IS_TRANS = 0x20;
    static const unsigned char IS_LNDPRT = 0x10;
    static const unsigned char IS_PULSE = 0x8;
    static const unsigned char IS_FFMT = 0x4;
    static const unsigned char IS_A_VECM = 0x2;
    static const unsigned char IS_DRDY = 0x1;
    
    static const unsigned char WHO_AM_I_RO = 0xd;
    
    static const unsigned char XYZ_DATA_CFG_RW = 0xe;
    static const unsigned char XYZDC_HPF_EN = 0x10;
    static const unsigned char XYZDC_FS_2G = 0x0;
    static const unsigned char XYZDC_FS_4G = 0x1;
    static const unsigned char XYZDC_FS_8G = 0x2;
    
    static const unsigned char HP_FILTER_CUTOFF_RW = 0xf;
    static const unsigned char HPF_PULSE_HPF_EN = 0x20;
    static const unsigned char HPF_PULSE_LPF_EN = 0x10;
    static const unsigned char HPF_HI = 0;
    static const unsigned char HPF_LO = 1;
    
    static const unsigned char PL_STATUS_R = 0x10;
    static const unsigned char PLS_NEWLP = 0x80;
    static const unsigned char PLS_LO = 0x40;
    static const unsigned char PLS_PORT_UP = 0x0;
    static const unsigned char PLS_PORT_DOWN = 0x2;
    static const unsigned char PLS_LAND_RT = 0x4;
    static const unsigned char PLS_LAND_LF = 0x6;
    static const unsigned char PLS_BAFRO = 1;

    
    static const unsigned char PL_CFG_RW = 0x11;
    static const unsigned char PLC_DBNCE_CLR = 0x80;
    static const unsigned char PLC_EN = 0x40;
    
    static const unsigned char PL_COUNT_RW = 0x12;

    static const unsigned char PL_BF_ZCOMP_RW = 0x13;
    static const unsigned char PLBZ_BKFR_M = 0x3;
    static const unsigned char PLBZ_BKFR_S = 6;
    static const unsigned char PLBZ_LOCK_M = 0x7;
    static const unsigned char PLBZ_LOCK_S = 0;

    static const unsigned char PL_THS_REG = 0x14;
    static const unsigned char PLT_THS_M = 0x1f;
    static const unsigned char PLT_THS_S = 3;
    static const unsigned char PLT_HYS_M = 0x7;
    static const unsigned char PLT_HYS_S = 0;

    static const unsigned char A_FFMT_CFG_RW = 0x15;
    static const unsigned char AFC_FFMT_ELE = 0x80;
    static const unsigned char AFC_FFMT_OAE = 0x40;
    static const unsigned char AFC_FFMT_ZEFE = 0x20;
    static const unsigned char AFC_FFMT_YEFE = 0x10;
    static const unsigned char AFC_FFMT_XEFE = 0x8;
    

    static const unsigned char A_FFMT_SRC_RO = 0x16;
    static const unsigned char AFS_FFMT_EA = 0x80;
    static const unsigned char AFS_FFMT_ZHE = 0x20;
    static const unsigned char AFS_FFMT_ZHP = 0x10;    
    static const unsigned char AFS_FFMT_YHE = 0x8;
    static const unsigned char AFS_FFMT_YHP = 0x4;    
    static const unsigned char AFS_FFMT_XHE = 0x2;
    static const unsigned char AFS_FFMT_XHP = 0x1;    

    
    static const unsigned char A_FFMT_THS_RW = 0x17;
    static const unsigned char AFT_DBCNTM = 0x80;
    static const unsigned char AFT_THS_M = 0x7f;
    static const unsigned char AFT_THS_S = 0;

    // The formats of these registers are hopelessly baroque
    // If we ever need this part, we'll fill them in then.
    static const unsigned char A_FFMT_THS_X_MSB_RW = 0x73;
    static const unsigned char A_FFMT_THS_X_LSB_RW = 0x74;
    static const unsigned char A_FFMT_THS_Y_MSB_RW = 0x75;
    static const unsigned char A_FFMT_THS_Y_LSB_RW = 0x76;
    static const unsigned char A_FFMT_THS_Z_MSB_RW = 0x77;
    static const unsigned char A_FFMT_THS_Z_LSB_RW = 0x78;
    
    static const unsigned char A_FFMT_COUNT_RW = 0x18;

    static const unsigned char TRANSIENT_CFG_RW = 0x1d;
    static const unsigned char TRANSIENT_SRC_RO = 0x1e;
    static const unsigned char TRANSIENT_THS_RW = 0x1f;
    static const unsigned char TRANSIENT_COUNT_RW = 0x20;
      
    static const unsigned char PULSE_CFG_RW = 0x21;
    static const unsigned char PULSE_SRC_RO = 0x22;
    static const unsigned char PULSE_THSX_RW = 0x23;
    static const unsigned char PULSE_THSY_RW = 0x24;
    static const unsigned char PULSE_THSZ_RW = 0x25;
    static const unsigned char PULSE_TMLT_RW = 0x26;
    static const unsigned char PULSE_LTCY_RW = 0x27;
    static const unsigned char PULSE_WIND_RW = 0x28;    

    static const unsigned char ASLP_COUNT_RW = 0x29;
    
    static const unsigned char CTRL_REG1_RW = 0x2a;
    static const unsigned char CR1_ASLP_RATE_M = 0x3;
    static const unsigned char CR1_ASLP_RATE_S = 6;
    static const unsigned char CR1_DR_M = 0x7;
    static const unsigned char CR1_DR_S = 3; 
    static const unsigned char CR1_LNOISE = 0x4;
    static const unsigned char CR1_F_READ = 0x2;
    static const unsigned char CR1_ACTIVE = 0x1;
    enum CR1_DATA_RATE {
			CR1_DATA_RATE_800 = 0,
			CR1_DATA_RATE_400 = 1,
			CR1_DATA_RATE_200 = 2,
			CR1_DATA_RATE_100 = 3,
			CR1_DATA_RATE_50 = 4,
			CR1_DATA_RATE_12r5 = 5,
			CR1_DATA_RATE_6r25 = 6,
			CR1_DATA_RATE_1r5625 = 7
    };


    
    static const unsigned char CTRL_REG2_RW = 0x2b;
    static const unsigned char CR2_STEST = 0x80;
    static const unsigned char CR2_RESET = 0x40;
    static const unsigned char CR2_SMODS_M = 0x3;
    static const unsigned char CR2_SMODS_S = 3;
    static const unsigned char CR2_SLPE = 0x40;
    static const unsigned char CR2_MODS_M = 0x3;
    static const unsigned char CR2_MODS_S = 0;
    enum CR2_MODS {
		   CR2_MODS_NORM = 0,
		   CR2_MODS_LN_LP = 1,
		   CR2_MODS_HI_RES = 2,
		   CR2_MODS_LP = 3
    };

    static const unsigned char CTRL_REG3_RW = 0x2c;
    static const unsigned char CR3_FIFO_GATE = 0x80;
    static const unsigned char CR3_WAKE_TRANS = 0x40;
    static const unsigned char CR3_WAKE_LNDPRT = 0x20;
    static const unsigned char CR3_WAKE_PULSE = 0x10;
    static const unsigned char CR3_WAKE_FFMT = 0x8;
    static const unsigned char CR3_WAKE_A_VECM = 0x4;
    static const unsigned char CR3_IPOL = 0x2;
    static const unsigned char CR3_PP_OD = 0x1;

    static const unsigned char CTRL_REG4_RW = 0x2d;
    static const unsigned char CR4_INT_EN_ASLP = 0x80;
    static const unsigned char CR4_INT_EN_FIFO = 0x40;
    static const unsigned char CR4_INT_EN_TRANS = 0x20;    
    static const unsigned char CR4_INT_EN_LNDPRT = 0x10;
    static const unsigned char CR4_INT_EN_PULSE = 0x8;
    static const unsigned char CR4_INT_EN_FFMT = 0x4;
    static const unsigned char CR4_INT_EN_A_VECM = 0x2;
    static const unsigned char CR4_INT_EN_DRDY = 0x1;
    
    static const unsigned char CTRL_REG5_RW = 0x2e;
    static const unsigned char CR5_INT_CFG_ASLP = 0x80;
    static const unsigned char CR5_INT_CFG_FIFO = 0x40;
    static const unsigned char CR5_INT_CFG_TRANS = 0x20;    
    static const unsigned char CR5_INT_CFG_LNDPRT = 0x10;
    static const unsigned char CR5_INT_CFG_PULSE = 0x8;
    static const unsigned char CR5_INT_CFG_FFMT = 0x4;
    static const unsigned char CR5_INT_CFG_A_VECM = 0x2;
    static const unsigned char CR5_INT_CFG_DRDY = 0x1;

    static const unsigned char OFF_X_RW = 0x2f;
    static const unsigned char OFF_Y_RW = 0x30;
    static const unsigned char OFF_Z_RW = 0x31;

    static const unsigned char M_OUT_X_MSB_RO = 0x33;
    static const unsigned char M_OUT_X_LSB_RO = 0x34;
    static const unsigned char M_OUT_Y_MSB_RO = 0x35;
    static const unsigned char M_OUT_Y_LSB_RO = 0x36;
    static const unsigned char M_OUT_Z_MSB_RO = 0x37;
    static const unsigned char M_OUT_Z_LSB_RO = 0x38;

    static const unsigned char CMP_X_MSB_RO = 0x39;
    static const unsigned char CMP_X_LSB_RO = 0x3a;
    static const unsigned char CMP_Y_MSB_RO = 0x3b;
    static const unsigned char CMP_Y_LSB_RO = 0x3c;
    static const unsigned char CMP_Z_MSB_RO = 0x3d;
    static const unsigned char CMP_Z_LSB_RO = 0x3e;

    static const unsigned char M_OFF_X_MSB_RW = 0x3f;
    static const unsigned char M_OFF_X_LSB_RW = 0x40;
    static const unsigned char M_OFF_Y_MSB_RW = 0x41;
    static const unsigned char M_OFF_Y_LSB_RW = 0x42;
    static const unsigned char M_OFF_Z_MSB_RW = 0x43;
    static const unsigned char M_OFF_Z_LSB_RW = 0x44;

    static const unsigned char MAX_X_MSB_RO = 0x45;
    static const unsigned char MAX_X_LSB_RO = 0x46;
    static const unsigned char MAX_Y_MSB_RO = 0x47;
    static const unsigned char MAX_Y_LSB_RO = 0x48;
    static const unsigned char MAX_Z_MSB_RO = 0x49;
    static const unsigned char MAX_Z_LSB_RO = 0x4a;

    static const unsigned char MIN_X_MSB_RO = 0x4b;
    static const unsigned char MIN_X_LSB_RO = 0x4c;
    static const unsigned char MIN_Y_MSB_RO = 0x4d;
    static const unsigned char MIN_Y_LSB_RO = 0x4e;
    static const unsigned char MIN_Z_MSB_RO = 0x4f;
    static const unsigned char MIN_Z_LSB_RO = 0x50;

    static const unsigned char TEMP_RO = 0x51;

    static const unsigned char M_THS_CFG_RW = 0x52;
    static const unsigned char M_THS_SRC_RO = 0x53;

    static const unsigned char M_THS_X_MSB_RW = 0x54;
    static const unsigned char M_THS_X_LSB_RW = 0x55;
    static const unsigned char M_THS_Y_MSB_RW = 0x56;
    static const unsigned char M_THS_Y_LSB_RW = 0x57;
    static const unsigned char M_THS_Z_MSB_RW = 0x58;
    static const unsigned char M_THS_Z_LSB_RW = 0x59;

    static const unsigned char M_THS_COUNT_RW = 0x5a;

    static const unsigned char M_CTRL_REG1_RW = 0x5b;
    static const unsigned char MCR1_ACAL_ENA = 0x80;
    static const unsigned char MCR1_RESET = 0x40;
    static const unsigned char MCR1_OST = 0x20;
    enum MCR1_HMS {
		   ACC_ONLY = 0,
		   MAG_ONLY = 1,
		   BOTH = 3
    };
    static const unsigned char MCR1_OSR_M = 0x7;
    static const unsigned char MCR1_OSR_S = 2;

    
    static const unsigned char M_CTRL_REG2_RW = 0x5c;
    static const unsigned char MCR2_HYB_AUTO_INC = 0x20;
    static const unsigned char MCR2_MAXMIN_DIS = 0x10;
    static const unsigned char MCR2_MAXMIN_DIS_THS = 0x8;
    static const unsigned char MCR2_MAXMIN_RST = 0x4;
    enum MCR2_RST_CNT {
		       ODR_RESET = 0,
		       ODR16_RESET = 1,
		       ODR512_RESET = 2,
		       NO_AUTO_RESET = 3
    };
    static const unsigned char MCR2_RST_CNT_M = 3;
    static const unsigned char MCR2_RST_CNT_S = 0;

  
    static const unsigned char M_CTRL_REG3_RW = 0x5d;
    static const unsigned char MCR3_RAW = 0x80;
    static const unsigned char MCR3_ASLP_OS_M = 0x7;
    static const unsigned char MCR3_ASLP_OS_S = 4;
    static const unsigned char MCR3_THS_XYZ_UPDATE = 0x8;

    
    static const unsigned char M_INT_SRC_RO = 0x5e;
    static const unsigned char MIS_THS = 0x4;
    static const unsigned char MIS_VECM = 0x2;
    static const unsigned char MIS_DRDY = 0x1;

    // We'll skip these until we need them.
    static const unsigned char A_VECM_CFG_RW = 0x5f;
    static const unsigned char A_VECM_THS_MSB_RW = 0x60;
    static const unsigned char A_VECM_THS_LSB_RW = 0x61;
    static const unsigned char A_VECM_CNT_RW = 0x62;

    static const unsigned char A_VECM_INITX_MSB_RW = 0x63;
    static const unsigned char A_VECM_INITX_LSB_RW = 0x64;
    static const unsigned char A_VECM_INITY_MSB_RW = 0x65;
    static const unsigned char A_VECM_INITY_LSB_RW = 0x66;
    static const unsigned char A_VECM_INITZ_MSB_RW = 0x67;
    static const unsigned char A_VECM_INITZ_LSB_RW = 0x68;

    static const unsigned char M_VECM_CFG_RW = 0x69;
    static const unsigned char M_VECM_THS_MSB_RW = 0x6a;
    static const unsigned char M_VECM_THS_LSB_RW = 0x6b;
    static const unsigned char M_VECM_CNT_RW = 0x6c;

    static const unsigned char M_VECM_INITX_MSB_RW = 0x6d;
    static const unsigned char M_VECM_INITX_LSB_RW = 0x6e;
    static const unsigned char M_VECM_INITY_MSB_RW = 0x6f;
    static const unsigned char M_VECM_INITY_LSB_RW = 0x70;
    static const unsigned char M_VECM_INITZ_MSB_RW = 0x71;
    static const unsigned char M_VECM_INITZ_LSB_RW = 0x72;

    int readFIFO();

    int readDR(ISData & raw);

    void init(Mode mode, unsigned char int1_pin);

    void serviceDReady(int gpio, int level, unsigned int tick);

  public:

    int readDR(MXData & dat);    
    void start();

  protected:
    static void fifoIntCallback(int gpio, int level, unsigned int tick, void * obj);

    static void dReadyIntCallback(int gpio, int level, unsigned int tick, void * obj);
    
  };

}
