#pragma once

#include <mutex>
#include <queue>
#include <iostream>

namespace BlackBox {

  class PIIO;
  
#pragma pack(push, 1)    
  // This version of the Rate struct contains a sequence
  // number as well as the gyro rates.
  struct Rates {
    float x, y, z; 
    unsigned short seq_no; 

    std::ostream & print(std::ostream & os);
    static std::ostream & printFormat(std::ostream & os);
  };
#pragma pack(pop)    
  
  class FXAS21002C {
  public:
    enum Mode { FIFO, DR_INT, DR_POLL };    
    
    /**
     *
     * @param bus which i2c bus is the gyro on?  (1)
     * @param addr i2c address for the gyro
     * @param int1_pin first of two int pins from the chip
     * @param int2_pin second of two int pins from the chip
     */
    FXAS21002C(PIIO * piio_p, unsigned char bus, unsigned char addr,
	       unsigned char int1_pin, 
	       Mode mode);
    
    ~FXAS21002C();

    int getRates(int max_samples, Rates * samps);

    void stop() { }
    
  protected:

    
#pragma pack(push, 1)
    // want this in a contiguous 6 byte block.
    // This is the structure that mimics the layout of the
    // registers. 
    struct IRates {
      unsigned short x, y, z;
    }; 
#pragma pack(pop)

    short swapEnds(unsigned short v);
    
    float scale_factor; // in degrees/second per count
    
    void scaleBlock(const IRates & irate, Rates & out); 

    std::queue<Rates> gyro_rates;
    IRates rate_block[256];
    unsigned short sequence_number;
    static const int GYRO_RATE_QUEUE_MAXLEN = 1024;
    std::mutex gq_mutex;

    
    // register definitions, and bit fields.
    static const unsigned char STATUS_RO = 0x0;

    static const unsigned char OUT_X_MSB_RO = 0x1;
    static const unsigned char OUT_X_LSB_RO = 0x2;    
    static const unsigned char OUT_Y_MSB_RO = 0x3;
    static const unsigned char OUT_Y_LSB_RO = 0x4;    
    static const unsigned char OUT_Z_MSB_RO = 0x5;
    static const unsigned char OUT_Z_LSB_RO = 0x6;    

    static const unsigned char DR_STATUS_RO = 0x7;
    static const unsigned char DRS_ZYXOW = 0x80;
    static const unsigned char DRS_ZOW = 0x40;
    static const unsigned char DRS_YOW = 0x20;
    static const unsigned char DRS_XOW = 0x10;        
    static const unsigned char DRS_ZYXDR = 0x8;
    static const unsigned char DRS_ZDR = 0x4;
    static const unsigned char DRS_YDR = 0x2;
    static const unsigned char DRS_XDR = 0x1;        

    static const unsigned char F_STATUS_RO = 0x8;
    static const unsigned char FST_F_OVF = 0x80;
    static const unsigned char FST_F_WMKF = 0x40;
    static const unsigned char FST_F_COUNT_M = 0x3f;
    
    static const unsigned char F_SETUP_RW = 0x9;
    static const unsigned char FS_MODE_DIS = 0;
    static const unsigned char FS_MODE_CIR = 0x40;
    static const unsigned char FS_MODE_STOP = 0x80;
    static const unsigned char FS_WMRK_M = 0x3f;
    
    static const unsigned char F_EVENT_RO = 0xA;
    static const unsigned char FE_EVENT = 0x20;
    static const unsigned char FE_TIME_M = 0x1f;
    
    static const unsigned char INT_SRC_RO = 0xB;
    static const unsigned char IS_BOOTEND = 0x8;
    static const unsigned char IS_FIFO = 0x4;
    static const unsigned char IS_RT = 0x2;
    static const unsigned char IS_DRDY = 0x1;

    
    static const unsigned char WHO_AM_I_RO = 0xC;

    // -----------------------------------
    static const unsigned char CTRL_REG0_RW = 0xD;
    static const unsigned char CR0_LPF_H = 0x0;
    static const unsigned char CR0_LPF_M = 0x40;
    static const unsigned char CR0_LPF_L = 0x80;
    static const unsigned char CR0_HPF_0 = 0x0;
    static const unsigned char CR0_HPF_1 = 0x8;
    static const unsigned char CR0_HPF_2 = 0x10;
    static const unsigned char CR0_HPF_3 = 0x18;    
    static const unsigned char CR0_HPF_EN = 0x4;

