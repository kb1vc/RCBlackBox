#pragma once

namespace BlackBox {
  class FXBase {
  public:
    FXBase::FXBase(unsigned char bus, unsigned char base);
    void writeByte(unsigned char reg, unsigned char dat);

    unsigned char readByte(unsigned char reg);

    void readBlock(unsigned char reg, int len, char * buf);

  protected:
    int i2c_handle;   
  };

}