    static const unsigned char CR0_SPI_4W = 0x00;
    static const unsigned char CR0_SPI_3W = 0x20;
    
    static const unsigned char CR0_RANGE_2000 = 0x0;
    static const unsigned char CR0_RANGE_1000 = 0x1;
    static const unsigned char CR0_RANGE_500 = 0x2;
    static const unsigned char CR0_RANGE_250 = 0x3;    

    // -----------------------------------    
    static const unsigned char RT_CFG_RW = 0xE;
    static const unsigned char RTC_ELE = 0x8;
    static const unsigned char RTC_ZTEFE = 0x4;
    static const unsigned char RTC_YTEFE = 0x2;
    static const unsigned char RTC_XTEFE = 0x1;

    // -----------------------------------    
    static const unsigned char RT_SRC_RO = 0xF;
    static const unsigned char RTS_EA = 0x40;
    static const unsigned char RTS_ZRT = 0x20;
    static const unsigned char RTS_ZRT_POL = 0x10;
    static const unsigned char RTS_YRT = 0x8;
    static const unsigned char RTS_YRT_POL = 0x4;
    static const unsigned char RTS_XRT = 0x2;
    static const unsigned char RTS_XRT_POL = 0x1;

    // -----------------------------------        
    static const unsigned char RT_THS_RW = 0x10;
    static const unsigned char RTT_DBCNTM = 0x80;
    static const unsigned char RTT_THS_M = 0x7f;

    // -----------------------------------        
    static const unsigned char RT_COUNT_RW = 0x11;    

    // -----------------------------------            
    static const unsigned char TEMP_RO = 0x12;    

    // -----------------------------------            
    static const unsigned char CTRL_REG1_RW = 0x13;
    static const unsigned char CR1_RESET = 0x40;
    static const unsigned char CR1_SELF_TEST = 0x20;

    static const unsigned char CR1_ACTIVE = 0x2;
    static const unsigned char CR1_READY = 0x1;
    
    // -----------------------------------
    static const unsigned char CTRL_REG2_RW = 0x14;
    static const unsigned char CR2_INT_CFG_FIFO_I2 = 0;
    static const unsigned char CR2_INT_CFG_FIFO_I1 = 0x80;
    static const unsigned char CR2_INT_EN_FIFO = 0x40;
    static const unsigned char CR2_INT_CFG_RT_I2 = 0x0;
    static const unsigned char CR2_INT_CFG_RT_I1 = 0x20;
    static const unsigned char CR2_INT_EN_RT = 0x10;
    static const unsigned char CR2_INT_CFG_DRDY_I2 = 0;
    static const unsigned char CR2_INT_CFG_DRDY_I1 = 0x8;    
    static const unsigned char CR2_INT_EN_DRDY = 0x4;
    static const unsigned char CR2_IPOL_LO = 0;
    static const unsigned char CR2_IPOL_HI = 0x2;     
    static const unsigned char CR2_PP_OD = 0x1; 

    // -----------------------------------
    static const unsigned char CTRL_REG3_RW = 0x15;
    static const unsigned char CR3_WRAPTOONE = 0x8;
    static const unsigned char CR3_EXTCTRLEN = 0x4;
    static const unsigned char CR3_FS_DOUBLE = 0x1;

    void writeByte(unsigned char reg, unsigned char dat);
    unsigned char readByte(unsigned char reg);

    void readBlock(unsigned char reg, int len, char * buf);

    int readFIFO();

    int readDR(IRates & rates);


    void init(Mode mode, unsigned char int1_pin);

    void serviceFIFO(int gpio, int level, unsigned int tick);
    void serviceDReady(int gpio, int level, unsigned int tick);
    

  public:

    enum CR1_DATA_RATE {
			CR1_DATA_RATE_800 = 0x0,
			CR1_DATA_RATE_400 = 0x4,
			CR1_DATA_RATE_200 = 0x8,
			CR1_DATA_RATE_100 = 0xc,
			CR1_DATA_RATE_50 = 0x10,
			CR1_DATA_RATE_25 = 0x14,
			CR1_DATA_RATE_12r5 = 0x18,
			CR1_DATA_RATE_XXX = 0x1c
    };
    
    void start(CR1_DATA_RATE data_rate);

  protected:
    PIIO * piio_p; 
    Mode mode;
    unsigned char int1_pin; 
    
    static void fifoIntCallback(int gpio, int level, unsigned int tick, void * obj);

    static void dReadyIntCallback(int gpio, int level, unsigned int tick, void * obj);
  };
}
